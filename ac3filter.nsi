;
; AC3Filter installation script
;

OutFile "${SETUP_FILE}"
Name "AC3Filter"
CRCCheck on

Icon "ac3filter.ico"
UninstallIcon "ac3filter.ico"

InstallDir $PROGRAMFILES\AC3Filter
InstallDirRegKey HKLM "Software\AC3Filter" "Install_Dir"

InstallColors {000000 C0C0C0}
InstProgressFlags "smooth"
ShowInstDetails "show"


Page directory
Page instfiles
UninstPage uninstConfirm
UninstPage instfiles


Section "AC3Filter"
  SetOutPath $INSTDIR

  ;; Remember where we're installed
  WriteRegStr HKLM SOFTWARE\AC3Filter "Install_Dir" "$INSTDIR"

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

  ;; Create Start Menu shortcuts
  CreateDirectory "$SMPROGRAMS\AC3Filter"
  CreateShortCut  "$SMPROGRAMS\AC3Filter\AC3Filter Config.lnk"         "$INSTDIR\ac3config.exe"
  CreateShortCut  "$SMPROGRAMS\AC3Filter\AC3Filter Help (English).lnk" "$INSTDIR\AC3Filter_eng.html"
  CreateShortCut  "$SMPROGRAMS\AC3Filter\AC3Filter Help (Russian).lnk" "$INSTDIR\AC3Filter_rus.html"
  CreateShortCut  "$SMPROGRAMS\AC3Filter\AC3Filter Help (Italian).lnk" "$INSTDIR\AC3Filter_ita.html"
  CreateShortCut  "$SMPROGRAMS\AC3Filter\uninstall.lnk"                "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0

  ;; Delete shit from old versions
  DeleteRegValue HKLM "Software\Microsoft\Windows\CurrentVersion\Control Panel\Cpls" "AC3Filter"
  Delete "$SYSDIR\ac3filter.ax"
  Delete "$SYSDIR\ac3filter.cpl"
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
