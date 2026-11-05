#!/usr/bin/env bash
set -euo pipefail
if [[ ${1:-} == --help ]]; then
    echo 'Usage: bash installer/linux/build-deb.sh [/path/to/completed.AppDir]'
    echo 'Run build-appimage.sh first. Builds an amd64 package for Ubuntu 24.04 / Mint 22.'
    exit 0
fi
repo=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../.." && pwd)
for tool in dpkg-deb dpkg-shlibdeps readelf desktop-file-validate sha256sum; do
    command -v "$tool" >/dev/null || { echo "Missing tool: $tool (install dpkg-dev binutils desktop-file-utils)." >&2; exit 1; }
done
[[ $(dpkg --print-architecture) == amd64 ]] || { echo 'Only amd64 builds are supported.' >&2; exit 1; }
version=$(sed -n 's/^#define AppVersion "\([^"]*\)"/\1/p' "$repo/installer/Earth-and-Sky.iss")
[[ "$version" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]] || { echo 'Invalid installer version.' >&2; exit 1; }
appdir=${1:-}
if [[ -z "$appdir" && -f "$repo/work/appimage/latest-appdir.txt" ]]; then
    IFS= read -r appdir < "$repo/work/appimage/latest-appdir.txt"
fi
[[ -n "$appdir" && -x "$appdir/usr/bin/Earth-and-Sky" && -f "$appdir/usr/bin/qt.conf" ]] || {
    echo 'Run build-appimage.sh first, or supply its completed AppDir.' >&2; exit 1;
}
appdir=$(cd -- "$appdir" && pwd)
mkdir -p "$repo/work/deb" "$repo/outputs/installers"
work=$(mktemp -d "$repo/work/deb/package.XXXXXX")
stage="$work/root"
payload="$stage/opt/earth-and-sky"
mkdir -p "$payload" "$stage/DEBIAN" "$stage/usr/bin" "$work/debian"
cp -a "$appdir/." "$payload/"
# AppRun and all library RPATHs remain relative within the private bundle.
cat > "$stage/usr/bin/earth-and-sky" <<'EOF'
#!/bin/sh
exec /opt/earth-and-sky/AppRun "$@"
EOF
chmod 755 "$stage/usr/bin/earth-and-sky"
install -Dm644 "$repo/installer/linux/earth-and-sky.desktop" "$stage/usr/share/applications/earth-and-sky.desktop"
sed -i 's/^Exec=.*/Exec=earth-and-sky/' "$stage/usr/share/applications/earth-and-sky.desktop"
install -Dm644 "$repo/outputs/Earth-and-Sky/source/assets/earth-and-sky.png" "$stage/usr/share/icons/hicolor/256x256/apps/earth-and-sky.png"
desktop-file-validate "$stage/usr/share/applications/earth-and-sky.desktop"
mkdir -p "$stage/usr/share/doc/earth-and-sky"
cp -a "$payload/usr/share/doc/earth-and-sky/." "$stage/usr/share/doc/earth-and-sky/"
# Refresh notices even when the completed AppDir predates them.
for destination in "$payload/usr/share/doc/earth-and-sky" "$stage/usr/share/doc/earth-and-sky"; do
    cp "$repo/LICENSE" "$destination/LICENSE"
    cp "$repo/THIRD-PARTY-NOTICES.txt" "$destination/THIRD-PARTY-NOTICES.txt"
    cp "$repo/THIRD-PARTY-NOTICES.txt" "$destination/copyright"
done
cat > "$work/debian/control" <<'EOF'
Source: earth-and-sky
Section: science
Priority: optional
Maintainer: cbsom <cbsom@users.noreply.github.com>

Package: earth-and-sky
Architecture: amd64
Description: Offline Earth and sky explorer
EOF
# Scan every bundled ELF, including dynamically loaded Qt/QML plugins. Private
# libraries have no dpkg metadata; system libraries supply versioned dependencies.
elf_args=()
while IFS= read -r -d '' file; do
    if readelf -h "$file" >/dev/null 2>&1; then elf_args+=("-e$file"); fi
done < <(find "$payload" -type f -print0)
[[ ${#elf_args[@]} -gt 0 ]] || { echo 'No ELF binaries found.' >&2; exit 1; }
dependencies=$(cd -- "$work" && dpkg-shlibdeps --ignore-missing-info -O \
    "-l$payload/usr/lib" "${elf_args[@]}" 2> "$work/shlibdeps.log")
dependencies=${dependencies#shlibs:Depends=}
[[ "$dependencies" == *libc6* ]] || { echo 'System dependency detection failed.' >&2; exit 1; }
size=$(du -sk "$stage" | cut -f1)
cat > "$stage/DEBIAN/control" <<EOF
Package: earth-and-sky
Version: $version
Section: science
Priority: optional
Architecture: amd64
Maintainer: cbsom <cbsom@users.noreply.github.com>
Homepage: https://github.com/cbsom/Earth-and-Sky
Depends: $dependencies
Installed-Size: $size
Description: Offline Earth and sky explorer
 Explore daylight, twilight, the Sun, Moon, planets and constellations
 using interactive flat-map and globe views. Includes a private Qt runtime.
EOF
# No maintainer scripts: package removal removes the launcher and payload while
# retaining preferences in the user's home directory. Desktop caches use distro triggers.
chmod -R go-w "$stage"
# mktemp AppDirs start at 0700; installed payload must be traversable by users.
find "$stage" -type d -exec chmod 755 {} +
output="$repo/outputs/installers/Earth-and-Sky-${version}-Linux-amd64.deb"
dpkg-deb --root-owner-group -Zxz -z6 --build "$stage" "$output"
extracted="$work/extracted"
dpkg-deb --extract "$output" "$extracted"
bash "$repo/installer/linux/smoke-test.sh" "$extracted/opt/earth-and-sky/AppRun"
(cd -- "$(dirname -- "$output")" && sha256sum "$(basename -- "$output")") > "$output.sha256"
printf '\nCreated: %s\nDependency scan log: %s\n' "$output" "$work/shlibdeps.log"
