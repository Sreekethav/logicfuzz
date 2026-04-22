# LogicFuzz v3.3 – New Project Integration Guide

This document provides detailed instructions on how to run LogicFuzz with **any C/C++ project** whose source code, OSS-Fuzz environment files, and (optionally) documentation are available locally. LogicFuzz v3.3 removes the dependency on FuzzIntrospector and uses local Clang/LLVM-based API extraction instead.

## What's New in v3.3

| Feature | Description |
|---------|-------------|
| **Project-Level Mode** | Simplified YAML – no need to list individual functions. APIs are extracted automatically via Clang/LLVM. |
| **RAG Documentation** | Add `document_paths` to your YAML to feed project docs (`.h`, `.md`, `.txt`, `.html`, `.pdf`, `.rst`, `.adoc`, `.zip`) into a ChromaDB vector store for context-aware driver generation. |
| **Local LLM Support** | Use any Ollama model with `--model ollama/<model-id>` (e.g. `ollama/qwen3:32b`). |
| **No FuzzIntrospector** | Replaced by `HybridAPIExtractor` (Clang/LLVM). Works with internal/proprietary projects. |

## Table of Contents

1. [Prerequisites](#1-prerequisites)
2. [Preparing Your Project Files](#2-preparing-your-project-files)
3. [Creating Benchmark Configuration](#3-creating-benchmark-configuration)
4. [Running LogicFuzz](#4-running-logicfuzz)
5. [Understanding Results](#5-understanding-results)
6. [Full Walkthrough: PSM Project](#6-full-walkthrough-psm-project)
7. [CLI Reference](#7-cli-reference)
8. [Troubleshooting](#8-troubleshooting)
9. [Cleanup and Reset](#9-cleanup-and-reset)

---

## 1. Prerequisites

### 1.1 Environment Requirements

- **Docker** installed and running (used by OSS-Fuzz to build/run fuzz targets)
- **Python 3.10+**
- **Git**
- **LLM access** – one of:
  - Local Ollama server with a pulled model (e.g. `ollama pull qwen3:32b`)
  - API key for OpenAI, DeepSeek, Anthropic, or Qwen (DashScope)

### 1.2 Install LogicFuzz Dependencies

```bash
cd /path/to/logicfuzz
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
```

### 1.3 Clone OSS-Fuzz (First-time Setup)

LogicFuzz uses the OSS-Fuzz build infrastructure (`infra/helper.py`) to compile and run fuzz targets inside Docker containers.

```bash
cd /path/to/logicfuzz
git clone --depth 1 https://github.com/google/oss-fuzz.git oss-fuzz

# Verify
ls oss-fuzz/infra/helper.py
```

> **Note:** FuzzIntrospector is **no longer required** in v3.3.

---

## 2. Preparing Your Project Files

LogicFuzz builds and runs fuzz targets through OSS-Fuzz Docker containers.
You need three files for your project:

```
data-dir/projects/<project-name>/
├── Dockerfile       # Copies source into the container
├── build.sh         # Compiles the project and links the fuzzer
├── project.yaml     # Metadata (language, homepage)
├── fuzzer.c         # Placeholder (overwritten by LogicFuzz-generated target)
├── src/             # (optional) Source included via COPY in Dockerfile
├── include/         # (optional) Headers
└── doc/             # (optional) Documentation files/archives
```

Place project files in `data-dir/projects/<project-name>/` (not directly
in `oss-fuzz/projects/`). LogicFuzz syncs them via the `OSS_FUZZ_DATA_DIR`
environment variable on each run.

```bash
PROJECT_NAME=my-project
SOURCE_DIR=/path/to/my_project_source

mkdir -p data-dir/projects/$PROJECT_NAME
cp -r "$SOURCE_DIR"/* data-dir/projects/$PROJECT_NAME/
```

### 2.1 Dockerfile

The Dockerfile must use `gcr.io/oss-fuzz-base/base-builder` as the base image
and copy your source code into `$SRC/<project-name>/`.

```dockerfile
FROM gcr.io/oss-fuzz-base/base-builder

RUN apt-get update && apt-get install -y make gcc

COPY src/    $SRC/my-project/src/
COPY include/ $SRC/my-project/include/

COPY build.sh $SRC/
```

### 2.2 build.sh

The build script compiles your library and links it with `$LIB_FUZZING_ENGINE`:

```bash
#!/bin/bash -eu
cd $SRC/my-project

$CC $CFLAGS -Iinclude -c src/my_lib.c -o my_lib.o
$CXX $CXXFLAGS $LIB_FUZZING_ENGINE my_lib.o -o $OUT/my_fuzzer
```

### 2.3 project.yaml

```yaml
homepage: "https://example.com/my-project"
language: c      # or c++
primary_contact: "dev@example.com"
```

### 2.4 Documentation Files (Optional)

Place documentation files alongside your source. Supported formats:
`.h` (Doxygen comments), `.md`, `.txt`, `.html`, `.pdf`, `.rst`, `.adoc`.

You can also provide a `.zip` archive — LogicFuzz will extract it automatically.

```
oss-fuzz/projects/my-project/doc/my_docs.zip
```

---

## 3. Creating Benchmark Configuration

Create a YAML file under `conti-benchmark/` (or any location).

### 3.1 Project-Level Mode (Recommended)

In project-level mode, you **do not list individual functions**.
LogicFuzz automatically extracts APIs from your project via Clang/LLVM.

```yaml
"project": "my-project"
"language": "c"
"target_path": "/src/my-project/fuzz_target.c"
"target_name": "my_fuzzer"

# Optional: documentation paths for RAG-based knowledge retrieval
# Paths are relative to the LogicFuzz working directory
"document_paths":
  - "doc/my_docs.zip"
```

| Field | Required | Description |
|-------|----------|-------------|
| `project` | Yes | Must match the directory name under `oss-fuzz/projects/` |
| `language` | Yes | `c` or `c++` |
| `target_path` | Yes | Where the generated fuzz target will be placed inside the container |
| `target_name` | Yes | Output binary name (passed to `$OUT/<name>`) |
| `document_paths` | No | List of paths to docs (relative to CWD or absolute). Supports dirs, files, and `.zip` |

### 3.2 Function-Level Mode (Advanced)

If you want to target specific functions, add a `functions` section:

```yaml
"project": "my-project"
"language": "c"
"target_path": "/src/my-project/fuzz_target.c"
"target_name": "my_fuzzer"
"functions":
  - "name": "parse_input"
    "signature": "int parse_input(const char*, size_t)"
    "return_type": "int"
    "params":
      - "name": "data"
        "type": "const char*"
      - "name": "size"
        "type": "size_t"
```

---

## 4. Running LogicFuzz

### 4.1 Set Up LLM Access

**Option A: Local Ollama (no API key needed)**

```bash
# Ensure Ollama is running and a model is pulled
ollama list                      # Check available models
ollama pull qwen3:32b            # Pull a model if needed

# Optional: override the Ollama endpoint (default: http://localhost:11434/v1)
export OLLAMA_BASE_URL=http://localhost:11434/v1
```

**Option B: Cloud API**

```bash
# Pick one:
export OPENAI_API_KEY="sk-..."       # For GPT models
export DEEPSEEK_API_KEY="sk-..."     # For DeepSeek
export DASHSCOPE_API_KEY="sk-..."    # For Qwen
```

### 4.2 Set Up Your Project's Data Directory

LogicFuzz cleans the OSS-Fuzz tree (`git clean`) on each run, so project files
must be placed in a **separate data directory** that gets synced automatically.

```bash
cd /path/to/logicfuzz

PROJECT_NAME=my-project
SOURCE_DIR=/path/to/my_project_source

# Create the data-dir structure
mkdir -p data-dir/projects/$PROJECT_NAME
cp -r "$SOURCE_DIR"/* data-dir/projects/$PROJECT_NAME/
```

LogicFuzz copies `data-dir/projects/<name>/` into `oss-fuzz/projects/<name>/`
before each run when `OSS_FUZZ_DATA_DIR` is set.

> **Important:** Ensure a placeholder `fuzzer.c` (or `.cpp`) file exists if
> your `Dockerfile` has a `COPY fuzzer.c ...` line.
> LogicFuzz will **overwrite** this file with the generated fuzz target.

### 4.3 Run

```bash
OSS_FUZZ_DATA_DIR=$(pwd)/data-dir \
LLM_NUM_EXP=1 \
python run_logicfuzz.py \
  -y conti-benchmark/my-project.yaml \
  --model ollama/qwen3:32b \
  --oss-fuzz-dir oss-fuzz \
  --num-samples 1 \
  --run-timeout 60
```

Key flags:
- `-y` — Path to benchmark YAML file
- `--model` — LLM model name (`ollama/<id>` for local, or `gpt-5.2`, `deepseek-chat`, etc.)
- `--oss-fuzz-dir` — Path to the OSS-Fuzz checkout (defaults to `./oss-fuzz`)
- `--num-samples` — Number of driver generation trials (default: 5)
- `--run-timeout` — Seconds to run each generated fuzzer (default: 60)
- `--max-round` — Max agent retry rounds (default: 10)

### 4.4 Environment Variables

| Variable | Default | Description |
|----------|---------|-------------|
| `OSS_FUZZ_DATA_DIR` | *(unset)* | Path to your data directory containing `projects/`. **Required for custom projects.** |
| `OLLAMA_BASE_URL` | `http://localhost:11434/v1` | Ollama API endpoint |
| `OLLAMA_API_KEY` | `ollama` | Ollama API key (usually not needed) |
| `LLM_NUM_EXP` | `10` | Number of parallel experiments |
| `LLM_NUM_EVA` | `6` | Number of parallel target evaluations per experiment |
| `OFG_USE_CACHING` | `1` | Enable Docker image caching |

> **Tip:** For local experiments, set `LLM_NUM_EXP=1` and `LLM_NUM_EVA=2` to avoid overloading your system.

---

## 5. Understanding Results

Results are saved under `./results/output-<benchmark-id>/`:

```
results/output-my-project-project/
├── benchmark.yaml              # Copy of the benchmark used
├── fuzz_targets/               # Generated fuzz target source files
│   ├── 01.cpp
│   ├── 02.cpp
│   └── ...
├── build_log.txt               # Compilation output
├── code-coverage-reports/      # Coverage reports per sample
│   └── 01/textcov/
├── status/                     # Per-trial results
│   └── 01/result.json          # Build success, coverage, crashes, token usage
└── ...
```

A `report.json` file is written to the work directory with aggregate statistics:
- `token_usage_summary` — Total and per-agent token counts
- `project_summary` — Coverage gains per project
- `start_time` / `completion_time` / `total_run_time`

---

## 6. Full Walkthrough: PSM Project

This section walks through running LogicFuzz on the PSM (Power Sequence Manager) project using a local Ollama model. The PSM source, OSS-Fuzz files, and documentation are in `psm_oss_source/`.

### Step 1: Set up environment

```bash
cd /path/to/logicfuzz

# Create virtual environment
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
```

### Step 2: Clone OSS-Fuzz (if not already done)

```bash
git clone --depth 1 https://github.com/google/oss-fuzz.git oss-fuzz
```

### Step 3: Set up the data directory

LogicFuzz cleans the OSS-Fuzz tree on each run, so keep your project files
in a separate data directory:

```bash
mkdir -p data-dir/projects/psm
cp -r psm_oss_source/* data-dir/projects/psm/
```

### Step 4: Verify the benchmark YAML

The file `conti-benchmark/psm.yaml` should look like:

```yaml
"project": "psm"
"language": "c++"
"target_path": "/src/psm-project/fuzzer.c"
"target_name": "psm_project_fuzzer"
"document_paths":
  - "data-dir/projects/psm/doc/psm_doc.zip"
```

Key points:
- `project` must match the directory name `data-dir/projects/psm/`
- `target_path` must match the path used by `build.sh` inside the Docker container
- `document_paths` are relative to the LogicFuzz working directory

### Step 5: Run LogicFuzz with an Ollama model

```bash
# Verify Ollama has a model available
ollama list

# Run LogicFuzz
OSS_FUZZ_DATA_DIR=$(pwd)/data-dir \
LLM_NUM_EXP=1 \
python run_logicfuzz.py \
  -y conti-benchmark/psm.yaml \
  --model ollama/qwen3:32b \
  --oss-fuzz-dir oss-fuzz \
  --num-samples 1 \
  --run-timeout 60
```

### Step 6: Check results

```bash
ls results/output-psm-project/
cat results/report.json | python3 -m json.tool
```

---

## 7. CLI Reference

```
usage: run_logicfuzz.py [-h] [-n NUM_SAMPLES] [-t TEMPERATURE]
                        [-tr TEMPERATURE_LIST [TEMPERATURE_LIST ...]]
                        [-b BENCHMARKS_DIRECTORY] [-y BENCHMARK_YAML]
                        [-to RUN_TIMEOUT] [-a [AI_BINARY]] [-l MODEL]
                        [-w WORK_DIR] [--context]
                        [-e INTROSPECTOR_ENDPOINT]
                        [-lo {debug,info,error}]
                        [-of OSS_FUZZ_DIR] [-mr MAX_ROUND]
                        [--enable-source-filter]
                        [--source-filter-min-lines N]
                        [--list-models]
```

List all supported models:

```bash
python run_logicfuzz.py --list-models
```

---

## 8. Troubleshooting

| Problem | Solution |
|---------|----------|
| `No APIs extracted from project` | Ensure `build.sh` compiles successfully: `cd oss-fuzz && python infra/helper.py build_fuzzers --sanitizer address <project>` |
| `Docker permission denied` | Run with `sudo` or add your user to the `docker` group |
| Ollama connection refused | Verify `ollama serve` is running and `OLLAMA_BASE_URL` is correct |
| `Unknown model: ollama/...` | The `ollama/` prefix is required — e.g. `ollama/qwen3:32b`, not just `qwen3:32b` |
| Build fails in container | Inspect `results/output-*/build_log.txt` for compiler errors |
| Out of memory | Reduce `LLM_NUM_EXP` and `LLM_NUM_EVA` to `1` |

---

## 9. Cleanup and Reset

Use these commands to start from a clean state before re-running a benchmark.

### 9.1 Remove previous LogicFuzz artifacts

```bash
rm -rf results/*
```

### 9.2 Remove project Docker containers/images

```bash
PROJECT_NAME=psm

# Remove running/stopped containers created from this OSS-Fuzz project image
docker ps -aq --filter "ancestor=gcr.io/oss-fuzz/$PROJECT_NAME" | xargs -r docker rm -f

# Optional: remove the project image so it is rebuilt from scratch
docker image rm -f gcr.io/oss-fuzz/$PROJECT_NAME || true
```

### 9.3 Reset OSS-Fuzz build outputs for one project

```bash

cd /oss-fuzz/build/out
rm -rf ./*
cd /oss-fuzz/build/work
rm -rf ./*
```

### 9.4 Re-sync custom project files

When using custom projects, always keep canonical files under `data-dir/projects/<project>/`.
LogicFuzz syncs these files into `oss-fuzz/projects/<project>/` on each run when `OSS_FUZZ_DATA_DIR` is set.

```bash
OSS_FUZZ_DATA_DIR=$(pwd)/data-dir \
LLM_NUM_EXP=1 \
python3 run_logicfuzz.py \
  -y conti-benchmark/psm.yaml \
  --model ollama/qwen3:32b \
  --oss-fuzz-dir oss-fuzz \
  --num-samples 1 \
  --run-timeout 60

export VIO_BASE_URL="https://contivity.aws3116.ec1.aws.automotive.cloud:446"
export VIO_API_KEY="3VL15EK9Hb8SSFD01feX9okRKC7g7cRB3srmSC_192k"
# optional:
export VIO_MODEL="Default"

OSS_FUZZ_DATA_DIR=$(pwd)/data-dir \
LLM_NUM_EXP=1 \
python3 run_logicfuzz.py \
  -y conti-benchmark/psm.yaml \
  --model vio/Default \
  --oss-fuzz-dir oss-fuzz \
  --num-samples 1 \
  --run-timeout 60

  curl https://contivity.aws3116.ec1.aws.automotive.cloud:446/chat/completions \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer 3VL15EK9Hb8SSFD01feX9okRKC7g7cRB3srmSC_192k" \
  -d '{
    "model": "Default",
    "messages": [
      {"role": "system", "content": "You are a helpful assistant."},
      {"role": "user", "content": "Explain quantum computing"}
    ]
  }'
```
