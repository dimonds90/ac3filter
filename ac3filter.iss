#ifndef appver
#define appver "test"
#endif

[Setup]
AppID=AC3Filter
AppVersion={#appver}
AppName=AC3Filter
AppVerName=AC3Filter {#appver}
AppPublisher=Alexander Vigovsky
AppPublisherURL=http://ac3filter.net
AppCopyright=Copyright (c) 2002-2009 by Alexander Vigovsky
DefaultDirName={pf}\AC3Filter
DefaultGroupName=AC3Filter
SolidCompression=yes
LanguageDetectionMethod=locale
MinVersion=0, 4.0

[Languages]
Name: en; MessagesFile: "compiler:Default.isl"
Name: eu; MessagesFile: "compiler:Languages\Basque.isl"
Name: pt_br; MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"
Name: ca; MessagesFile: "compiler:Languages\Catalan.isl"
Name: cs; MessagesFile: "compiler:Languages\Czech.isl"
Name: da; MessagesFile: "compiler:Languages\Danish.isl"
Name: nl; MessagesFile: "compiler:Languages\Dutch.isl"
Name: fi; MessagesFile: "compiler:Languages\Finnish.isl"
Name: fr; MessagesFile: "compiler:Languages\French.isl"
Name: de; MessagesFile: "compiler:Languages\German.isl"
Name: he; MessagesFile: "compiler:Languages\Hebrew.isl"
Name: hu; MessagesFile: "compiler:Languages\Hungarian.isl"
Name: it; MessagesFile: "compiler:Languages\Italian.isl"
Name: nn; MessagesFile: "compiler:Languages\Norwegian.isl"
Name: pl; MessagesFile: "compiler:Languages\Polish.isl"
Name: pt; MessagesFile: "compiler:Languages\Portuguese.isl"
Name: ru; MessagesFile: "compiler:Languages\Russian.isl"
Name: sk; MessagesFile: "compiler:Languages\Slovak.isl"
Name: sl; MessagesFile: "compiler:Languages\Slovenian.isl"
Name: es; MessagesFile: "compiler:Languages\Spanish.isl"

[Components]
Name: "prog";          Description: "Program files:"; Types: full compact
Name: "prog\filter32"; Description: "AC3Filter (32bit)"; Types: full compact
Name: "prog\filter64"; Description: "AC3Filter (64bit)"; Check: IsWin64; Types: full compact
Name: "prog\acm32";    Description: "AC3Filter ACM driver (32bit)"; Types: full compact
Name: "prog\acm64";    Description: "AC3Filter ACM driver (64bit)"; Check: IsWin64; Types: full compact

Name: "lang"; Description: "Language files"; Types: full

Name: "doc";     Description: "Documentation"; Types: full
Name: "doc\eng"; Description: "English"; Types: full
Name: "doc\rus"; Description: "Russian"; Types: full

[Files]
Source: "filter\Release\ac3filter.ax";             DestDir: "{app}"; Components: prog\filter32; Flags: 32bit Regserver RestartReplace UninsRestartDelete IgnoreVersion
Source: "intl\Release\ac3filter_intl.dll";         DestDir: "{app}"; Components: prog\filter32 and lang; Flags: 32bit RestartReplace UninsRestartDelete IgnoreVersion
Source: "tools\ac3config\Release\ac3config.exe";   DestDir: "{app}"; Components: prog\filter32; Flags: 32bit RestartReplace UninsRestartDelete IgnoreVersion
Source: "tools\spdif_test\Release\spdif_test.exe"; DestDir: "{app}"; Components: prog\filter32; Flags: 32bit RestartReplace UninsRestartDelete IgnoreVersion

Source: "filter\x64\Release\ac3filter64.ax";           DestDir: "{app}\x64"; Components: prog\filter64; Flags: 64bit Regserver RestartReplace UninsRestartDelete IgnoreVersion
Source: "intl\x64\Release\ac3filter64_intl.dll";       DestDir: "{app}\x64"; Components: prog\filter64 and lang; Flags: 64bit RestartReplace UninsRestartDelete IgnoreVersion
Source: "tools\ac3config\x64\Release\ac3config.exe";   DestDir: "{app}\x64"; Components: prog\filter64; Flags: 64bit RestartReplace UninsRestartDelete IgnoreVersion
Source: "tools\spdif_test\x64\Release\spdif_test.exe"; DestDir: "{app}\x64"; Components: prog\filter64; Flags: 64bit RestartReplace UninsRestartDelete IgnoreVersion

Source: "acm\Release\ac3filter.acm";       DestDir: "{sys}"; Components: prog\acm32; Flags: 32bit RestartReplace UninsRestartDelete IgnoreVersion
Source: "acm\x64\Release\ac3filter64.acm"; DestDir: "{sys}"; Components: prog\acm64; Flags: 64bit RestartReplace UninsRestartDelete IgnoreVersion

Source: "doc\*_rus.pdf"; DestDir: "{app}\doc"; Components: doc\rus
Source: "doc\*_eng.pdf"; DestDir: "{app}\doc"; Components: doc\eng

Source: "lang\lang\*"; DestDir: "{app}\lang"; Components: lang; Flags: recursesubdirs

Source: "reg\*.reg";   DestDir: "{app}"
Source: "Readme.txt";  DestDir: "{app}"
Source: "Changes.txt"; DestDir: "{app}"
Source: "GPL.txt";     DestDir: "{app}"

[Icons]
Name: "{group}\Documentation (eng)\AC3Filter User's Manual"; Filename: "{app}\doc\ac3filter_eng.pdf"; Components: doc\eng
Name: "{group}\Documentation (eng)\AC3Filter & SPDIF"; Filename: "{app}\doc\spdif_eng.pdf"; Components: doc\eng
Name: "{group}\Documentation (eng)\Loudness and dynamic range"; Filename: "{app}\doc\loudness_eng.pdf"; Components: doc\eng

Name: "{group}\Documentation (rus)\AC3Filter - Руководство пользователя"; Filename: "{app}\doc\ac3filter_rus.pdf"; Components: doc\rus
Name: "{group}\Documentation (rus)\AC3Filter & SPDIF"; Filename: "{app}\doc\spdif_rud.pdf"; Components: doc\rus
Name: "{group}\Documentation (rus)\Громкость и динамический диапазон"; Filename: "{app}\doc\loudness_rus.pdf"; Components: doc\rus

Name: "{group}\AC3Filter home"; Filename: "http://ac3filter.net"
Name: "{group}\AC3Filter Config"; Filename: "{app}\ac3config.exe"; Components: prog\filter32
Name: "{group}\AC3Filter Config"; Filename: "{app}\x64\ac3config.exe"; Components: prog\filter64 and not prog\filter32
Name: "{group}\SPDIF test (32bit)"; Filename: "{app}\spdif_test.exe"; Components: prog\filter32
Name: "{group}\SPDIF test (64bit)"; Filename: "{app}\x64\spdif_test.exe"; Components: prog\filter64

Name: "{group}\Readme"; Filename: "{app}\Readme.txt"
Name: "{group}\Reset to defaults"; Filename: "{app}\Reset to defaults.reg"
Name: "{group}\Restore default presets"; Filename: "{app}\Presets.reg"
Name: "{group}\Uninstall AC3Filter"; Filename: "{uninstallexe}"

[Registry]
Root: HKCU; Subkey: "Software\AC3Filter"; Flags: uninsdeletekey
Root: HKCU; Subkey: "Software\AC3Filter"; ValueType: string; ValueName: "Install_Dir"; ValueData: "{app}"
Root: HKCU; Subkey: "Software\AC3Filter"; ValueType: string; ValueName: "Lang_Dir"; ValueData: "{app}\Lang"; Components: lang;
Root: HKCU; Subkey: "Software\AC3Filter"; ValueType: dword;  ValueName: "tray"; ValueData: 1
Root: HKCU; Subkey: "Software\AC3Filter"; ValueType: dword;  ValueName: "refresh_time"; ValueData: 50

Root: HKLM32; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueType: string; ValueName: "msacm.ac3filter"; ValueData: "ac3filter.acm"; Components: prog\acm32; Flags: uninsdeletevalue
Root: HKLM32; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers.desc"; ValueType: string; ValueName: "ac3filter.acm"; ValueData: "AC3Filter ACM codec"; Components: prog\acm32; Flags: uninsdeletevalue

Root: HKLM64; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueType: string; ValueName: "msacm.ac3filter"; ValueData: "ac3filter64.acm"; Components: prog\acm64; Flags: uninsdeletevalue
Root: HKLM64; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers.desc"; ValueType: string; ValueName: "ac3filter.acm"; ValueData: "AC3Filter ACM codec"; Components: prog\acm64; Flags: uninsdeletevalue

[Run]
Filename: "regedit"; Parameters: "/s ""{app}\Presets.reg""";
Filename: "regedit"; Parameters: "/s ""{app}\Reset to defaults.reg""";

