# Knowledge Extraction Setup Guide

This document explains how to configure documentation paths and how LogicFuzz uses knowledge from documentation and existing fuzz drivers.

## Overview

LogicFuzz extracts knowledge from two sources to improve fuzz driver generation:

1. **Documentation** - API docs, header files, README files
2. **Existing Fuzz Drivers** - Patterns from existing OSS-Fuzz drivers

## 1. Documentation Setup

### Configuring document_paths in YAML

Add `document_paths` to your benchmark YAML file to specify documentation sources:

```yaml
"project": "libucl"
"language": "c"
"target_path": "/src/libucl/ucl_fuzzer.c"
"target_name": "ucl_fuzzer"
"document_paths":
  - "/src/libucl/doc/api.md"       # API documentation
  - "/src/libucl/include/ucl.h"    # Header file with comments
  - "/src/libucl/README.md"        # Project README
```

### Path Format

Paths are relative to the project's Docker container root, typically:
- `/src/<project>/` - Main source directory
- `/src/<project>/include/` - Header files
- `/src/<project>/doc/` or `/src/<project>/docs/` - Documentation

### Supported File Types

| Type | Extension | Description |
|------|-----------|-------------|
| Markdown | `.md` | API docs, README files |
| Header | `.h`, `.hpp` | C/C++ headers with Doxygen comments |
| reStructuredText | `.rst` | Sphinx documentation |
| Plain text | `.txt` | Plain text documentation |

### What Gets Extracted

The DocumentKnowledgeManager extracts:

1. **Function-level documentation** - Parameters, return values, descriptions
2. **Structured API semantics** (via LLM):
   - Parameter constraints (e.g., "must not be NULL", "size > 0")
   - Ownership semantics (caller_frees, callee_frees, borrowed)
   - Error return values
   - Preconditions and postconditions
   - Cleanup API relationships

### Example: Structured Extraction

From documentation:
```c
/**
 * ucl_parser_add_chunk - Add data chunk to parser
 * @parser: Parser object (must not be NULL)
 * @data: Input data buffer
 * @len: Length of data (must be > 0)
 * @return: true on success, false on error
 */
```

LogicFuzz extracts:
```
Function: ucl_parser_add_chunk
  - parser: must not be NULL, ownership=borrowed
  - data: input buffer, ownership=borrowed
  - len: must be > 0
  - Returns: true/false
  - Cleanup: ucl_parser_free (inferred)
```

## 2. Existing Fuzz Driver Knowledge

### Automatic Discovery

LogicFuzz automatically discovers existing fuzz drivers from:

1. **OSS-Fuzz project directory**: `/src/<project>/` in Docker
2. **Common patterns**: `*_fuzzer.c`, `*_fuzzer.cpp`, `fuzz_*.c`

### What Gets Extracted from Drivers

The driver pattern analyzer extracts:

| Pattern | Description | Used By |
|---------|-------------|---------|
| `core_functionality` | What the driver tests | Prototyper |
| `setup_teardown` | Init/cleanup patterns | Prototyper |
| `code_patterns` | Reusable code idioms | Prototyper |
| `header_config` | Header include order | Fixer (header errors) |
| `boundary_checks` | Size/NULL checks | Fixer (crash fixes) |
| `error_handling` | Error handling patterns | Fixer |

### Header Configuration Extraction

From existing driver:
```c
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "ucl.h"
```

Extracted as:
```
<header_config>
stdint.h (system)
stdlib.h (system)
string.h (system)
ucl.h (project)
</header_config>
```

### Boundary Check Extraction

From existing driver:
```c
if (size < 4) return 0;
if (data == NULL) return 0;
```

Extracted as:
```
<boundary_checks>
- Early return if size < 4
- NULL check on data pointer
</boundary_checks>
```

## 3. Knowledge Usage in Workflow

### Where Knowledge Is Used

```
┌─────────────────┐
│   Prototyper    │ ← Documentation (API semantics)
│                 │ ← Driver patterns (code structure)
└────────┬────────┘
         │
┌────────▼────────┐
│     Build       │
└────────┬────────┘
         │
┌────────▼────────┐
│     Fixer       │ ← Driver patterns (headers, boundary checks)
│                 │   for targeted error fixes
└────────┬────────┘
         │
┌────────▼────────┐
│    Improver     │ ← Documentation (for new API discovery)
└─────────────────┘
```

### Prototyper Usage

The Prototyper receives:
- API semantics (parameter constraints, ownership)
- Driver patterns (structure, setup/teardown)

Example prompt injection:
```
## API Documentation
Function: ucl_parser_new
  - flags: initialization flags (0 for default)
  - Returns: parser object (caller must free with ucl_parser_free)

## Reference Driver Patterns
- Always check parser != NULL after creation
- Use try-finally pattern for cleanup
```

### Fixer Usage

The Fixer receives patterns based on error type:

**For header errors:**
```
Reference header configuration from existing driver:
- stdint.h (system)
- ucl.h (project)
```

**For crash/runtime errors:**
```
Boundary check patterns from existing driver:
- if (size < 4) return 0;
- NULL pointer checks on all inputs
```

## 4. Configuration Tips

### Best Practices

1. **Include header files** - They often have the most accurate API documentation
2. **Include README** - Contains usage examples and common patterns
3. **Order matters** - List more specific docs before general ones
4. **Check paths** - Paths must exist inside the Docker container

### Debugging

Check if documents are being loaded:
```
INFO doc_knowledge - index_documents: Discovered 3 documents to index
INFO doc_knowledge - index_documents: Indexed 45 new document chunks
```

If you see:
```
WARNING doc_knowledge - discover_documents: Document path does not exist: /src/...
```

The path doesn't exist in the container. Verify with:
```bash
docker run -it gcr.io/oss-fuzz/<project> ls -la /src/<project>/
```

### Finding Documentation Paths

Common locations to check:
```
/src/<project>/
├── README.md
├── doc/
│   ├── api.md
│   └── manual.md
├── docs/
│   └── index.rst
├── include/
│   └── *.h
└── src/
    └── *.h
```

## 5. RAG Retrieval System

### How Retrieval Works

1. Documents are chunked (512 tokens with 50 token overlap)
2. Chunks are embedded using sentence-transformers
3. Stored in ChromaDB for semantic search
4. At query time:
   - API names are used as queries
   - Top-K relevant chunks are retrieved
   - Structured semantics are extracted via LLM

### Caching

- Document embeddings are cached per project
- Re-indexing happens when:
  - Document content changes
  - Document paths change
  - Cache is cleared

### Token Efficiency

The system is designed for token efficiency:
- Only relevant chunks are retrieved (not entire documents)
- Structured extraction is batched
- Results are cached across trials
