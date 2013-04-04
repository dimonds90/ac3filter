#ifndef appver
#define appver GetFileVersion("filter\Release\ac3filter.ax")
#endif
#define filever StringChange(appver, '.', '_')

[Setup]
AppID=AC3Filter
AppVersion={#appver}
AppName=AC3Filter Lite
AppVerName=AC3Filter {#appver}
AppPublisher=Alexander Vigovsky
AppPublisherURL=http://ac3filter.net
AppCopyright=Copyright (c) 2002-2013 by Alexander Vigovsky
OutputBaseFilename=ac3filter_{#filever}_lite
DefaultDirName={pf}\AC3Filter
DefaultGroupName=AC3Filter
AllowNoIcons=yes
SolidCompression=yes
LanguageDetectionMethod=locale
MinVersion=0, 5.0
LicenseFile=GPL.txt

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
Name: swe; MessagesFile: "compiler:Default.isl,lang\isl\Swedish-10-5.1.12.isl"
Name: tur; MessagesFile: "compiler:Default.isl,lang\isl\Turkish-3-5.1.11.isl"
Name: "zh_at_Simplified";  MessagesFile: "compiler:Default.isl,lang\isl\ChineseSimp-12-5.1.11.isl"
Name: "zh_at_Traditional"; MessagesFile: "compiler:Default.isl,lang\isl\ChineseTrad-2-5.1.11.isl"

[Components]
Name: "prog";          Description: "Program files:"; Types: full compact
Name: "prog\filter32"; Description: "AC3Filter (32bit)"; Types: full compact

[Files]
Source: "filter\Release\ac3filter.ax";             DestDir: "{app}"; Components: prog\filter32; Flags: 32bit Regserver RestartReplace UninsRestartDelete IgnoreVersion
Source: "BugTrap\Release\BugTrap.dll";             DestDir: "{app}"; Components: prog\filter32; Flags: 32bit RestartReplace UninsRestartDelete IgnoreVersion
Source: "tools\ac3config\Release\ac3config.exe";   DestDir: "{app}"; Components: prog\filter32; Flags: 32bit RestartReplace UninsRestartDelete IgnoreVersion
Source: "tools\spdif_test\Release\spdif_test.exe"; DestDir: "{app}"; Components: prog\filter32; Flags: 32bit RestartReplace UninsRestartDelete IgnoreVersion

Source: "reg\*.reg";   DestDir: "{app}"
Source: "Readme.txt";  DestDir: "{app}"
Source: "Changes.txt"; DestDir: "{app}"
Source: "GPL.txt";     DestDir: "{app}"

[InstallDelete]
; Delete map files installed by v2.2a
Type: files; Name: "{app}\ac3filter.map"
Type: files; Name: "{app}\BugTrap.map"
Type: files; Name: "{app}\ac3filter64.map"
Type: files; Name: "{app}\BugTrap-x64.map"
; Delete dlls installed by v2.0a - v2.5b
Type: files; Name: "{app}\avcodec-53.dll"
Type: files; Name: "{app}\avutil-51.dll"
Type: files; Name: "{app}\x64\avcodec64-53.dll"
Type: files; Name: "{app}\x64\avutil64-51.dll"

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
Root: HKCU; Subkey: "Software\AC3Filter"; ValueType: dword;  ValueName: "tray"; ValueData: 1
Root: HKCU; Subkey: "Software\AC3Filter"; ValueType: dword;  ValueName: "refresh_time"; ValueData: 50

; Language
Root: HKCU; Subkey: "Software\AC3Filter"; ValueType: string; ValueName: "Language"; ValueData: {code:AC3FilterLang}

; Version
Root: HKCU; Subkey: "Software\AC3Filter"; ValueType: string; ValueName: "Version"; ValueData: "{#appver}"

; Clear DirectShow filter cache on install and ununstall
Root: HKCU; Subkey: "Software\Microsoft\Multimedia\ActiveMovie\Filter Cache"; Flags: deletekey uninsdeletekey noerror

[Run]
Filename: "regedit"; Parameters: "/s ""{app}\Presets.reg""";
Filename: "regedit"; Parameters: "/s ""{app}\Reset to defaults.reg""";
Filename: "http://ac3filter.net/donate"; Flags: nowait postinstall shellexec skipifsilent; Description: "Donate"

[code]
var
  SendReportIndex : integer;
  PrevVersion : string;
  
function AC3FilterLang(param: string): string;
begin
  result := ActiveLanguage();
  StringChangeEx(result, '_at_', '@', false);
end;

procedure SendReport(reportType: string);
var
  WinHttpReq: Variant;
begin
try
  WinHttpReq := CreateOleObject('WinHttp.WinHttpRequest.5.1');
  WinHttpReq.Open('POST', 'http://ac3filter.net/install.php', false);
  WinHttpReq.SetRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
  WinHttpReq.Send(
    'reportType=' + reportType + '&' +
    'appName=AC3Filter&' +
    ExpandConstant('appVersion={#appver}&') +
    'prevVersion=' + PrevVersion + '&' +
    'language=' + ActiveLanguage + '&' +
    'windowsVersion=' + GetWindowsVersionString + '&' +
    'CPU=' + GetEnv('PROCESSOR_IDENTIFIER')
  );
except
end;
end;

////////////////////////////////////////////////////////////

function InitializeSetup(): Boolean;
begin
  SendReportIndex := -1;
  PrevVersion := '';
  RegQueryStringValue(HKEY_CURRENT_USER, 'Software\AC3Filter', 'Version', PrevVersion);
  Result := true;
end;

procedure CurPageChanged(CurPageID: Integer);
begin
  if CurPageID=wpFinished then
    SendReportIndex := WizardForm.RunList.AddCheckbox('Send installation report', '', 0, true, true, false, false, nil);
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if (CurStep = ssDone) and
     (SendReportIndex <> -1) and
     (WizardForm.RunList.Checked[SendReportIndex]) then
  begin
    if PrevVersion = '' then
      SendReport('install')
    else
      SendReport('upgrade');
  end;
end;
