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
  WriteRegStr HKCU SOFTWARE\AC3Filter "Install_Dir" "$INSTDIR"

  ;; Make an uninstaller
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\AC3Filter" "DisplayName" "AC3Filter (remove only)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\AC3Filter" "UninstallString" "$INSTDIR\uninstall.exe"
  WriteUninstaller "uninstall.exe"

  ;; Copy Files
  File /r "${SOURCE_DIR}\*.*"
  	
  ;; Install presets
  ExecWait 'regedit /s "$INSTDIR\ac3filter_reg_presets.reg"'
  ExecWait 'regedit /s "$INSTDIR\ac3filter_reg_reset.reg"'

  ;; Import registry keys for DirectSound/WaveOut

  ;; Register filter
  UnRegDLL "$INSTDIR\ac3filter.ax"
  RegDll   "$INSTDIR\ac3filter.ax"

  ;; Create Start Menu shortcuts
  CreateDirectory "$SMPROGRAMS\AC3Filter"

  CreateShortCut  "$SMPROGRAMS\AC3Filter\AC3Filter Config.lnk"     "$INSTDIR\ac3config.exe"
  CreateShortCut  "$SMPROGRAMS\AC3Filter\Reset to Defaults.lnk"    "$INSTDIR\ac3filter_reg_reset.reg"
  CreateShortCut  "$SMPROGRAMS\AC3Filter\Online Documentation.lnk" "http://ac3filter.net"

  CreateDirectory "$SMPROGRAMS\AC3Filter\English docs"
  CreateShortCut  "$SMPROGRAMS\AC3Filter\English docs\AC3Filter User's Manual.lnk"    "$INSTDIR\doc\ac3filter_eng.pdf"
  CreateShortCut  "$SMPROGRAMS\AC3Filter\English docs\AC3Filter & SPDIF.lnk"          "$INSTDIR\doc\spdif_eng.pdf"
  CreateShortCut  "$SMPROGRAMS\AC3Filter\English docs\Loudness and dynamic range.lnk" "$INSTDIR\doc\loudness_eng.pdf"

  CreateDirectory "$SMPROGRAMS\AC3Filter\Russian docs"
  CreateShortCut  "$SMPROGRAMS\AC3Filter\Russian docs\AC3Filter Руководство пользователя.lnk" "$INSTDIR\doc\ac3filter_rus.pdf"
  CreateShortCut  "$SMPROGRAMS\AC3Filter\Russian docs\AC3Filter & SPDIF.lnk"                  "$INSTDIR\doc\spdif_rus.pdf"
  CreateShortCut  "$SMPROGRAMS\AC3Filter\Russian docs\Громкость и динамический диапазон.lnk"  "$INSTDIR\doc\loudness_rus.pdf"

  CreateShortCut  "$SMPROGRAMS\AC3Filter\Change Log (English).lnk" "$INSTDIR\_changes_eng.txt"
  CreateShortCut  "$SMPROGRAMS\AC3Filter\Change Log (Russian).lnk" "$INSTDIR\_changes_rus.txt"
  CreateShortCut  "$SMPROGRAMS\AC3Filter\Uninstall.lnk"            "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0

  ;; Delete shit from old versions
  DeleteRegValue HKLM "Software\Microsoft\Windows\CurrentVersion\Control Panel\Cpls" "AC3Filter"
  Delete "$SYSDIR\ac3filter.ax"
  Delete "$SYSDIR\ac3filter.cpl"
SectionEnd

Section "Uninstall"
  DeleteRegKey   HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\AC3Filter"
  DeleteRegKey   HKCU "Software\AC3Filter"

  UnRegDLL "$INSTDIR\ac3filter.ax"

  Delete "$SMPROGRAMS\AC3Filter\English docs\*.*"
  Delete "$SMPROGRAMS\AC3Filter\Russian docs\*.*"
  Delete "$SMPROGRAMS\AC3Filter\*.*"

  RMDir  "$SMPROGRAMS\AC3Filter\English docs"
  RMDir  "$SMPROGRAMS\AC3Filter\Russian docs"
  RMDir  "$SMPROGRAMS\AC3Filter"

  RMDir /r "$INSTDIR\lang"
  RMDir /r "$INSTDIR\doc"
  RMDir  "$INSTDIR"

  ;; Delete shit from old versions
  DeleteRegValue HKLM "Software\Microsoft\Windows\CurrentVersion\Control Panel\Cpls" "AC3Filter"
  Delete "$SYSDIR\ac3filter.ax"
  Delete "$SYSDIR\ac3filter.cpl"
SectionEnd
