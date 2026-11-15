# Linux 1.0.0 validation — 2026-11-15

Built from pushed tag `v1.0.0`, commit `3505acbfd5732887061f068839fc0be4bdeee17c`.
The main checkout remains on master; an isolated detached worktree is retained at
`work/release-1.0.0/source`. No application or packaging source was changed.

Toolchain: Linux Mint 22.3 / Ubuntu 24.04 x86_64, GCC 13.3.0,
Qt 6.8.3 gcc_64 (`/home/chaim/Qt/6.8.3/gcc_64`), CMake/Ninja Release.
Packaging command: `bash installer/linux/build-packages.sh /home/chaim/Qt/6.8.3/gcc_64`.
Cached official linuxdeploy/Qt plugin/runtime tools from the November 8 build
were copied into the worktree. Versioned tool hashes and JSON provenance accompany
packages in the main `outputs/installers` folder. No dependency patches applied.

## Passed

- Fresh Release build and built-in astronomy, date/time, observer, polar daylight,
  DST, twilight, lunar phase, zodiac, navigation and globe tests.
- Both final packages passed self-tests and flat/globe capture checks with isolated
  preferences, SDK environment paths cleared, offscreen/software rendering.
- UI regression suite passed date entry, location panel, playback, responsive
  layouts, maximum-zoom overlays, PNG/JPEG export, compact credits, five-second
  and click dismissal, file-picker cancellation and write-error recovery.
- Visually inspected location panel, export dialog and exported globe screenshots;
  the UI footer shows v1.0.0. Captures are in `work/release-1.0.0/snapshots`.
- Final .deb metadata: earth-and-sky 1.0.0 amd64; ELF dependency scan completed,
  including libc6 >= 2.38. All archive members are root:root, directories traversable.
- Project/third-party notices and QtQuick.Dialogs runtime present in .deb.
- Desktop validation in packaging scripts, shell syntax and diff whitespace checks.
- Package SHA-256 manifests verified. Previous v0.1.0 and v0.1.1 downloads unchanged.

## Linux UI test adjustment

The unchanged UI test failed at `cancel export with Escape` after programmatically
closing the fallback file picker. Waiting alone did not resolve it. Explicitly
reactivating the main window and allowing event processing after closing the picker
made the complete suite pass without QML warnings. This is a test-only adjustment;
application/package payloads remain from the tag. Reviewable patch:
`work/release-1.0.0/linux-ui-focus.patch`. It remains uncommitted in the build worktree.
Logs: `ui-tests.log`, `ui-tests-settled.log`, `ui-tests-activation.log`, and UI build
logs under `work/release-1.0.0`. Full packaging log: `build.log` there.

## Remaining before publication

- Normal Mint desktop installation/menu launch, upgrade from 0.1.1 preserving
  preferences, removal, native Save As, mouse/keyboard and GPU checks.
- Clean-machine testing without Qt SDK; older distributions, ARM64 and native
  Wayland remain unvalidated. Software/offscreen checks do not establish these.
- Corresponding dependency-source and notices review per RELEASE-CHECKLIST.md.
  linuxdeploy reports SDK copyright lookup warnings through dpkg-query; available
  SDK licenses are copied by the packaging script. No dependency-source claim made.

No system installation/removal or GitHub publication was performed.
No tags were changed and no commits were pushed. Historical validation reports
and existing release downloads were preserved.
