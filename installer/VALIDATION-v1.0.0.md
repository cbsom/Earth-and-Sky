# Version 1.0 release validation

Status: candidate preparation started 2026-11-15. Not a published release.

Application/package version: 1.0.0. Intended tag: v1.0.0. Release source commit and final asset hashes will be recorded in the generated build-provenance file beside the packages.

## Windows

Toolchain: Qt 6.8.3 MSVC x64; Visual Studio 2026 Community, MSVC 14.52.37025; Inno Setup 7.1.1. Required QtQuick.Dialogs runtime is bundled by QML import scanning.

- Passed: Release build and complete astronomy/UI regression suite, including observer/polar/DST checks, label placement, maximum-zoom overlays, file-picker cancellation, PNG/JPEG export, write-error recovery and five-second/click confirmation dismissal.
- Passed: 1280 x 752 candidate screenshots inspected with the actual 1.0.0 version label.
- Passed: Qt runtime deployment with QtQuick.Dialogs, isolated installer compilation, installation into the workspace, installed executable hash matching the final release payload, self-tests and window capture, same-version reinstall preserving the Qt6Core DLL hash, and uninstall removing the installed executable.
- The isolated test uses `InstallerSmokeTest` and a separate AppId: shortcuts and uninstall-registry registration are disabled for that test only. The normal release installer retains them.
- Evidence: `work/release-1.0.0-tests.log`, `work/release-1.0.0-install.log`, `work/release-1.0.0-upgrade.log`, `work/release-1.0.0-uninstall.log`. Final SHA-256/provenance manifests identify the packaged artifacts.
- Remaining manual checks: normal Start menu/desktop shortcuts, upgrade from v0.1.1 with preferences retained, native Save As interaction, graphics on a clean Windows machine without the Qt SDK.
- Builds are unsigned.

## Linux

The available WSL Ubuntu has g++, but no CMake, Ninja or Linux Qt 6.8+ kit. No 1.0 Linux package or validation is claimed. Build on Mint 22 / Ubuntu 24.04 x86_64 with `installer/linux/build-packages.sh` and record test/toolchain evidence. `linux/VALIDATION.md` remains the historical v0.1.0 report.

## Publication gates

- Confirm the candidate's source and asset provenance; keep matching source and SHA-256 manifests.
- Finish Linux builds and desktop/clean-machine checks for the platforms offered.
- Supply corresponding source/build information for bundled copyleft dependencies according to RELEASE-CHECKLIST.md.
- Only then publish the stable release and update stable-download links. Automated development releases are prereleases and are not marked latest.
