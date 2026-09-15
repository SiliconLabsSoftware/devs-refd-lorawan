#!/usr/bin/env bash
# Runs inside the Sonar build-env container.
# Required env: REPO_DIR, BOARDS, APPS, SONAR_PARAMS
set -euo pipefail

make setup
BW=/opt/build-wrapper-linux-x86/build-wrapper-linux-x86-64
SONAR_BW_DIR=${REPO_DIR}/sonar-bw
CANON=${REPO_DIR}/projects/lorawan_extension/components/lbm_applications/4_porting_efr32
mkdir -p "${SONAR_BW_DIR}"

for board in ${BOARDS}; do
  for app in ${APPS}; do
    app_lc=$(echo "${app}" | tr A-Z a-z)
    variant_dir=${SONAR_BW_DIR}/main_${app_lc}_${board}
    echo "==> [build-wrapper] [${board}] ${app}"
    mkdir -p "${variant_dir}"
    "${BW}" --out-dir "${variant_dir}" make build BOARD="${board}" APP="${app}"
    python3 "${REPO_DIR}/.github/remap_compile_commands.py" \
      "${variant_dir}/compile_commands.json" "${CANON}"
  done
done

sonar-scanner ${SONAR_PARAMS}
