#!/bin/bash
# Prepare a customized project for LogicFuzz
# After running this script, use run_logicfuzz.py as normal
#
# Usage: ./scripts/prepare_custom_project.sh <project_name> [--start-server]

set -e

PROJECT=$1
START_SERVER=${2:-""}
DATA_DIR="data-dir"
OSS_FUZZ_DIR="$DATA_DIR/oss-fuzz2"
FI_DB_DIR="$DATA_DIR/fuzz_introspector_db"

if [ -z "$PROJECT" ]; then
  echo "Usage: $0 <project_name> [--start-server]"
  echo ""
  echo "This script prepares a customized project for LogicFuzz by:"
  echo "  1. Building the project with OSS-Fuzz"
  echo "  2. Generating Fuzz Introspector static analysis data"
  echo "  3. Optionally starting the FI server"
  echo ""
  echo "After preparation, run LogicFuzz normally:"
  echo "  python run_logicfuzz.py -y conti-benchmark/\$PROJECT.yaml --model gpt-5 -e http://127.0.0.1:8080/api"
  exit 1
fi

# Check project exists
if [ ! -d "$OSS_FUZZ_DIR/projects/$PROJECT" ]; then
  echo "Error: Project '$PROJECT' not found in $OSS_FUZZ_DIR/projects/"
  echo "Make sure you have:"
  echo "  - $OSS_FUZZ_DIR/projects/$PROJECT/Dockerfile"
  echo "  - $OSS_FUZZ_DIR/projects/$PROJECT/build.sh"
  echo "  - $OSS_FUZZ_DIR/projects/$PROJECT/project.yaml"
  exit 1
fi

echo "=== Preparing $PROJECT for LogicFuzz ==="

# Step 1: Build and run introspector
cd "$OSS_FUZZ_DIR"
echo "[1/2] Building project and running introspector..."
python3 infra/helper.py build_image "$PROJECT"
python3 infra/helper.py introspector "$PROJECT"

# Step 2: Copy FI data
echo "[2/2] Setting up FI database..."
mkdir -p "../fuzz_introspector_db"
cp -r "build/out/$PROJECT/introspector-report/"* "../fuzz_introspector_db/"

cd - > /dev/null

echo ""
echo "=== Preparation complete ==="

# Optionally start server
if [ "$START_SERVER" == "--start-server" ]; then
  echo "Starting FI server..."
  bash report/launch_introspector.sh --source data-dir --data-dir "$DATA_DIR" &
  sleep 10
  echo ""
  echo "FI server running at http://127.0.0.1:8080/api"
fi

echo ""
echo "Next: Run LogicFuzz with your benchmark YAML:"
echo "  python run_logicfuzz.py -y conti-benchmark/$PROJECT.yaml --model gpt-5 -e http://127.0.0.1:8080/api"
