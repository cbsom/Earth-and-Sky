# Windows installer

Install Inno Setup 7, then run from the repository root:

```bat
installer\build-installer.cmd
```

An optional first argument is the full path to `ISCC.exe`. Build and deploy the current application into `outputs/Earth-and-Sky/app` first; this command packages that folder and does not rebuild C++. The installer is written to `outputs/installers/Earth-and-Sky-0.1.0-Windows-x64-Setup.exe`.

The installer targets Windows 10/11 x64 (including x64 emulation on ARM64), installs for the current user under `%LOCALAPPDATA%\Programs\Earth-and-Sky`, and requires no administrator elevation. It creates a Start menu shortcut and offers an optional desktop shortcut and launch checkbox. Windows Settings can uninstall it. Saved home city, view, and layer preferences are retained during upgrades and uninstall.

Qt DLLs, QML imports, plugins, and compiler runtimes remain private to the application. Versioned files with identical versions and contents are skipped during upgrades; files with changed contents at the same version are replaced. No system-wide Qt installation is required or modified. The installer includes bundled license notices and documentation.

Keep `InstallerAppId` stable across releases. Update `AppVersion` in the `.iss` script and the application version metadata for a new release. The compiler accepts `/DInstallerAppId=EarthSkyInstallerTest` for an isolated test identity. Builds are unsigned until a code-signing certificate is configured.

## Validation

The installer payload was installed into an isolated test directory, the installed executable passed `--self-test`, a second install skipped identical Qt DLLs (verified by SHA-256 in the setup log), and uninstall removed the installed files successfully. The test build used `/DInstallerSmokeTest` to disable shortcut creation and uninstall-registry registration because the agent sandbox blocks those Windows locations. Those actions remain enabled in the normal build and require an interactive installation check outside the sandbox.

## Licensing and source snapshots

Every installer includes the project GPL license and THIRD-PARTY-NOTICES.txt. Qt, city data, images and vendored code retain their respective licenses. Run `python installer/package-source.py` to archive the current application source and build scripts with a file-hash manifest; preserve the matching snapshot when building binaries. See `../RELEASE-CHECKLIST.md` for source distribution and Qt dependency-source requirements before publishing.
