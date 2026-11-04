# Linux AppImage build

Run inside x86_64 Linux (including Ubuntu under WSL). This builds the Linux executable from source and bundles Qt using linuxdeploy and its Qt plugin. Windows Inno Setup is not involved. The app currently requires **Qt 6.8 or newer**.

## Prepare Ubuntu / Mint

Install build dependencies in your Linux terminal:

```sh
sudo apt update
sudo apt install build-essential cmake ninja-build curl desktop-file-utils libgl1-mesa-dev libxkbcommon-dev libxkbcommon-x11-0 libxcb-cursor0 libxcb-icccm4 libxcb-image0 libxcb-keysyms1 libxcb-render-util0 libxcb-xinerama0 libxcb-xkb1 libegl1
```

Install the Linux desktop **Qt 6.8.3 gcc_64** kit using the Qt installer, including Qt Quick/QML and Qt SVG. For an existing Qt kit, use its actual path in the command below. A Windows MSVC Qt kit cannot be used, and Qt 6.4.2 is too old. Installing Qt in WSL is separate from the kit on your dual-boot Mint partition.

## Build

In WSL Ubuntu, run:

```sh
cd /mnt/c/repos_git/Earth-and-Sky
bash installer/linux/build-appimage.sh "$HOME/Qt/6.8.3/gcc_64"
```

On native Linux, use the path to your checkout instead. Builds are faster in the Linux filesystem than under `/mnt/c`; when copying the repository there, use the current working files so uncommitted changes are included.

The script downloads the official linuxdeploy and Qt-plugin AppImages on first use and caches them under `work/appimage/tools`. Internet is needed for that first download. These tools use upstream continuous builds; their SHA-256 values are recorded alongside the result. Cached tools can be replaced manually when intentionally updating the packaging toolchain.

Result: `outputs/installers/Earth-and-Sky-0.1.0-Linux-x86_64.AppImage` plus a checksum. The script runs built-in checks before and after bundling with isolated test preferences. Qt dependencies are bundled; core system and graphics libraries still come from Linux. Build-tool execution uses extraction mode, so it does not require FUSE in WSL. Work directories and generated packages are ignored by Git.

## Run and validate

```sh
chmod +x outputs/installers/Earth-and-Sky-0.1.0-Linux-x86_64.AppImage
./outputs/installers/Earth-and-Sky-0.1.0-Linux-x86_64.AppImage
```

If FUSE is unavailable, try `APPIMAGE_EXTRACT_AND_RUN=1 ./outputs/installers/Earth-and-Sky-0.1.0-Linux-x86_64.AppImage`. Test the GUI on Linux Mint, including Globe, Flat map, city search, saved settings, animation, and mouse input. An AppImage is a portable executable rather than a traditional package-manager installer.

Build on the oldest Linux base you intend to support; newer glibc dependencies can prevent running on older distributions. No minimum supported distro is claimed until the resulting package is tested there. The SDK license directory is copied when available; review all Qt and third-party notices/source obligations before publishing.

References: [linuxdeploy](https://github.com/linuxdeploy/linuxdeploy), [Qt plugin](https://github.com/linuxdeploy/linuxdeploy-plugin-qt), [AppImage best practices](https://docs.appimage.org/reference/best-practices.html).

## Current validation status

The script has been prepared on Windows. Linux compilation and AppImage runtime validation remain pending: this agent session cannot access WSL (`Wsl/E_ACCESSDENIED`). Git Bash syntax checks are also blocked by the Windows sandbox (`NtCreateDirectoryObject` access denied), so shell syntax execution remains unverified.
