"""
LangChain-based LLM models for LogicFuzz.

This module provides a unified interface for LLM providers that support tool calling.
Supported providers: OpenAI, DeepSeek, Anthropic Claude, VIO (enterprise gateway), Ollama (local).
"""

import logging
import os
from typing import Any, Callable, Dict, List

from langchain_core.language_models import BaseChatModel
from langchain_core.runnables import Runnable

logger = logging.getLogger(__name__)

# Default parameters
MAX_TOKENS: int = 4096
TEMPERATURE: float = 0.4
DEFAULT_MODEL = "gpt-5.2"

# ---------------------------------------------------------------------------
# Flag: set to True to prefer local Ollama models over cloud providers
# ---------------------------------------------------------------------------
USE_LOCAL_MODELS: bool = os.getenv("USE_LOCAL_MODELS", "").lower() in ("1", "true", "yes")


def _create_openai_model(model_name: str,
                         temperature: float = TEMPERATURE,
                         max_tokens: int = MAX_TOKENS,
                         **kwargs) -> BaseChatModel:
    """Create an OpenAI chat model."""
    from langchain_openai import ChatOpenAI
    return ChatOpenAI(model=model_name,
                      temperature=temperature,
                      max_tokens=max_tokens,
                      **kwargs)


def _create_deepseek_model(model_name: str = "deepseek-chat",
                           temperature: float = TEMPERATURE,
                           max_tokens: int = MAX_TOKENS,
                           **kwargs) -> BaseChatModel:
    """Create a DeepSeek chat model (OpenAI-compatible API)."""
    import httpx
    from langchain_openai import ChatOpenAI

    # Prefer system CA store so corporate/intermediate CAs are honored.
    ca_bundle = _get_system_ca_bundle_path()
    verify = ca_bundle if ca_bundle else True
    http_client = httpx.Client(verify=verify, trust_env=True)
    return ChatOpenAI(model=model_name,
                      base_url="https://api.deepseek.com",
                      api_key=os.getenv("DEEPSEEK_API_KEY"),
                      temperature=temperature,
                      max_tokens=max_tokens,
                      http_client=http_client,
                      **kwargs)


def _create_anthropic_model(model_name: str,
                            temperature: float = TEMPERATURE,
                            max_tokens: int = MAX_TOKENS,
                            **kwargs) -> BaseChatModel:
    """Create an Anthropic Claude chat model."""
    from langchain_anthropic import ChatAnthropic
    return ChatAnthropic(model=model_name,
                         temperature=temperature,
                         max_tokens=max_tokens,
                         **kwargs)


# ---------------------------------------------------------------------------
# VIO enterprise gateway helpers
# ---------------------------------------------------------------------------

def _is_vio_model_name(name: str) -> bool:
    return name == "vio" or name.startswith("vio/")


def _extract_vio_model_name(name: str) -> str:
    if name == "vio":
        return os.getenv("VIO_MODEL", "Default")
    model_name = name.split("/", 1)[1]
    if not model_name:
        raise ValueError("vio/ prefix requires a model name, e.g. vio/gpt-4o")
    return model_name


def _normalize_openai_base_url(base_url: str) -> str:
    normalized = base_url.rstrip("/")
    if normalized.endswith("/chat/completions"):
        normalized = normalized[: -len("/chat/completions")]
    return normalized


def _get_system_ca_bundle_path() -> str:
    """Resolve CA bundle path, preferring explicit LogicFuzz/system paths."""
    explicit = (os.getenv("LOGICFUZZ_CA_BUNDLE") or os.getenv("VIO_CA_BUNDLE")
                or os.getenv("DEEPSEEK_CA_BUNDLE") or os.getenv("OPENAI_CA_BUNDLE")
                or "")
    if explicit:
        return explicit
    for candidate in ("/usr/lib/ssl/cert.pem",
                      "/etc/ssl/certs/ca-certificates.crt"):
        if os.path.exists(candidate):
            return candidate
    return ""


def _create_vio_model(model_name: str,
                      temperature: float = TEMPERATURE,
                      max_tokens: int = MAX_TOKENS,
                      **kwargs) -> BaseChatModel:
    """Create a model via the VIO enterprise gateway (OpenAI-compatible)."""
    import httpx
    from langchain_openai import ChatOpenAI

    base_url = os.getenv("VIO_BASE_URL", "")
    api_key = os.getenv("VIO_API_KEY") or os.getenv("API_KEY")
    ca_bundle = _get_system_ca_bundle_path()
    verify = ca_bundle if ca_bundle else True
    http_client = httpx.Client(verify=verify, trust_env=True)
    return ChatOpenAI(model=model_name,
                      base_url=_normalize_openai_base_url(base_url),
                      api_key=api_key,
                      temperature=temperature,
                      max_tokens=max_tokens,
                      http_client=http_client,
                      **kwargs)


# ---------------------------------------------------------------------------
# Ollama (local) helper
# ---------------------------------------------------------------------------

def _create_ollama_model(model_name: str,
                         temperature: float = TEMPERATURE,
                         max_tokens: int = MAX_TOKENS,
                         **kwargs) -> BaseChatModel:
    """Create a local Ollama chat model."""
    from langchain_ollama import ChatOllama
    base_url = os.getenv("OLLAMA_BASE_URL", "http://localhost:11434")
    return ChatOllama(model=model_name,
                      base_url=base_url,
                      temperature=temperature,
                      num_predict=max_tokens,
                      **kwargs)


# Model registry: maps model names to factory functions
# Only models with tool calling support are included
MODEL_REGISTRY: Dict[str, Callable[..., BaseChatModel]] = {
    # OpenAI models (all support tool calling)
    "gpt-5.2":
    lambda **kw: _create_openai_model("gpt-5.2", **kw),
    "gpt-5-mini":
    lambda **kw: _create_openai_model("gpt-5-mini", **kw),
    "gpt-4o":
    lambda **kw: _create_openai_model("gpt-4o", **kw),
    "gpt-4o-mini":
    lambda **kw: _create_openai_model("gpt-4o-mini", **kw),
    "gpt-4-turbo":
    lambda **kw: _create_openai_model("gpt-4-turbo", **kw),
    "gpt-4":
    lambda **kw: _create_openai_model("gpt-4", **kw),

    # DeepSeek models (OpenAI-compatible, support tool calling)
    "deepseek-chat":
    lambda **kw: _create_deepseek_model("deepseek-chat", **kw),

    # Anthropic Claude models (all support tool calling)
    "claude-3-5-sonnet":
    lambda **kw: _create_anthropic_model("claude-3-5-sonnet-latest", **kw),
    "claude-3-opus":
    lambda **kw: _create_anthropic_model("claude-3-opus-latest", **kw),
    "claude-3-haiku":
    lambda **kw: _create_anthropic_model("claude-3-haiku-20240307", **kw),
}


def get_chat_model(name: str,
                   temperature: float = TEMPERATURE,
                   max_tokens: int = MAX_TOKENS,
                   **kwargs) -> BaseChatModel:
    """
    Get a LangChain chat model by name.

    Args:
        name: Model name (e.g., "gpt-5.2", "gpt-4o", "deepseek-chat", "claude-3-5-sonnet", "ollama/qwen", "vio/gpt-4")
        temperature: Sampling temperature (default: 0.4)
        max_tokens: Maximum tokens for response (default: 4096)
        **kwargs: Additional arguments passed to the model constructor

    Returns:
        LangChain BaseChatModel instance

    Raises:
        ValueError: If model name is not recognized
    """
    if name not in MODEL_REGISTRY:
        # Check for VIO enterprise gateway models (vio or vio/<model>)
        if _is_vio_model_name(name):
            vio_model = _extract_vio_model_name(name)
            return _create_vio_model(vio_model,
                                     temperature=temperature,
                                     max_tokens=max_tokens,
                                     **kwargs)
        # Check for Ollama local models (ollama or ollama/<model>)
        if name == "ollama" or name.startswith("ollama/"):
            ollama_model = name.split("/", 1)[1] if "/" in name else os.getenv("OLLAMA_MODEL", "llama2")
            return _create_ollama_model(ollama_model,
                                        temperature=temperature,
                                        max_tokens=max_tokens,
                                        **kwargs)
        raise ValueError(
            f"Unknown model: {name}. Available: {list(MODEL_REGISTRY.keys())}")
    return MODEL_REGISTRY[name](temperature=temperature,
                                max_tokens=max_tokens,
                                **kwargs)


def get_available_models() -> List[str]:
    """Return list of available model names."""
    models = list(MODEL_REGISTRY.keys())
    models.extend(["vio/<model-id>", "vio", "ollama/<model-id>", "ollama"])
    return models


def get_model_with_tools(name: str,
                         tools: List[Any],
                         temperature: float = TEMPERATURE,
                         max_tokens: int = MAX_TOKENS,
                         **kwargs) -> Runnable:
    """Get a LangChain chat model with tools bound."""
    model = get_chat_model(name, temperature, max_tokens, **kwargs)
    return model.bind_tools(tools)
