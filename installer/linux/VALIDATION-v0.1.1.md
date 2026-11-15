# Linux v0.1.1 validation — 2026-11-08

Source commit: `ecb0db634a26e67be83f47e6df6e59b3be61d5dc`.
`git pull --ff-only` reported already up to date; the working tree was clean
before building. No application or packaging source changes were made.

Built with Linux Qt 6.8.3 gcc_64 from `/home/chaim/Qt/6.8.3/gcc_64`,
GCC 13.3.0, Release/Ninja, on Linux Mint 22.3 (Ubuntu 24.04), x86_64.
Command: `bash installer/linux/build-packages.sh /home/chaim/Qt/6.8.3/gcc_64`.
The first run compiled and passed self-tests but sandbox DNS blocked tool downloads;
the successful rerun used network access to download the official tools.
No Qt or dependency patches were applied in this session.

## Passed

- Application self-tests: astronomy, twilight, lunar phase, zodiac, navigation and globe.
- Final AppImage and extracted final .deb: self-tests and flat/globe PNG captures,
  using offscreen/software rendering, isolated preferences, and cleared SDK
  library/plugin/QML environment variables.
- Visually inspected an additional capture of the extracted .deb: rendered globe
  and footer `EARTH & SKY v0.1.1`.
- Debian metadata: version 0.1.1, architecture amd64, private runtime under
  `/opt/earth-and-sky`, generated system dependencies (including libc6 >= 2.38).
- Every Debian archive member has root:root ownership; directories are traversable.
- Desktop launcher validation and shell syntax checks.
- Both package SHA-256 files verified from `outputs/installers`.
- All existing v0.1.0 Linux packages and checksum files retain their original hashes.

## Evidence and artifacts

Packages and relative-filename checksums are in `outputs/installers`.
Build logs, preserved v0.1.0 hashes and footer capture are in `work/release-0.1.1`.
Completed AppDir: `work/appimage/Earth-and-Sky.VEhxU0.AppDir`.
Debian stage/extraction and dependency scan: `work/deb/package.VmJrhF`.
Versioned packaging-tool hashes accompany the packages. These tools came from
upstream continuous releases; they are not assumed identical to the v0.1.0 tools.
The dependency scan emitted unused-linkage warnings, with no fatal errors.
linuxdeploy could not find SDK copyright files through dpkg-query; available Qt
SDK licenses and project/third-party notices were copied by the packaging script.

## Remaining before publication

- Normal desktop .deb installation, menu launch, upgrade and removal.
- Interactive city search, saved settings, animation, dragging, zoom and GPU checks.
- Clean-machine testing without a Qt SDK; older distributions and ARM64 remain unvalidated.
- Review exact corresponding Qt/dependency source availability and license obligations
  described in RELEASE-CHECKLIST.md. The application snapshot excludes Qt sources.

No application was installed or removed. No release, tag or asset was published
or changed on GitHub. Existing v0.1.0 downloads were preserved.
