;
; AC3Filter installation script
;


OutFile "${SETUP_FILE}"
Name "AC3Filter"
Icon "ac3filter.ico"
CRCCheck on

InstallDir "$PROGRAMFILES\AC3Filter"
DirShow show
DirText "This will install AC3Filter ver ${VERSION} to your computer:"

InstallColors {000000 C0C0C0}
InstProgressFlags "smooth"
ShowInstDetails "show"

UninstallText 'This will uninstall AC3Filter ver ${VERSION}. Hit "Uninstall" to continue.'
ShowUnInstDetails "show"


Section "Install"
  SetOutPath $INSTDIR

  ;; Make an uninstaller
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\AC3Filter" "DisplayName" "AC3Filter (remove only)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\AC3Filter" "UninstallString" "$INSTDIR\uninstall.exe"
  WriteUninstaller "uninstall.exe"

  ;; Copy Files
  File /r "${SOURCE_DIR}\*.*"

  ;; Install presets

  ;; Import registry keys for DirectSound/WaveOut

  ;; Register filter
  UnRegDLL "$INSTDIR\ac3filter.ax"
  RegDll   "$INSTDIR\ac3filter.ax"

  ;; Delete shit from old versions
  DeleteRegValue HKLM "Software\Microsoft\Windows\CurrentVersion\Control Panel\Cpls" "AC3Filter"
  Delete "$SYSDIR\ac3filter.cpl"
SectionEnd

Section "Start menu shortcuts"
  ;; Create Start Menu shortcuts
  CreateDirectory "$SMPROGRAMS\AC3Filter"
  CreateShortCut  "$SMPROGRAMS\AC3Filter\AC3Filter Config.lnk"         "$INSTDIR\ac3config.exe"
  CreateShortCut  "$SMPROGRAMS\AC3Filter\AC3Filter Help (English).lnk" "$INSTDIR\AC3Filter_eng.html"
  CreateShortCut  "$SMPROGRAMS\AC3Filter\AC3Filter Help (Russian).lnk" "$INSTDIR\AC3Filter_rus.html"
  CreateShortCut  "$SMPROGRAMS\AC3Filter\AC3Filter Help (Italian).lnk" "$INSTDIR\AC3Filter_ita.html"
  CreateShortCut  "$SMPROGRAMS\AC3Filter\uninstall.lnk"                "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
SectionEnd

Section "Uninstall"
  DeleteRegKey   HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\AC3Filter"
  DeleteRegKey   HKCU "Software\AC3Filter"

  UnRegDLL "$INSTDIR\ac3filter.ax"

  Delete "$SMPROGRAMS\AC3Filter\*.*"
  RMDir  "$SMPROGRAMS\AC3Filter"

  Delete "$INSTDIR\pic\*.*"
  Delete "$INSTDIR\*.*"

  RMDir  "$INSTDIR\pic"
  RMDir  "$INSTDIR"

  ;; Delete shit from old versions
  DeleteRegValue HKLM "Software\Microsoft\Windows\CurrentVersion\Control Panel\Cpls" "AC3Filter"
  Delete "$SYSDIR\ac3filter.ax"
  Delete "$SYSDIR\ac3filter.cpl"
SectionEnd
