#ifndef appver
#define appver GetStringFileInfo("filter\Release\ac3filter.ax", "ProductVersion")
#endif
#define filever StringChange(appver, '.', '_')

#ifdef OPENCANDY
#include "OCKeys.iss"
#endif

[Setup]
AppID=AC3Filter
AppVersion={#appver}
AppName=AC3Filter
AppVerName=AC3Filter {#appver}
AppPublisher=Alexander Vigovsky
AppPublisherURL=http://ac3filter.net
AppCopyright=Copyright (c) 2002-2013 by Alexander Vigovsky
OutputBaseFilename=ac3filter_{#filever}
DefaultDirName={pf}\AC3Filter
DefaultGroupName=AC3Filter
AllowNoIcons=yes
SolidCompression=yes
LanguageDetectionMethod=locale
MinVersion=0, 5.0
PrivilegesRequired=admin

#ifdef OPENCANDY
LicenseFile=OCLicense.txt
#else
LicenseFile=GPL.txt
#endif

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
Name: ukr; MessagesFile: "compiler:Languages\Ukrainian.isl"
Name: "zh_at_Simplified";  MessagesFile: "compiler:Default.isl,lang\isl\ChineseSimp-12-5.1.11.isl"
Name: "zh_at_Traditional"; MessagesFile: "compiler:Default.isl,lang\isl\ChineseTrad-2-5.1.11.isl"

[Components]
Name: "prog";          Description: "Program files:"; Types: full compact
Name: "prog\filter32"; Description: "AC3Filter (32bit)"; Types: full compact
Name: "prog\filter64"; Description: "AC3Filter (64bit)"; Check: IsWin64; Types: full compact
Name: "prog\acm32";    Description: "AC3Filter ACM driver (32bit)"; Types: full compact
Name: "prog\acm64";    Description: "AC3Filter ACM driver (64bit)"; Check: IsWin64; Types: full compact
Name: "debug";         Description: "Debugging files"; Types: full
Name: "lang";          Description: "Language files"; Types: full

[Files]
#ifdef OPENCANDY
  Source: "{#OC_OCSETUPHLP_FILE_PATH}"; Flags: dontcopy ignoreversion;
#endif

Source: "filter\Release\ac3filter.ax";             DestDir: "{app}"; Components: prog\filter32; Flags: 32bit Regserver RestartReplace UninsRestartDelete IgnoreVersion
Source: "BugTrap\Release\BugTrap.dll";             DestDir: "{app}"; Components: prog\filter32 or prog\acm32; Flags: 32bit RestartReplace UninsRestartDelete IgnoreVersion
Source: "intl\Release\ac3filter_intl.dll";         DestDir: "{app}"; Components: prog\filter32 and lang; Flags: 32bit RestartReplace UninsRestartDelete IgnoreVersion
Source: "tools\ac3config\Release\ac3config.exe";   DestDir: "{app}"; Components: prog\filter32; Flags: 32bit RestartReplace UninsRestartDelete IgnoreVersion
Source: "tools\spdif_test\Release\spdif_test.exe"; DestDir: "{app}"; Components: prog\filter32; Flags: 32bit RestartReplace UninsRestartDelete IgnoreVersion

Source: "filter\x64\Release\ac3filter64.ax";           DestDir: "{app}\x64"; Components: prog\filter64; Flags: 64bit Regserver RestartReplace UninsRestartDelete IgnoreVersion
Source: "BugTrap\x64\Release\BugTrap-x64.dll";         DestDir: "{app}\x64"; Components: prog\filter64 or prog\acm64; Flags: 64bit RestartReplace UninsRestartDelete IgnoreVersion
Source: "intl\x64\Release\ac3filter64_intl.dll";       DestDir: "{app}\x64"; Components: prog\filter64 and lang; Flags: 64bit RestartReplace UninsRestartDelete IgnoreVersion
Source: "tools\ac3config\x64\Release\ac3config.exe";   DestDir: "{app}\x64"; Components: prog\filter64; Flags: 64bit RestartReplace UninsRestartDelete IgnoreVersion
Source: "tools\spdif_test\x64\Release\spdif_test.exe"; DestDir: "{app}\x64"; Components: prog\filter64; Flags: 64bit RestartReplace UninsRestartDelete IgnoreVersion

Source: "acm\Release\ac3filter.acm";       DestDir: "{sys}"; Components: prog\acm32; Flags: 32bit RestartReplace UninsRestartDelete IgnoreVersion
Source: "acm\x64\Release\ac3filter64.acm"; DestDir: "{sys}"; Components: prog\acm64; Flags: 64bit RestartReplace UninsRestartDelete IgnoreVersion

Source: "DbgHelp\dbghelp.dll";     DestDir: "{app}";     Components: debug and prog\filter32 or prog\acm32; Flags: 32bit RestartReplace UninsRestartDelete IgnoreVersion
Source: "DbgHelp\x64\dbghelp.dll"; DestDir: "{app}\x64"; Components: debug and prog\filter64 or prog\acm64; Flags: 64bit RestartReplace UninsRestartDelete IgnoreVersion

Source: "lang\lang\*"; DestDir: "{app}\lang"; Components: lang; Flags: recursesubdirs

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
Name: "{group}\AC3Filter Config"; Filename: "{app}\x64\ac3config.exe"; Components: prog\filter64 and not prog\filter32
Name: "{group}\SPDIF test (32bit)"; Filename: "{app}\spdif_test.exe"; Components: prog\filter32
Name: "{group}\SPDIF test (64bit)"; Filename: "{app}\x64\spdif_test.exe"; Components: prog\filter64

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
Root: HKCU; Subkey: "Software\AC3Filter"; ValueType: dword;  ValueName: "refresh_time"; ValueData: 50

; Language
Root: HKCU; Subkey: "Software\AC3Filter"; ValueType: string; ValueName: "Language"; ValueData: {code:AC3FilterLang}

; Version
Root: HKCU; Subkey: "Software\AC3Filter"; ValueType: string; ValueName: "Version"; ValueData: "{#appver}"

; Clear DirectShow filter cache on install and ununstall
Root: HKCU; Subkey: "Software\Microsoft\Multimedia\ActiveMovie\Filter Cache"; Flags: deletekey uninsdeletekey noerror

; ACM codec registration
Root: HKLM32; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueType: string; ValueName: "msacm.ac3filter"; ValueData: "ac3filter.acm"; Components: prog\acm32; Flags: uninsdeletevalue
Root: HKLM32; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers.desc"; ValueType: string; ValueName: "ac3filter.acm"; ValueData: "AC3Filter ACM codec"; Components: prog\acm32; Flags: uninsdeletevalue
Root: HKLM64; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"; ValueType: string; ValueName: "msacm.ac3filter"; ValueData: "ac3filter64.acm"; Components: prog\acm64; Flags: uninsdeletevalue
Root: HKLM64; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Drivers.desc"; ValueType: string; ValueName: "ac3filter.acm"; ValueData: "AC3Filter ACM codec"; Components: prog\acm64; Flags: uninsdeletevalue

[Run]
Filename: "regedit"; Parameters: "/s ""{app}\Presets.reg"""; Flags: waituntilterminated
Filename: "regedit"; Parameters: "/s ""{app}\Reset to defaults.reg"""; Flags: waituntilterminated
Filename: "http://ac3filter.net/donate"; Flags: nowait postinstall shellexec skipifsilent; Description: "Donate"

[Code]

#ifdef OPENCANDY
  #include "OCSetupHlp.iss"
#endif

var
  SendReportIndex : integer;
  PrevVersion : string;
  
function AC3FilterLang(param: string): string;
begin
  result := ActiveLanguage();
  StringChangeEx(result, '_at_', '@', false);
end;

function OpenCandyLang: string;
begin
  case ActiveLanguage() of
    'baq': result := 'eu';
    'bul': result := 'bg';
    'eng': result := 'en';
    'cze': result := 'cs';
    'dan': result := 'da';
    'dut': result := 'nl';
    'fin': result := 'fi';
    'fre': result := 'fr';
    'ger': result := 'de';
    'gre': result := 'el';
    'heb': result := 'he';
    'hun': result := 'hu';
    'ind': result := 'id';
    'ita': result := 'it';
    'jpn': result := 'ja';
    'kor': result := 'ko';
    'pol': result := 'pl';
    'pt_BR': result := 'pt';
    'pt_PT': result := 'pt';
    'rus': result := 'ru';
    'slo': result := 'sk';
    'slv': result := 'sl';
    'spa': result := 'es';
    'sr_at_Cyrillic': result := 'sr';
    'sr_at_Latin': result := 'sr';
    'swe': result := 'sv';
    'tur': result := 'tr';
    'ukr': result := 'uk';
    'zh_at_Simplified': result := 'zh';
    'zh_at_Traditional': result := 'zh';
  else
    result := 'en';
  end;
end;

procedure SendReport(reportType: string);
var
  WinHttpReq: Variant;
begin try
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
end; end;

////////////////////////////////////////////////////////////

procedure InitializeWizard();
#ifdef OPENCANDY
var
  OCtszInstallerLanguage: OCTString;
  iOpenCandyNewPageID: Integer;
#endif

begin
  SendReportIndex := -1;
  PrevVersion := '';
  RegQueryStringValue(HKEY_CURRENT_USER, 'Software\AC3Filter', 'Version', PrevVersion);

  #ifdef OPENCANDY
    OCtszInstallerLanguage := OpenCandyLang();

    // Initialize OpenCandy, check for offers
    OpenCandyInit('{#OC_STR_MY_PRODUCT_NAME}', '{#OC_STR_KEY}', '{#OC_STR_SECRET}', OCtszInstallerLanguage, {#OC_INIT_MODE_NORMAL});	
    // Extract and load the OpenCandy Network Client library after the user accepts the EULA
    iOpenCandyNewPageID := OpenCandyInsertLoadDLLPage(wpLicense);
    // After loading the client library, connect to the OpenCandy Network to check for recommendations
    iOpenCandyNewPageID := OpenCandyInsertConnectPage(iOpenCandyNewPageID);
    // Show a loading screen after the Select Tasks page to allow more time for server connection.
    iOpenCandyNewPageID := OpenCandyInsertLoadingPage(wpSelectTasks, ' ', ' ', 'Loading...', 'Arial', 100);
    // After the loading screen insert the OpenCandy offer page
    iOpenCandyNewPageID := OpenCandyInsertOfferPage(iOpenCandyNewPageID);		
  #endif
end;

procedure CurPageChanged(CurPageID: Integer);
begin
  if CurPageID=wpFinished then
    SendReportIndex := WizardForm.RunList.AddCheckbox('Send installation report', '', 0, true, true, false, false, nil);

  #ifdef OPENCANDY
    OpenCandyCurPageChanged(CurPageID);
  #endif
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

  #ifdef OPENCANDY
    OpenCandyCurStepChanged(CurStep);
  #endif
end;

function ShouldSkipPage(PageID: Integer): Boolean;
begin
  Result := false; // Don't skip pages by default

  #ifdef OPENCANDY
    if OpenCandyShouldSkipPage(PageID) then
      Result := true;
  #endif
end;

function NextButtonClick(CurPageID: Integer): Boolean;
begin
  Result := true; // Allow action by default

  #ifdef OPENCANDY
    if not OpenCandyNextButtonClick(CurPageID) then
      Result := false;
  #endif
end;

function BackButtonClick(CurPageID: Integer): Boolean;
begin
  Result := true; // Allow action by default

  #ifdef OPENCANDY
    OpenCandyBackButtonClick(CurPageID);
  #endif
end;

procedure DeinitializeSetup();
begin
  #ifdef OPENCANDY
    OpenCandyDeinitializeSetup();
  #endif
end;
