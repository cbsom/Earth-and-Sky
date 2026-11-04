#!/usr/bin/env bash
set -euo pipefail
script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
bash "$script_dir/build-appimage.sh" "$@"
[[ ${1:-} == --help ]] && exit 0
bash "$script_dir/build-deb.sh"
