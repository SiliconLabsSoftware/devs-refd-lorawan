#!/bin/bash

# This script builds the project components and collects coverage data for SonarQube analysis.
# It is designed to be used inside Docker containers set up for SonarQube scanning.
# Usage: ./sonar_build.sh [target]
# where [target] can be "all" or a space-separated list of components/variants to build.

set -euo pipefail

# --- Configuration ---
COMPONENTS=${COMPONENTS}
PYTHON_VARIANTS=${PYTHON_VARIANTS}

BW_PATH="/opt/build-wrapper-linux-x86/build-wrapper-linux-x86-64"
BOARDS="brd4405a brd4400c brd4187c xiao_mg24"
CANON="${PWD}/projects/lorawan_extension/components/lbm_applications/4_porting_efr32"

TARGET=${1:-"all"}

echo ">>> [sonar_build.sh] Starting build process for target: $TARGET"

parse_variant() {
    local comp=$1
    local board app_lc app

    for board in $BOARDS; do
        if [[ "$comp" == *_${board} ]]; then
            app_lc=${comp#main_}
            app_lc=${app_lc%_${board}}
            app=$(echo "$app_lc" | tr '[:lower:]' '[:upper:]')
            echo "$board $app"
            return 0
        fi
    done

    echo "Error: could not parse board/app from variant '$comp'" >&2
    return 1
}

rewrite_compile_commands() {
    local out_dir=$1

    python3 - "${out_dir}/compile_commands.json" "${CANON}" <<'PY'
import json
import sys

path, canon = sys.argv[1:3]
prefixes = (
    "main_examples/",
    "radio_hal/",
    "smtc_hal_efr32/",
    "smtc_modem_hal/",
    "smtc_modem_crypto/",
)

with open(path, encoding="utf-8") as f:
    entries = json.load(f)

rewritten = 0
for entry in entries:
    file_path = entry.get("file", "")
    if file_path.startswith(canon + "/"):
        rel = file_path[len(canon) + 1 :]
        abs_file = file_path
    elif file_path.startswith(prefixes):
        rel = file_path
        abs_file = f"{canon}/{rel}"
        entry["file"] = abs_file
    else:
        continue

    args = entry.get("arguments")
    if isinstance(args, list):
        for i in range(len(args) - 1, -1, -1):
            if args[i] == rel:
                args[i] = abs_file
                rewritten += 1
                break

if rewritten == 0:
    sys.stderr.write("rewrite_compile_commands: no in-scope entries rewritten\n")
    sys.exit(1)

with open(path, "w", encoding="utf-8") as f:
    json.dump(entries, f, indent=2)
    f.write("\n")

print(f"rewrite_compile_commands: rewritten {rewritten} entries")
PY
}

#This is for Python variants. IT IS  EXPERIMENTAL USE WITH CAUTION
if [ -n "$PYTHON_VARIANTS" ]; then
    for variant in $PYTHON_VARIANTS; do
        if [[ "$TARGET" == "all" ]] || [[ " $TARGET " =~ " $variant " ]]; then
            echo ">>> [sonar_build.sh] Processing Python variant: $variant"
            PROJECT_DIR="projects/$variant"

            if [ -d "$PROJECT_DIR" ]; then
                pushd "$PROJECT_DIR" > /dev/null

                if [ ! -d ".venv" ]; then
                    echo "Creating virtual environment for $variant..."
                    make venv
                fi

                source .venv/bin/activate

                echo "Installing coverage..."
                python3 -m pip install coverage

                echo "Running tests for $variant..."
                python3 -m coverage run --branch -m unittest discover -s tests/unit

                echo "Generating XML report: coverage.xml"
                python3 -m coverage xml -o "coverage.xml"

                if [ ! -f "coverage.xml" ]; then
                    echo "Error: coverage.xml was not generated for $variant"
                    exit 1
                fi
                if [ ! -s "coverage.xml" ]; then
                    echo "Error: coverage.xml is empty for $variant"
                    exit 1
                fi

                deactivate
                popd > /dev/null
            else
                echo "Warning: $PROJECT_DIR directory not found. Skipping."
            fi
        fi
    done
fi

if [ -n "$COMPONENTS" ]; then
    BW_CHECKED=false
    SETUP_DONE=false

    for comp in $COMPONENTS; do
        if [[ "$TARGET" == "all" ]] || [[ " $TARGET " =~ " $comp " ]]; then

            if [ "$BW_CHECKED" = false ]; then
                echo ">>> [sonar_build.sh] Building C/C++ components with build-wrapper..."

                if [ ! -f "$BW_PATH" ]; then
                    echo "Warning: build-wrapper not found at $BW_PATH. Checking PATH..."
                    if command -v build-wrapper-linux-x86-64 &> /dev/null; then
                        BW_PATH="build-wrapper-linux-x86-64"
                    else
                        echo "Error: build-wrapper-linux-x86-64 not found. Cannot proceed with CFamily analysis."
                        exit 1
                    fi
                fi

                mkdir -p sonar-bw
                BW_CHECKED=true
            fi

            if [ "$SETUP_DONE" = false ]; then
                echo ">>> [sonar_build.sh] Running make setup..."
                make setup
                SETUP_DONE=true
            fi

            read -r board app <<< "$(parse_variant "$comp")"
            OUT_DIR="sonar-bw/$comp"

            echo ">>> [sonar_build.sh] Building component: $comp (BOARD=$board APP=$app)"
            $BW_PATH --out-dir "$OUT_DIR" make build BOARD="$board" APP="$app"
            rewrite_compile_commands "$OUT_DIR"

            echo ">>> [sonar_build.sh] Finished building $comp"
        fi
    done
fi

echo ">>> [sonar_build.sh] Build process complete."
