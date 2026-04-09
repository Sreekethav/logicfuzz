"""
Context modules for LogicFuzz workflow.

Provides:
- FuzzingContext: Immutable data context for fuzzing workflow
- DocumentKnowledgeManager: RAG-based documentation retrieval
- Session memory utilities: Functions for injecting session memory into prompts
"""

from src.context.data_context import FuzzingContext
from src.context.doc_knowledge import (
    DocumentKnowledgeManager,
    DocumentExcerpt,
    DocumentKnowledge,
    ParameterConstraint,
    APISemantics,
    create_knowledge_manager,
)
from src.context.session_memory_injector import (
    build_prompt_with_session_memory,
    extract_session_memory_updates_from_response,
    merge_session_memory_updates,
)

__all__ = [
    "FuzzingContext",
    "DocumentKnowledgeManager",
    "DocumentExcerpt",
    "DocumentKnowledge",
    "ParameterConstraint",
    "APISemantics",
    "create_knowledge_manager",
    "build_prompt_with_session_memory",
    "extract_session_memory_updates_from_response",
    "merge_session_memory_updates",
]
