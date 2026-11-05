#!/usr/bin/env bash
set -euo pipefail
if [[ ${1:-} == --help ]]; then
    echo 'Usage: bash installer/linux/build-appimage.sh /path/to/Qt/6.8.3/gcc_64'
    echo 'Builds and packages a Linux x86_64 AppImage; Qt 6.8+ is required.'
    exit 0
fi
[[ $(uname -s) == Linux && $(uname -m) == x86_64 ]] || { echo 'Run inside x86_64 Linux or WSL Ubuntu.' >&2; exit 1; }
repo=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../.." && pwd)
source_dir="$repo/outputs/Earth-and-Sky/source"
app_version=$(sed -n 's/^#define AppVersion "\([^"]*\)"/\1/p' "$repo/installer/Earth-and-Sky.iss")
[[ "$app_version" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]] || { echo "Invalid installer version" >&2; exit 1; }
qt_prefix=${1:-${QT_ROOT:-}}
[[ -n "$qt_prefix" && -x "$qt_prefix/bin/qmake" ]] || { echo 'Supply a Linux Qt 6.8+ kit path, e.g. "$HOME/Qt/6.8.3/gcc_64".' >&2; exit 1; }
qt_prefix=$(cd -- "$qt_prefix" && pwd)
for command in cmake ninja c++ curl sha256sum desktop-file-validate; do
    command -v "$command" >/dev/null || { echo "Missing build tool: $command. See installer/linux/README.md." >&2; exit 1; }
done
version=$("$qt_prefix/bin/qmake" -query QT_VERSION)
# CMake checks the exact minimum too; avoid downloading tools for an incompatible kit.
IFS=. read -r major minor patch <<< "$version"
(( major == 6 && minor >= 8 )) || { echo "Qt 6.8+ required; selected kit is $version." >&2; exit 1; }
work="$repo/work/appimage"
build="$work/build"
tools="$work/tools"
output="$repo/outputs/installers"
mkdir -p "$build" "$tools" "$output"
# A fresh staging directory prevents removed dependencies lingering in later builds.
# Retain older stages for inspection rather than recursively deleting directories.
appdir=$(mktemp -d "$work/Earth-and-Sky.XXXXXX.AppDir")
chmod 755 "$appdir"
export PATH="$qt_prefix/bin:$tools:$PATH"
export QMAKE="$qt_prefix/bin/qmake"
export QML_SOURCES_PATHS="$source_dir"
export LD_LIBRARY_PATH="$qt_prefix/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
cmake -S "$source_dir" -B "$build" -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$qt_prefix"
cmake --build "$build" --parallel "${BUILD_JOBS:-2}"
mkdir -p "$work/test-config"
QT_QPA_PLATFORM=offscreen QT_QUICK_BACKEND=software XDG_CONFIG_HOME="$work/test-config" "$build/Earth-and-Sky" --self-test
install -Dm755 "$build/Earth-and-Sky" "$appdir/usr/bin/Earth-and-Sky"
install -Dm644 "$repo/installer/linux/earth-and-sky.desktop" "$appdir/usr/share/applications/earth-and-sky.desktop"
install -Dm644 "$source_dir/assets/earth-and-sky.png" "$appdir/usr/share/icons/hicolor/256x256/apps/earth-and-sky.png"
desktop-file-validate "$appdir/usr/share/applications/earth-and-sky.desktop"
docs="$appdir/usr/share/doc/earth-and-sky"
mkdir -p "$docs"
cp "$repo/LICENSE" "$docs/LICENSE"
cp "$repo/THIRD-PARTY-NOTICES.txt" "$docs/THIRD-PARTY-NOTICES.txt"
cp "$repo/outputs/Earth-and-Sky/README.md" "$docs/README.md"
cp "$source_dir/assets/README.md" "$docs/IMAGE-CREDITS.md"
cp "$source_dir/data/D3-CELESTIAL-LICENSE" "$docs/CONSTELLATION-LICENSE"
cp "$source_dir/vendor/LICENSE" "$docs/ASTRONOMY-ENGINE-LICENSE"
# Include the notices shipped with the selected SDK when present.
for licenses in "$qt_prefix/../../Licenses" "$qt_prefix/../Licenses" "$qt_prefix/licenses"; do
    if [[ -d "$licenses" ]]; then cp -a "$licenses" "$docs/Qt-licenses"; break; fi
done
fetch() {
    local file="$1" url="$2"
    if [[ ! -s "$tools/$file" ]]; then
        curl --fail --location --retry 3 "$url" -o "$tools/$file.part"
        mv -- "$tools/$file.part" "$tools/$file"
    fi
    chmod +x "$tools/$file"
}
fetch linuxdeploy-x86_64.AppImage https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
fetch linuxdeploy-plugin-qt-x86_64.AppImage https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage
sha256sum "$tools/"*.AppImage > "$output/Earth-and-Sky-Linux-build-tools.sha256"
# Run build tools without requiring FUSE, useful in WSL and containers.
export APPIMAGE_EXTRACT_AND_RUN=1
export LDAI_OUTPUT="$output/Earth-and-Sky-${app_version}-Linux-x86_64.AppImage"
export LDAI_VERSION="$app_version"
export EXTRA_QT_MODULES=svg
# Bundle the offscreen platform for checks without a desktop or SDK.
export EXTRA_PLATFORM_PLUGINS="libqoffscreen.so${EXTRA_PLATFORM_PLUGINS:+;$EXTRA_PLATFORM_PLUGINS}"
# Cache the AppImage runtime too; appimagetool otherwise downloads it each time.
fetch runtime-x86_64 https://github.com/AppImage/type2-runtime/releases/download/continuous/runtime-x86_64
export LDAI_RUNTIME_FILE="$tools/runtime-x86_64"
sha256sum "$tools/runtime-x86_64" >> "$output/Earth-and-Sky-Linux-build-tools.sha256"
cd -- "$work"
"$tools/linuxdeploy-x86_64.AppImage" --appdir "$appdir" --plugin qt --output appimage
[[ -s "$LDAI_OUTPUT" ]] || { echo 'AppImage output missing.' >&2; exit 1; }
chmod +x "$LDAI_OUTPUT"
bash "$repo/installer/linux/smoke-test.sh" "$LDAI_OUTPUT"
(cd -- "$output" && sha256sum "$(basename -- "$LDAI_OUTPUT")") > "$LDAI_OUTPUT.sha256"
printf '%s\n' "$appdir" > "$work/latest-appdir.txt"
printf '\nCreated: %s\nTest on a normal Linux desktop before publishing.\n' "$LDAI_OUTPUT"
