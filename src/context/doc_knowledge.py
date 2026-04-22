"""
Documentation Knowledge Module for LogicFuzz.

Provides RAG-based retrieval of project documentation to enhance
LLM understanding when generating fuzz drivers.

Supports multiple document formats:
- Header files with Doxygen comments (.h)
- Markdown documentation (.md)
- Plain text files (.txt)
- HTML documentation (.html, .htm)
- PDF documents (.pdf)
- reStructuredText (.rst)
- AsciiDoc (.adoc)

Architecture:
============
1. Document Discovery: Scans document_paths from project config
2. Document Loading: Parses different formats into text chunks
3. Embedding & Indexing: Creates vector embeddings in ChromaDB
4. Retrieval: Semantic search for relevant documentation excerpts
"""

import hashlib
import json
import logging
import os
import re
import zipfile
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any, Dict, List, Optional, Tuple

import chromadb
from chromadb.config import Settings

# Try to import optional PDF support
try:
    import pypdf
    HAS_PYPDF = True
except ImportError:
    HAS_PYPDF = False


@dataclass
class DocumentExcerpt:
    """A retrieved documentation excerpt with metadata."""
    content: str
    source: str  # File path or URL
    relevance_score: float = 0.0

    def __str__(self) -> str:
        return f"[{self.source}]\n{self.content}"


@dataclass
class ParameterConstraint:
    """Constraint on an API parameter extracted from documentation.

    Captures semantic constraints that help generate correct fuzz drivers.
    """
    param_name: str
    constraints: List[str]  # e.g., ["must not be NULL", "size > 0", "valid UTF-8"]
    ownership: str = ""     # "caller_frees", "callee_frees", "borrowed", ""
    default_value: str = "" # If documented


@dataclass
class APISemantics:
    """Structured semantics of an API extracted from documentation.

    This captures the "WHAT" and "WHY" from documentation:
    - What does the API do?
    - What constraints must be satisfied?
    - What does the return value mean?
    """
    function_name: str
    brief_description: str = ""

    # Parameter constraints
    parameter_constraints: Dict[str, ParameterConstraint] = field(default_factory=dict)

    # Return value semantics
    return_description: str = ""
    return_ownership: str = ""  # "caller_frees", "static", "internal", ""
    error_return_value: str = ""  # e.g., "NULL on error", "-1 on failure"

    # Preconditions and postconditions
    preconditions: List[str] = field(default_factory=list)  # "must call X before"
    postconditions: List[str] = field(default_factory=list)  # "resource is initialized"

    # Related APIs
    init_api: str = ""      # API that must be called before this one
    cleanup_api: str = ""   # API that must be called after this one

    # Source documentation excerpts (for reference)
    source_excerpts: List[str] = field(default_factory=list)

    def to_prompt_text(self) -> str:
        """Format for inclusion in LLM prompt."""
        lines = [f"**{self.function_name}**"]
        if self.brief_description:
            lines.append(f"  Purpose: {self.brief_description}")

        if self.parameter_constraints:
            lines.append("  Parameters:")
            for param, constraint in self.parameter_constraints.items():
                constraint_str = ", ".join(constraint.constraints) if constraint.constraints else "no constraints"
                ownership_str = f" [{constraint.ownership}]" if constraint.ownership else ""
                lines.append(f"    - {param}: {constraint_str}{ownership_str}")

        if self.error_return_value:
            lines.append(f"  Returns: {self.error_return_value}")
        if self.return_ownership:
            lines.append(f"  Return ownership: {self.return_ownership}")

        if self.preconditions:
            lines.append(f"  Preconditions: {'; '.join(self.preconditions)}")
        if self.cleanup_api:
            lines.append(f"  Cleanup: call {self.cleanup_api} after use")

        return "\n".join(lines)


@dataclass
class DocumentKnowledge:
    """Container for project documentation knowledge.

    Attributes:
        library_purpose: One-sentence description of the library's purpose
        function_docs: Dict mapping function names to their documentation
        api_semantics: Dict mapping function names to structured APISemantics
        api_usage_examples: List of usage examples from documentation
        data_type_docs: Dict mapping type names to their documentation
        excerpts_by_query: Cache of retrieved excerpts by query string
    """
    library_purpose: str = ""
    function_docs: Dict[str, str] = field(default_factory=dict)
    api_semantics: Dict[str, APISemantics] = field(default_factory=dict)  # NEW: structured semantics
    api_usage_examples: List[str] = field(default_factory=list)
    data_type_docs: Dict[str, str] = field(default_factory=dict)
    excerpts_by_query: Dict[str, List[DocumentExcerpt]] = field(default_factory=dict)


# Supported document extensions
DOCUMENT_EXTENSIONS = {
    '.h', '.hpp', '.hxx',  # C/C++ headers with Doxygen
    '.md', '.markdown',    # Markdown
    '.txt',                # Plain text
    '.html', '.htm',       # HTML
    '.pdf',                # PDF (requires pypdf)
    '.rst',                # reStructuredText
    '.adoc',               # AsciiDoc
}

# Files to auto-discover (case insensitive)
AUTO_DISCOVER_NAMES = {'readme', 'usage', 'api', 'guide', 'manual', 'doc', 'documentation'}


class DoxygenParser:
    """Parser for Doxygen-formatted documentation in header files."""

    # Doxygen command patterns
    DOXYGEN_PATTERNS = {
        'brief': re.compile(r'@(?:brief|short)\s+(.+?)(?=@|\*/|$)', re.DOTALL),
        'param': re.compile(r'@param\s+(\w+)\s+(.+?)(?=@|\*/|$)', re.DOTALL),
        'return': re.compile(r'@(?:return|returns)\s+(.+?)(?=@|\*/|$)', re.DOTALL),
        'section': re.compile(r'@(?:section|subsection)\s+\w+\s+(.+?)\n', re.DOTALL),
        'page': re.compile(r'@page\s+\w+\s+(.+?)\n', re.DOTALL),
    }

    @classmethod
    def parse_file(cls, filepath: str) -> List[Dict[str, Any]]:
        """Parse a Doxygen-documented header file.

        Returns list of documentation blocks with extracted information.
        """
        try:
            with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
        except Exception:
            return []

        docs = []

        # Extract Doxygen comment blocks
        pattern = re.compile(r'/\*\*(.+?)\*/', re.DOTALL)
        for match in pattern.finditer(content):
            block = match.group(1)
            doc = cls._parse_block(block)
            if doc.get('content'):
                doc['source'] = filepath
                doc['offset'] = match.start()
                docs.append(doc)

        return docs

    @classmethod
    def _parse_block(cls, block: str) -> Dict[str, Any]:
        """Parse a single Doxygen comment block."""
        doc = {'content': '', 'params': {}, 'return': '', 'type': 'unknown'}

        # Clean up block (remove leading * and whitespace)
        lines = block.split('\n')
        cleaned_lines = []
        for line in lines:
            line = line.strip()
            if line.startswith('*'):
                line = line[1:].strip()
            cleaned_lines.append(line)
        cleaned_block = '\n'.join(cleaned_lines)

        # Extract brief/short description
        if match := cls.DOXYGEN_PATTERNS['brief'].search(cleaned_block):
            doc['content'] = match.group(1).strip()

        # Extract page title
        if match := cls.DOXYGEN_PATTERNS['page'].search(cleaned_block):
            doc['type'] = 'page'
            doc['title'] = match.group(1).strip()

        # Extract section title
        if match := cls.DOXYGEN_PATTERNS['section'].search(cleaned_block):
            doc['type'] = 'section'
            doc['title'] = match.group(1).strip()

        # Extract parameters
        for match in cls.DOXYGEN_PATTERNS['param'].finditer(cleaned_block):
            param_name = match.group(1).strip()
            param_desc = match.group(2).strip()
            doc['params'][param_name] = param_desc

        # Extract return value
        if match := cls.DOXYGEN_PATTERNS['return'].search(cleaned_block):
            doc['return'] = match.group(1).strip()

        # If no brief, use the whole cleaned content
        if not doc['content']:
            # Remove Doxygen commands and use plain text
            plain_text = re.sub(r'@\w+\s*', '', cleaned_block)
            plain_text = re.sub(r'<[^>]+>', '', plain_text)  # Remove HTML tags
            doc['content'] = plain_text.strip()[:500]  # Limit length

        return doc


class DocumentLoader:
    """Loads and parses documents from various formats."""

    def __init__(self, chunk_size: int = 1000, chunk_overlap: int = 200):
        self.chunk_size = chunk_size
        self.chunk_overlap = chunk_overlap
        self.logger = logging.getLogger(__name__)

    def load_document(self, filepath: str) -> List[Dict[str, str]]:
        """Load a document and return list of text chunks with metadata.

        Returns:
            List of dicts with 'content' and 'source' keys
        """
        filepath = os.path.abspath(filepath)
        ext = os.path.splitext(filepath)[1].lower()

        if ext in {'.h', '.hpp', '.hxx'}:
            return self._load_header(filepath)
        elif ext in {'.md', '.markdown'}:
            return self._load_markdown(filepath)
        elif ext == '.pdf':
            return self._load_pdf(filepath)
        elif ext in {'.html', '.htm'}:
            return self._load_html(filepath)
        elif ext in {'.txt', '.rst', '.adoc'}:
            return self._load_text(filepath)
        else:
            self.logger.warning(f"Unsupported document format: {ext}")
            return []

    def _load_header(self, filepath: str) -> List[Dict[str, str]]:
        """Load C/C++ header with Doxygen documentation."""
        docs = DoxygenParser.parse_file(filepath)
        chunks = []

        for doc in docs:
            content = doc.get('content', '')
            if doc.get('params'):
                params_text = '\n'.join([f"  {k}: {v}" for k, v in doc['params'].items()])
                content += f"\nParameters:\n{params_text}"
            if doc.get('return'):
                content += f"\nReturns: {doc['return']}"

            if content.strip():
                chunks.append({
                    'content': content.strip(),
                    'source': f"{filepath}:{doc.get('offset', 0)}"
                })

        return chunks

    def _load_markdown(self, filepath: str) -> List[Dict[str, str]]:
        """Load and chunk Markdown document."""
        try:
            with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
        except Exception as e:
            self.logger.warning(f"Failed to read {filepath}: {e}")
            return []

        # Split by headers for semantic chunking
        sections = re.split(r'\n(?=#+\s)', content)
        chunks = []

        for section in sections:
            if not section.strip():
                continue
            # Further chunk if section is too large
            if len(section) > self.chunk_size:
                sub_chunks = self._chunk_text(section, filepath)
                chunks.extend(sub_chunks)
            else:
                chunks.append({
                    'content': section.strip(),
                    'source': filepath
                })

        return chunks

    def _load_pdf(self, filepath: str) -> List[Dict[str, str]]:
        """Load and chunk PDF document."""
        if not HAS_PYPDF:
            self.logger.warning("pypdf not installed, skipping PDF: %s", filepath)
            return []

        try:
            chunks = []
            reader = pypdf.PdfReader(filepath)

            for page_num, page in enumerate(reader.pages):
                text = page.extract_text()
                if text and text.strip():
                    page_chunks = self._chunk_text(text, f"{filepath}:page_{page_num + 1}")
                    chunks.extend(page_chunks)

            return chunks
        except Exception as e:
            self.logger.warning(f"Failed to read PDF {filepath}: {e}")
            return []

    def _load_html(self, filepath: str) -> List[Dict[str, str]]:
        """Load HTML document and extract text."""
        try:
            with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
        except Exception as e:
            self.logger.warning(f"Failed to read {filepath}: {e}")
            return []

        # Simple HTML tag removal
        text = re.sub(r'<script[^>]*>.*?</script>', '', content, flags=re.DOTALL)
        text = re.sub(r'<style[^>]*>.*?</style>', '', text, flags=re.DOTALL)
        text = re.sub(r'<[^>]+>', ' ', text)
        text = re.sub(r'\s+', ' ', text).strip()

        return self._chunk_text(text, filepath)

    def _load_text(self, filepath: str) -> List[Dict[str, str]]:
        """Load plain text file."""
        try:
            with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
        except Exception as e:
            self.logger.warning(f"Failed to read {filepath}: {e}")
            return []

        return self._chunk_text(content, filepath)

    def _chunk_text(self, text: str, source: str) -> List[Dict[str, str]]:
        """Split text into overlapping chunks."""
        if len(text) <= self.chunk_size:
            return [{'content': text.strip(), 'source': source}]

        chunks = []
        start = 0
        while start < len(text):
            end = start + self.chunk_size
            chunk = text[start:end]

            # Try to break at sentence or paragraph boundary
            if end < len(text):
                # Look for paragraph break
                para_break = chunk.rfind('\n\n')
                if para_break > self.chunk_size // 2:
                    chunk = chunk[:para_break]
                    end = start + para_break
                else:
                    # Look for sentence break
                    sent_break = max(chunk.rfind('. '), chunk.rfind('.\n'))
                    if sent_break > self.chunk_size // 2:
                        chunk = chunk[:sent_break + 1]
                        end = start + sent_break + 1

            if chunk.strip():
                chunks.append({'content': chunk.strip(), 'source': source})

            start = end - self.chunk_overlap
            if start >= len(text):
                break

        return chunks


class DocumentKnowledgeManager:
    """Manages documentation knowledge with RAG-based retrieval.

    Uses ChromaDB for vector storage and semantic search.
    Optionally uses LLM for summarization and excerpt filtering (PromeFuzz-style).
    """

    def __init__(
        self,
        project_name: str,
        persist_dir: Optional[str] = None,
        embedding_model: str = "all-MiniLM-L6-v2",
        logger_instance: Optional[logging.Logger] = None,
        llm_adapter: Optional[Any] = None
    ):
        """Initialize the document knowledge manager.

        Args:
            project_name: Name of the project
            persist_dir: Directory to persist ChromaDB (None for in-memory)
            embedding_model: Name of embedding model (default: sentence-transformers)
            logger_instance: Optional logger
            llm_adapter: Optional LLM adapter for summarization/filtering
        """
        self.project_name = project_name
        self.persist_dir = persist_dir
        self.embedding_model = embedding_model
        self.logger = logger_instance or logging.getLogger(__name__)
        self.loader = DocumentLoader()
        self.llm_adapter = llm_adapter  # For LLM-based summarization

        # Initialize ChromaDB
        if persist_dir:
            os.makedirs(persist_dir, exist_ok=True)
            self.client = chromadb.PersistentClient(
                path=persist_dir,
                settings=Settings(anonymized_telemetry=False)
            )
        else:
            self.client = chromadb.Client(
                settings=Settings(anonymized_telemetry=False)
            )

        # Get or create collection for this project
        collection_name = f"logicfuzz_{project_name}"[:63]  # ChromaDB name limit
        self.collection = self.client.get_or_create_collection(
            name=collection_name,
            metadata={"project": project_name}
        )

        # Track indexed documents
        self.indexed_docs_file = os.path.join(persist_dir, "indexed_docs.json") if persist_dir else None
        self.indexed_docs: Dict[str, str] = {}  # path -> hash
        self._load_indexed_docs()

        # Knowledge container
        self.knowledge = DocumentKnowledge()

    def _load_indexed_docs(self):
        """Load list of already-indexed documents."""
        if self.indexed_docs_file and os.path.exists(self.indexed_docs_file):
            try:
                with open(self.indexed_docs_file, 'r') as f:
                    self.indexed_docs = json.load(f)
            except Exception:
                self.indexed_docs = {}

    def _save_indexed_docs(self):
        """Save list of indexed documents."""
        if self.indexed_docs_file:
            try:
                with open(self.indexed_docs_file, 'w') as f:
                    json.dump(self.indexed_docs, f, indent=2)
            except Exception as e:
                self.logger.warning(f"Failed to save indexed docs: {e}")

    def _file_hash(self, filepath: str) -> str:
        """Compute hash of file content for change detection."""
        try:
            with open(filepath, 'rb') as f:
                return hashlib.md5(f.read()).hexdigest()
        except Exception:
            return ""

    def discover_documents(
        self,
        document_paths: List[str],
        exclude_patterns: Optional[List[str]] = None
    ) -> List[str]:
        """Discover all documents from configured paths.

        Args:
            document_paths: List of file paths, directories, or zip files
            exclude_patterns: Patterns to exclude (glob-style)

        Returns:
            List of discovered document file paths
        """
        exclude_patterns = exclude_patterns or []
        discovered = []

        for path in document_paths:
            path = os.path.abspath(path)

            if not os.path.exists(path):
                self.logger.warning(f"Document path does not exist: {path}")
                continue

            if os.path.isfile(path):
                if path.endswith('.zip'):
                    # Extract and discover from zip
                    discovered.extend(self._discover_from_zip(path))
                elif self._is_document(path):
                    discovered.append(path)
            elif os.path.isdir(path):
                discovered.extend(self._discover_from_directory(path, exclude_patterns))

        return discovered

    def _is_document(self, filepath: str) -> bool:
        """Check if file is a supported document type."""
        ext = os.path.splitext(filepath)[1].lower()
        if ext in DOCUMENT_EXTENSIONS:
            return True

        # Check for auto-discover names
        basename = os.path.basename(filepath).lower()
        name_without_ext = os.path.splitext(basename)[0]
        return name_without_ext in AUTO_DISCOVER_NAMES

    def _discover_from_directory(
        self,
        directory: str,
        exclude_patterns: List[str]
    ) -> List[str]:
        """Recursively discover documents in a directory."""
        discovered = []

        for root, dirs, files in os.walk(directory):
            # Skip hidden directories
            dirs[:] = [d for d in dirs if not d.startswith('.')]

            for filename in files:
                filepath = os.path.join(root, filename)

                # Check exclusions
                excluded = False
                for pattern in exclude_patterns:
                    if re.search(pattern, filepath):
                        excluded = True
                        break

                if not excluded and self._is_document(filepath):
                    discovered.append(filepath)

        return discovered

    def _discover_from_zip(self, zip_path: str) -> List[str]:
        """Extract and discover documents from a zip file."""
        extract_dir = os.path.join(
            os.path.dirname(zip_path),
            os.path.splitext(os.path.basename(zip_path))[0] + "_extracted"
        )

        try:
            with zipfile.ZipFile(zip_path, 'r') as zf:
                zf.extractall(extract_dir)
            return self._discover_from_directory(extract_dir, [])
        except Exception as e:
            self.logger.warning(f"Failed to extract zip {zip_path}: {e}")
            return []

    def index_documents(self, document_paths: List[str]) -> int:
        """Index documents into ChromaDB.

        Args:
            document_paths: List of paths to documents, directories, or zips

        Returns:
            Number of new chunks indexed
        """
        # Discover all documents
        all_docs = self.discover_documents(document_paths)
        self.logger.info(f"Discovered {len(all_docs)} documents to index")

        chunks_indexed = 0

        for doc_path in all_docs:
            # Check if already indexed with same hash
            current_hash = self._file_hash(doc_path)
            if doc_path in self.indexed_docs and self.indexed_docs[doc_path] == current_hash:
                continue

            # Load and chunk document
            chunks = self.loader.load_document(doc_path)

            if not chunks:
                continue

            # Generate unique IDs for chunks
            ids = [f"{doc_path}_{i}" for i in range(len(chunks))]
            documents = [c['content'] for c in chunks]
            metadatas = [{'source': c['source']} for c in chunks]

            # Add to collection (ChromaDB handles embedding)
            try:
                self.collection.add(
                    documents=documents,
                    metadatas=metadatas,
                    ids=ids
                )
                chunks_indexed += len(chunks)
                self.indexed_docs[doc_path] = current_hash
                self.logger.debug(f"Indexed {len(chunks)} chunks from {doc_path}")
            except Exception as e:
                self.logger.warning(f"Failed to index {doc_path}: {e}")

        self._save_indexed_docs()
        self.logger.info(f"Indexed {chunks_indexed} new document chunks")

        return chunks_indexed

    def retrieve(
        self,
        query: str,
        top_k: int = 3,
        min_score: float = 0.0
    ) -> List[DocumentExcerpt]:
        """Retrieve relevant documentation excerpts.

        Args:
            query: Search query (e.g., function name, concept)
            top_k: Number of results to return
            min_score: Minimum relevance score threshold

        Returns:
            List of DocumentExcerpt objects
        """
        # Check cache
        cache_key = f"{query}_{top_k}"
        if cache_key in self.knowledge.excerpts_by_query:
            return self.knowledge.excerpts_by_query[cache_key]

        try:
            results = self.collection.query(
                query_texts=[query],
                n_results=top_k,
                include=['documents', 'metadatas', 'distances']
            )
        except Exception as e:
            self.logger.warning(f"Document retrieval failed: {e}")
            return []

        excerpts = []
        if results['documents'] and results['documents'][0]:
            for i, doc in enumerate(results['documents'][0]):
                metadata = results['metadatas'][0][i] if results['metadatas'] else {}
                distance = results['distances'][0][i] if results['distances'] else 1.0

                # Convert distance to similarity score (ChromaDB uses L2 distance)
                score = 1.0 / (1.0 + distance)

                if score >= min_score:
                    excerpts.append(DocumentExcerpt(
                        content=doc,
                        source=metadata.get('source', 'unknown'),
                        relevance_score=score
                    ))

        # Cache results
        self.knowledge.excerpts_by_query[cache_key] = excerpts

        return excerpts

    def retrieve_for_function(
        self,
        function_name: str,
        top_k: int = 3
    ) -> List[DocumentExcerpt]:
        """Retrieve documentation related to a specific function.

        Args:
            function_name: Name of the function to search for
            top_k: Number of results

        Returns:
            List of relevant documentation excerpts
        """
        # Try multiple query variations
        queries = [
            f"function {function_name}",
            f"{function_name} usage",
            f"{function_name} API",
            function_name
        ]

        all_excerpts = []
        seen_content = set()

        for query in queries:
            excerpts = self.retrieve(query, top_k=top_k)
            for exc in excerpts:
                content_hash = hash(exc.content)
                if content_hash not in seen_content:
                    seen_content.add(content_hash)
                    all_excerpts.append(exc)

        # Sort by relevance and take top_k
        all_excerpts.sort(key=lambda x: x.relevance_score, reverse=True)
        return all_excerpts[:top_k]

    def retrieve_library_purpose(self) -> str:
        """Retrieve and generate a library purpose summary.

        Uses LLM summarization if llm_adapter is available (PromeFuzz-style),
        otherwise falls back to simple truncation.

        Returns:
            One-sentence description of the library's purpose
        """
        if self.knowledge.library_purpose:
            return self.knowledge.library_purpose

        # Search for introduction/overview content
        excerpts = self.retrieve(
            f"introduction {self.project_name} library overview purpose",
            top_k=3
        )

        if not excerpts:
            return self.knowledge.library_purpose

        # Format excerpts for prompt
        excerpts_text = self.format_excerpts_for_prompt(excerpts, max_length=3000)

        # Use LLM for summarization if available (PromeFuzz-style)
        if self.llm_adapter and excerpts_text:
            try:
                prompt = f"""Below are excerpts from documents. Please provide a summary of the library's purpose, starting your response with "{self.project_name} is …".

{excerpts_text}"""
                summary = self.llm_adapter.query(prompt)
                if summary and len(summary) > 10:
                    self.knowledge.library_purpose = summary.strip()
                    self.logger.debug(f"LLM summarized library purpose: {summary[:100]}...")
                    return self.knowledge.library_purpose
            except Exception as e:
                self.logger.warning(f"LLM summarization failed, using fallback: {e}")

        # Fallback: simple truncation
        combined = "\n\n".join([e.content for e in excerpts])
        self.knowledge.library_purpose = combined[:500]

        return self.knowledge.library_purpose

    def filter_valuable_excerpts(
        self,
        function_name: str,
        excerpts: List[DocumentExcerpt]
    ) -> List[DocumentExcerpt]:
        """Filter excerpts to keep only valuable ones for a function.

        Uses LLM to determine which excerpts are valuable (PromeFuzz-style).

        Args:
            function_name: Name of the target function
            excerpts: List of candidate excerpts

        Returns:
            Filtered list of valuable excerpts
        """
        if not excerpts:
            return []

        # Without LLM, return all excerpts
        if not self.llm_adapter:
            return excerpts

        # Format excerpts for LLM evaluation
        excerpts_text = self.format_excerpts_for_prompt(excerpts, max_length=4000)

        try:
            system_context = f"""You are an expert in the {self.project_name} library. You will receive excerpts from the library's documentation. Your task is to identify which excerpts describe the API function {function_name}. Consider the following questions:

- Does the excerpt explain the function's purpose?
- Does it detail how the function is utilized?
- Does it include any code examples that call the function?

If any of these criteria are met, the excerpt is deemed valuable."""

            user_prompt = f"""Below are some excerpts from the library documents. Please respond with the numbers of the valuable excerpts, each on a new line, without additional information. If there are no valuable excerpts, reply with '0'.

{excerpts_text}"""

            response = self.llm_adapter.query(f"{system_context}\n\n{user_prompt}")

            # Parse response to get valuable excerpt indices
            valuable_indices = set()
            for line in response.strip().split('\n'):
                line = line.strip()
                # Extract numbers from the line
                for word in line.split():
                    try:
                        idx = int(word.rstrip('.,;'))
                        if 1 <= idx <= len(excerpts):
                            valuable_indices.add(idx - 1)  # Convert to 0-indexed
                    except ValueError:
                        continue

            if valuable_indices:
                filtered = [excerpts[i] for i in sorted(valuable_indices)]
                self.logger.debug(
                    f"Filtered {len(excerpts)} -> {len(filtered)} valuable excerpts for {function_name}"
                )
                return filtered

        except Exception as e:
            self.logger.warning(f"LLM excerpt filtering failed for {function_name}: {e}")

        # Fallback: return all excerpts
        return excerpts

    def extract_api_semantics(
        self,
        function_name: str,
        function_signature: str = "",
        excerpts: Optional[List[DocumentExcerpt]] = None
    ) -> Optional[APISemantics]:
        """Extract structured API semantics from documentation.

        Uses LLM to parse documentation excerpts and extract:
        - Parameter constraints (NULL checks, size limits, ownership)
        - Return value semantics (error values, ownership)
        - Preconditions and postconditions
        - Related init/cleanup APIs

        Args:
            function_name: Name of the function
            function_signature: Optional function signature for context
            excerpts: Optional pre-retrieved excerpts (will retrieve if not provided)

        Returns:
            APISemantics object or None if extraction fails
        """
        # Check cache
        if function_name in self.knowledge.api_semantics:
            return self.knowledge.api_semantics[function_name]

        # Retrieve documentation if not provided
        if excerpts is None:
            excerpts = self.retrieve_for_function(function_name, top_k=4)

        if not excerpts:
            return None

        # Without LLM, create basic semantics from raw excerpts
        if not self.llm_adapter:
            semantics = APISemantics(
                function_name=function_name,
                source_excerpts=[e.content[:500] for e in excerpts[:2]]
            )
            self.knowledge.api_semantics[function_name] = semantics
            return semantics

        # Use LLM to extract structured semantics
        excerpts_text = self.format_excerpts_for_prompt(excerpts, max_length=3000)

        prompt = f"""Analyze the documentation for the function `{function_name}` and extract structured semantic information.

{f"Function signature: {function_signature}" if function_signature else ""}

Documentation excerpts:
{excerpts_text}

Extract the following information in JSON format:
{{
  "brief_description": "One sentence describing what the function does",
  "parameters": {{
    "param_name": {{
      "constraints": ["list of constraints like 'must not be NULL', 'size > 0'"],
      "ownership": "caller_frees | callee_frees | borrowed | (empty if unknown)"
    }}
  }},
  "return": {{
    "description": "What the return value represents",
    "ownership": "caller_frees | static | internal | (empty if unknown)",
    "error_value": "What value indicates error (e.g., 'NULL', '-1', 'false')"
  }},
  "preconditions": ["Conditions that must be true before calling"],
  "postconditions": ["Conditions guaranteed after calling"],
  "init_api": "API that must be called before this one (or empty)",
  "cleanup_api": "API that must be called after this one (or empty)"
}}

If information is not available in the documentation, use empty strings or empty lists.
Respond with ONLY the JSON, no other text."""

        try:
            response = self.llm_adapter.query(prompt)

            # Parse JSON response
            # Try to extract JSON from response (handle markdown code blocks)
            json_str = response.strip()
            if json_str.startswith("```"):
                # Remove markdown code block
                lines = json_str.split("\n")
                json_str = "\n".join(lines[1:-1] if lines[-1].strip() == "```" else lines[1:])

            data = json.loads(json_str)

            # Build APISemantics from parsed data
            param_constraints = {}
            for param_name, param_data in data.get("parameters", {}).items():
                param_constraints[param_name] = ParameterConstraint(
                    param_name=param_name,
                    constraints=param_data.get("constraints", []),
                    ownership=param_data.get("ownership", "")
                )

            semantics = APISemantics(
                function_name=function_name,
                brief_description=data.get("brief_description", ""),
                parameter_constraints=param_constraints,
                return_description=data.get("return", {}).get("description", ""),
                return_ownership=data.get("return", {}).get("ownership", ""),
                error_return_value=data.get("return", {}).get("error_value", ""),
                preconditions=data.get("preconditions", []),
                postconditions=data.get("postconditions", []),
                init_api=data.get("init_api", ""),
                cleanup_api=data.get("cleanup_api", ""),
                source_excerpts=[e.content[:300] for e in excerpts[:2]]
            )

            self.knowledge.api_semantics[function_name] = semantics
            self.logger.debug(f"Extracted API semantics for {function_name}")
            return semantics

        except json.JSONDecodeError as e:
            self.logger.warning(f"Failed to parse API semantics JSON for {function_name}: {e}")
        except Exception as e:
            self.logger.warning(f"Failed to extract API semantics for {function_name}: {e}")

        # Fallback: basic semantics
        semantics = APISemantics(
            function_name=function_name,
            source_excerpts=[e.content[:500] for e in excerpts[:2]]
        )
        self.knowledge.api_semantics[function_name] = semantics
        return semantics

    def extract_api_semantics_batch(
        self,
        function_names: List[str],
        function_signatures: Optional[Dict[str, str]] = None
    ) -> Dict[str, APISemantics]:
        """Extract API semantics for multiple functions.

        Args:
            function_names: List of function names to analyze
            function_signatures: Optional dict mapping function names to signatures

        Returns:
            Dict mapping function names to APISemantics
        """
        function_signatures = function_signatures or {}
        result = {}

        for func_name in function_names:
            sig = function_signatures.get(func_name, "")
            semantics = self.extract_api_semantics(func_name, sig)
            if semantics:
                result[func_name] = semantics

        return result

    def get_function_documentation(
        self,
        function_names: List[str],
        top_k_per_function: int = 2,
        filter_valuable: bool = True
    ) -> Dict[str, List[DocumentExcerpt]]:
        """Get documentation for multiple functions.

        Args:
            function_names: List of function names
            top_k_per_function: Excerpts per function
            filter_valuable: If True and LLM available, filter to valuable excerpts

        Returns:
            Dict mapping function names to their documentation excerpts
        """
        result = {}
        for func_name in function_names:
            # Retrieve more candidates if we're going to filter
            retrieve_k = top_k_per_function * 2 if (filter_valuable and self.llm_adapter) else top_k_per_function
            excerpts = self.retrieve_for_function(func_name, retrieve_k)

            if excerpts:
                # Filter to valuable excerpts if enabled
                if filter_valuable and self.llm_adapter:
                    excerpts = self.filter_valuable_excerpts(func_name, excerpts)

                # Take top_k after filtering
                result[func_name] = excerpts[:top_k_per_function]

        return result

    def format_excerpts_for_prompt(
        self,
        excerpts: List[DocumentExcerpt],
        max_length: int = 2000
    ) -> str:
        """Format excerpts for inclusion in LLM prompt.

        Args:
            excerpts: List of excerpts to format
            max_length: Maximum total length

        Returns:
            Formatted string for prompt inclusion
        """
        if not excerpts:
            return ""

        formatted_parts = []
        current_length = 0

        for i, excerpt in enumerate(excerpts, 1):
            formatted = f"{i}. Excerpt from {excerpt.source}:\n```\n{excerpt.content}\n```"

            if current_length + len(formatted) > max_length:
                break

            formatted_parts.append(formatted)
            current_length += len(formatted)

        return "\n\n".join(formatted_parts)


def create_knowledge_manager(
    project_name: str,
    document_paths: Optional[List[str]] = None,
    persist_dir: Optional[str] = None,
    logger_instance: Optional[logging.Logger] = None,
    llm_adapter: Optional[Any] = None
) -> DocumentKnowledgeManager:
    """Factory function to create and initialize a DocumentKnowledgeManager.

    Args:
        project_name: Name of the project
        document_paths: List of document paths to index
        persist_dir: Directory to persist the vector database
        logger_instance: Optional logger
        llm_adapter: Optional LLM adapter for summarization/filtering (PromeFuzz-style)

    Returns:
        Initialized DocumentKnowledgeManager
    """
    manager = DocumentKnowledgeManager(
        project_name=project_name,
        persist_dir=persist_dir,
        logger_instance=logger_instance,
        llm_adapter=llm_adapter
    )

    if document_paths:
        manager.index_documents(document_paths)

    return manager
