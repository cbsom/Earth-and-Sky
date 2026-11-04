#define AppName "Earth & Sky"
#define AppVersion "0.1.0"
#define PackageRoot "..\outputs\Earth-and-Sky"
#ifndef InstallerAppId
  #define InstallerAppId "E4C8B727-1050-4B93-BE33-F7281E60A73B"
#endif

[Setup]
AppId={#InstallerAppId}
AppName={#AppName}
AppVersion={#AppVersion}
AppPublisher=cbsom
AppPublisherURL=https://github.com/cbsom/Earth-and-Sky
AppSupportURL=https://github.com/cbsom/Earth-and-Sky/issues
AppUpdatesURL=https://github.com/cbsom/Earth-and-Sky/releases
DefaultDirName={localappdata}\Programs\Earth-and-Sky
DefaultGroupName={#AppName}
DisableProgramGroupPage=yes
PrivilegesRequired=lowest
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
MinVersion=10.0
UninstallDisplayIcon={app}\Earth-and-Sky.exe
SetupIconFile={#PackageRoot}\source\assets\earth-and-sky.ico
OutputDir=..\outputs\installers
OutputBaseFilename=Earth-and-Sky-{#AppVersion}-Windows-x64-Setup
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
CloseApplications=yes
RestartApplications=no
SetupLogging=yes
#ifdef InstallerSmokeTest
CreateUninstallRegKey=no
#endif

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "Create a &desktop shortcut"; GroupDescription: "Shortcuts:"; Flags: unchecked

[Files]
; Keep matching Qt DLLs, plugins, and QML imports private to this application.
; Inno's normal version/content checks skip identical files on upgrades.
; Do not use onlyifdoesntexist: changed runtime dependencies must be upgraded.
Source: "{#PackageRoot}\app\*"; DestDir: "{app}"; Flags: recursesubdirs createallsubdirs replacesameversion
Source: "{#PackageRoot}\README.md"; DestDir: "{app}\documentation"; Flags: ignoreversion
Source: "{#PackageRoot}\screenshots\*"; DestDir: "{app}\documentation\screenshots"; Flags: ignoreversion recursesubdirs
Source: "{#PackageRoot}\source\assets\README.md"; DestDir: "{app}\documentation\source\assets"; Flags: ignoreversion
Source: "{#PackageRoot}\source\data\D3-CELESTIAL-LICENSE"; DestDir: "{app}\licenses"; Flags: ignoreversion
Source: "{#PackageRoot}\source\vendor\LICENSE"; DestDir: "{app}\licenses"; DestName: "Astronomy-Engine-MIT.txt"; Flags: ignoreversion

[Icons]
#ifndef InstallerSmokeTest
Name: "{group}\{#AppName}"; Filename: "{app}\Earth-and-Sky.exe"
Name: "{autodesktop}\{#AppName}"; Filename: "{app}\Earth-and-Sky.exe"; Tasks: desktopicon

#endif

[Run]
Filename: "{app}\Earth-and-Sky.exe"; Description: "Launch {#AppName}"; Flags: nowait postinstall skipifsilent

; Preferences are deliberately preserved during upgrades and uninstall.
