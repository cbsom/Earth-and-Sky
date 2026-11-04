#!/usr/bin/env bash
set -euo pipefail
[[ $# == 1 && -x "$1" ]] || { echo 'Usage: smoke-test.sh /absolute/path/to/executable' >&2; exit 1; }
executable=$(realpath -- "$1")
test_root=$(mktemp -d)
trap 'rm -rf -- "$test_root"' EXIT
run() {
    env -u LD_LIBRARY_PATH -u QT_PLUGIN_PATH -u QML_IMPORT_PATH -u QML2_IMPORT_PATH \
        APPIMAGE_EXTRACT_AND_RUN=1 QT_QPA_PLATFORM=offscreen QT_QUICK_BACKEND=software \
        XDG_CONFIG_HOME="$test_root/config" timeout 30s "$executable" "$@"
}
run --self-test
mkdir -p "$test_root/config/EarthSky"
for mode in false true; do
    printf '[view]\nglobeMode=%s\n' "$mode" > "$test_root/config/EarthSky/EarthSky.conf"
    run --capture "$test_root/view-$mode.png"
    [[ -s "$test_root/view-$mode.png" ]] || { echo "Missing view capture: $mode" >&2; exit 1; }
done
echo 'Bundled self-tests and flat/globe rendering: PASS'
