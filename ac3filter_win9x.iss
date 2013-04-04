#ifndef appver
#define appver GetFileVersion("filter\Release\ac3filter.ax")
#endif
#define filever StringChange(appver, '.', '_')

[Setup]
AppID=AC3Filter
AppVersion={#appver}
AppName=AC3Filter for Win9x
AppVerName=AC3Filter {#appver}
AppPublisher=Alexander Vigovsky
AppPublisherURL=http://ac3filter.net
AppCopyright=Copyright (c) 2002-2013 by Alexander Vigovsky
OutputBaseFilename=ac3filter_{#filever}_win9x
DefaultDirName={pf}\AC3Filter
DefaultGroupName=AC3Filter
AllowNoIcons=yes
SolidCompression=yes
LanguageDetectionMethod=locale

[Languages]
Name: baq; MessagesFile: "compiler:Default.isl,lang\isl\Basque-1-5.1.11.isl"
Name: bul; MessagesFile: "compiler:Default.isl,lang\isl\Bulgarian-5.1.11.isl"
Name: eng; MessagesFile: "compiler:Default.isl"
Name: cze; MessagesFile: "compiler:Languages\Czech.isl"
Name: dan; MessagesFile: "compiler:Languages\Danish.isl"
Name: dut; MessagesFile: "compiler:Languages\Dutch.isl"
Name: fin; MessagesFile: "compiler:Languages\Finnish.isl"
Name: fre; MessagesFile: "compiler:Languages\French.isl"
Name: ger; MessagesFile: "compiler:Languages\German.isl"
Name: gre; MessagesFile: "compiler:Default.isl,lang\isl\Greek-4-5.1.11.isl"
Name: heb; MessagesFile: "compiler:Languages\Hebrew.isl"
Name: hun; MessagesFile: "compiler:Languages\Hungarian.isl"
Name: ind; MessagesFile: "compiler:Default.isl,lang\isl\Indonesian-5.1.11.isl"
Name: ita; MessagesFile: "compiler:Languages\Italian.isl"
Name: jpn; MessagesFile: "compiler:Default.isl,lang\isl\Japanese-5-5.1.11.isl"
Name: kor; MessagesFile: "compiler:Default.isl,lang\isl\Korean-5-5.1.11.isl"
Name: pol; MessagesFile: "compiler:Languages\Polish.isl"
Name: pt_BR; MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"
Name: pt_PT; MessagesFile: "compiler:Languages\Portuguese.isl"
Name: rus; MessagesFile: "compiler:Languages\Russian.isl"
Name: slo; MessagesFile: "compiler:Languages\Slovak.isl"
Name: slv; MessagesFile: "compiler:Languages\Slovenian.isl"
Name: spa; MessagesFile: "compiler:Languages\Spanish.isl"
Name: sr_at_Cyrillic; MessagesFile: "compiler:Languages\SerbianCyrillic.isl"
Name: sr_at_Latin;    MessagesFile: "compiler:Languages\SerbianLatin.isl"
Name: swe; MessagesFile: "compiler:Default.isl,lang\isl\Swedish-10-5.1.12.isl"
Name: tur; MessagesFile: "compiler:Default.isl,lang\isl\Turkish-3-5.1.11.isl"
Name: "zh_at_Simplified";  MessagesFile: "compiler:Default.isl,lang\isl\ChineseSimp-12-5.1.11.isl"
Name: "zh_at_Traditional"; MessagesFile: "compiler:Default.isl,lang\isl\ChineseTrad-2-5.1.11.isl"

[Components]
Name: "prog";          Description: "Program files:"; Types: full compact
Name: "prog\filter32"; Description: "AC3Filter (32bit)"; Types: full compact
Name: "prog\acm32";    Description: "AC3Filter ACM driver (32bit)"; Types: full compact

Name: "lang"; Description: "Language files"; Types: full

[Files]
Source: "filter\Release\ac3filter.ax";             DestDir: "{app}"; Components: prog\filter32; Flags: 32bit Regserver RestartReplace UninsRestartDelete IgnoreVersion
Source: "filter\Release\avcodec-53.dll";           DestDir: "{app}"; Components: prog\filter32; Flags: 32bit RestartReplace UninsRestartDelete IgnoreVersion
Source: "filter\Release\avutil-51.dll";            DestDir: "{app}"; Components: prog\filter32; Flags: 32bit RestartReplace UninsRestartDelete IgnoreVersion
Source: "BugTrap\Release\BugTrap.dll";             DestDir: "{app}"; Components: prog\filter32 or prog\acm32; Flags: 32bit RestartReplace UninsRestartDelete IgnoreVersion
Source: "intl\Release\ac3filter_intl.dll";         DestDir: "{app}"; Components: prog\filter32 and lang; Flags: 32bit RestartReplace UninsRestartDelete IgnoreVersion
Source: "tools\ac3config\Release\ac3config.exe";   DestDir: "{app}"; Components: prog\filter32; Flags: 32bit RestartReplace UninsRestartDelete IgnoreVersion
Source: "tools\spdif_test\Release\spdif_test.exe"; DestDir: "{app}"; Components: prog\filter32; Flags: 32bit RestartReplace UninsRestartDelete IgnoreVersion

Source: "acm\Release_Libc\ac3filter.acm";       DestDir: "{sys}"; Components: prog\acm32; Flags: 32bit RestartReplace UninsRestartDelete IgnoreVersion

Source: "lang\lang\*"; DestDir: "{app}\lang"; Components: lang; Flags: recursesubdirs

Source: "reg\*.reg";   DestDir: "{app}"
Source: "Readme.txt";  DestDir: "{app}"
Source: "Changes.txt"; DestDir: "{app}"
Source: "GPL.txt";     DestDir: "{app}"

[Icons]
Name: "{group}\Home"; Filename: "http://ac3filter.net"
Name: "{group}\Documentation"; Filename: "http://ac3filter.net/wiki/AC3Filter_Help"
Name: "{group}\AC3Filter Config"; Filename: "{app}\ac3config.exe"; Components: prog\filter32
Name: "{group}\SPDIF test (32bit)"; Filename: "{app}\spdif_test.exe"; Components: prog\filter32

Name: "{group}\Readme"; Filename: "{app}\Readme.txt"
Name: "{group}\License"; Filename: "{app}\GPL.txt"
Name: "{group}\Reset to defaults"; Filename: "{app}\Reset to defaults.reg"
Name: "{group}\Restore default presets"; Filename: "{app}\Presets.reg"
Name: "{group}\Clear filter cache"; Filename: "{app}\Clear filter cache.reg"
Name: "{group}\Uninstall AC3Filter"; Filename: "{uninstallexe}"

[Registry]

; AC3Filter registry parameters
Root: HKCU; Subkey: "Software\AC3Filter"; Flags: uninsdeletekey
Root: HKCU; Subkey: "Software\AC3Filter"; ValueType: string; ValueName: "Install_Dir"; ValueData: "{app}"
Root: HKCU; Subkey: "Software\AC3Filter"; ValueType: string; ValueName: "Lang_Dir"; ValueData: "{app}\Lang"; Components: lang;
Root: HKCU; Subkey: "Software\AC3Filter"; ValueType: dword;  ValueName: "tray"; ValueData: 1

; Language
Root: HKCU; Subkey: "Software\AC3Filter"; ValueType: string; ValueName: "Language"; ValueData: {code:AC3FilterLang}

; Version
Root: HKCU; Subkey: "Software\AC3Filter"; ValueType: string; ValueName: "Version"; ValueData: "{#appver}"

; Clear DirectShow filter cache on install and ununstall
Root: HKCU; Subkey: "Software\Microsoft\Multimedia\ActiveMovie\Filter Cache"; Flags: deletekey uninsdeletekey noerror

; ACM codec registration
Root: HKLM32; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueType: string; ValueName: "msacm.ac3filter"; ValueData: "ac3filter.acm"; Components: prog\acm32; Flags: uninsdeletevalue
Root: HKLM32; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers.desc"; ValueType: string; ValueName: "ac3filter.acm"; ValueData: "AC3Filter ACM codec"; Components: prog\acm32; Flags: uninsdeletevalue

[Run]
Filename: "regedit"; Parameters: "/s ""{app}\Presets.reg""";
Filename: "regedit"; Parameters: "/s ""{app}\Reset to defaults.reg""";
Filename: "http://store.kagi.com/cgi-bin/store.cgi?storeID=6CZJZ_LIVE&view=cart&product/969409773073/0/quantity=10"; Flags: nowait postinstall shellexec skipifsilent; Description: "Donate"
Filename: "{app}\ac3config.exe"; Flags: nowait postinstall skipifsilent; Description: "Configure AC3Filter"

[code]
function AC3FilterLang(param: string): string;
begin
  result := ActiveLanguage();
  StringChangeEx(result, '_at_', '@', false);
end;
