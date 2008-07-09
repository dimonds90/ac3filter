;
; AC3Filter installation script
;

OutFile "${SETUP_FILE}"
Name "AC3Filter"
CRCCheck on
SetCompressor lzma


Icon "ac3filter.ico"
UninstallIcon "ac3filter.ico"

InstallDir $PROGRAMFILES\AC3Filter
InstallDirRegKey HKLM "Software\AC3Filter" "Install_Dir"

InstallColors {000000 C0C0C0}
InstProgressFlags "smooth"
ShowInstDetails "show"

Page directory
Page components
Page instfiles
UninstPage uninstConfirm
UninstPage instfiles



Section "-Install"

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;; Installer stuff
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  ;; Remember where we're installed
  WriteRegStr HKCU SOFTWARE\AC3Filter "Install_Dir" "$INSTDIR"

  ;; Make an uninstaller
  SetOutPath $INSTDIR
  WriteUninstaller "uninstall.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\AC3Filter" "DisplayName" "AC3Filter (remove only)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\AC3Filter" "UninstallString" "$INSTDIR\uninstall.exe"

  ;; Delete shit from old versions
  DeleteRegValue HKLM "Software\Microsoft\Windows\CurrentVersion\Control Panel\Cpls" "AC3Filter"
  Delete "$SYSDIR\ac3filter.ax"
  Delete "$SYSDIR\ac3filter.cpl"



  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;; Common stuff
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  ;; Languages repository
  WriteRegStr HKCU SOFTWARE\AC3Filter "Lang_Dir" "$INSTDIR\Lang"

  ;; Base Files
  SetOutPath $INSTDIR
  File "${SOURCE_DIR}\*.*"

  ;; Create Start Menu shortcuts
  CreateDirectory "$SMPROGRAMS\AC3Filter"
  CreateShortCut  "$SMPROGRAMS\AC3Filter\AC3Filter home.lnk" "http://ac3filter.net"

SectionEnd



SectionGroup "Documentation"
Section "English"
  SetOutPath $INSTDIR\doc
  File "${SOURCE_DIR}\doc\*eng.*"
  CreateDirectory "$SMPROGRAMS\AC3Filter\English docs"
  CreateShortCut  "$SMPROGRAMS\AC3Filter\English docs\AC3Filter User's Manual.lnk"    "$INSTDIR\doc\ac3filter_eng.pdf"
  CreateShortCut  "$SMPROGRAMS\AC3Filter\English docs\AC3Filter & SPDIF.lnk"          "$INSTDIR\doc\spdif_eng.pdf"
  CreateShortCut  "$SMPROGRAMS\AC3Filter\English docs\Loudness and dynamic range.lnk" "$INSTDIR\doc\loudness_eng.pdf"
SectionEnd
Section "Russian"
  SetOutPath $INSTDIR\doc
  File "${SOURCE_DIR}\doc\*rus.*"
  CreateDirectory "$SMPROGRAMS\AC3Filter\Russian docs"
  CreateShortCut  "$SMPROGRAMS\AC3Filter\Russian docs\AC3Filter Руководство пользователя.lnk" "$INSTDIR\doc\ac3filter_rus.pdf"
  CreateShortCut  "$SMPROGRAMS\AC3Filter\Russian docs\AC3Filter & SPDIF.lnk"                  "$INSTDIR\doc\spdif_rus.pdf"
  CreateShortCut  "$SMPROGRAMS\AC3Filter\Russian docs\Громкость и динамический диапазон.lnk"  "$INSTDIR\doc\loudness_rus.pdf"
SectionEnd
SectionGroupEnd



SectionGroup /e "Program"
Section "AC3Filter DirectShow"

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;; Install DirectShow filter
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  ;; Register filter
  UnRegDLL "$INSTDIR\ac3filter.ax"
  RegDll   "$INSTDIR\ac3filter.ax"

  ;; Install presets
  ExecWait 'regedit /s "$INSTDIR\ac3filter_reg_presets.reg"'
  ExecWait 'regedit /s "$INSTDIR\ac3filter_reg_reset.reg"'

  ;; Import registry keys for DirectSound/WaveOut

  ;; Start Menu shortcuts
  CreateShortCut  "$SMPROGRAMS\AC3Filter\AC3Filter Config.lnk"     "$INSTDIR\ac3config.exe"
  CreateShortCut  "$SMPROGRAMS\AC3Filter\SPDIF test.lnk"           "$INSTDIR\spdif_test.exe"
  CreateShortCut  "$SMPROGRAMS\AC3Filter\Reset to Defaults.lnk"    "$INSTDIR\ac3filter_reg_reset.reg"

SectionEnd



Section "AC3Filter ACM"

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;; Install ACM codec
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  ;; Copy binary to sysdir
  SetOutPath $INSTDIR
  CopyFiles /SILENT "$INSTDIR\ac3filter.acm" "$SYSDIR\ac3filter.acm"

  ;; Register ACM codec
  ReadEnvStr $1 "OS"
  StrCmp $1 "Windows_NT" RegNT Reg9x

RegNT:
  DetailPrint "Registering for Windows NT/2k/XP"
  WriteRegStr HKLM "Software\Microsoft\Windows NT\CurrentVersion\Drivers32" "msacm.ac3filter" "ac3filter.acm"
  WriteRegStr HKLM "Software\Microsoft\Windows NT\CurrentVersion\Drivers.desc" "ac3filter.acm" "AC3Filter AC3/DTS codec"
  goto Finish

Reg9x:
  DetailPrint "Registering for Windows 9x"
  WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\MediaResources\msacm\msacm.ac3filter" "Description"  "AC3Filter ACM codec"
  WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\MediaResources\msacm\msacm.ac3filter" "Driver"       "ac3filter.acm"
  WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\MediaResources\msacm\msacm.ac3filter" "FriendlyName" "AC3Filter ACM codec"
  goto Finish

Finish:

SectionEnd
SectionGroupEnd



Section "Translations"
  SetOutPath $INSTDIR\lang
  File /r "${SOURCE_DIR}\lang\*.*"
SectionEnd



Section "-Last links"
  CreateShortCut  "$SMPROGRAMS\AC3Filter\Change Log (English).lnk" "$INSTDIR\_changes_eng.txt"
  CreateShortCut  "$SMPROGRAMS\AC3Filter\Change Log (Russian).lnk" "$INSTDIR\_changes_rus.txt"
  CreateShortCut  "$SMPROGRAMS\AC3Filter\Uninstall.lnk"            "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
SectionEnd



Section "Uninstall"

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;; Installer stuff
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  DeleteRegKey   HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\AC3Filter"
  DeleteRegKey   HKCU "Software\AC3Filter"

  ;; Delete shit from old versions
  DeleteRegValue HKLM "Software\Microsoft\Windows\CurrentVersion\Control Panel\Cpls" "AC3Filter"
  Delete "$SYSDIR\ac3filter.ax"
  Delete "$SYSDIR\ac3filter.cpl"



  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;; Uninstall DirectShow filter
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  UnRegDLL "$INSTDIR\ac3filter.ax"



  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;; Uninstall ACM codec
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  ;; Delete binary from sysdir (may require rebooting)
  Delete /REBOOTOK "$SYSDIR\ac3filter.acm"

  ;; Unregister for Win9x
  DeleteRegKey   HKLM "SYSTEM\CurrentControlSet\Control\MediaResources\msacm\msacm.ac3filter"

  ;; Unregister for WinNT 
  DeleteRegValue HKLM "Software\Microsoft\Windows NT\CurrentVersion\Drivers32"    "msacm.ac3filter"
  DeleteRegValue HKLM "Software\Microsoft\Windows NT\CurrentVersion\Drivers.desc" "ac3filter.acm"



  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;; Uninstall common stuff
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  Delete "$SMPROGRAMS\AC3Filter\English docs\*.*"
  Delete "$SMPROGRAMS\AC3Filter\Russian docs\*.*"
  Delete "$SMPROGRAMS\AC3Filter\*.*"

  RMDir  "$SMPROGRAMS\AC3Filter\English docs"
  RMDir  "$SMPROGRAMS\AC3Filter\Russian docs"
  RMDir  "$SMPROGRAMS\AC3Filter"

  RMDir /r "$INSTDIR\lang"
  RMDir /r "$INSTDIR\doc"

  Delete "$INSTDIR\*.*"
  RMDir  "$INSTDIR"


SectionEnd
