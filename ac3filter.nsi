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
  CopyFiles /SILENT "$INSTDIR\ac3filter.ax" "$SYSDIR\ac3filter.cpl"

  ;; Install presets

  ;; 'standard' preset
  WriteRegDWORD HKCU "SOFTWARE\AC3Filter\preset\standard" "dynrng"           1
  WriteRegDWORD HKCU "SOFTWARE\AC3Filter\preset\standard" "auto_gain"        1
  WriteRegDWORD HKCU "SOFTWARE\AC3Filter\preset\standard" "normalize"        0
  WriteRegDWORD HKCU "SOFTWARE\AC3Filter\preset\standard" "auto_matrix"      1
  WriteRegDWORD HKCU "SOFTWARE\AC3Filter\preset\standard" "normalize_matrix" 1
  WriteRegDWORD HKCU "SOFTWARE\AC3Filter\preset\standard" "expand_stereo"    1
  WriteRegDWORD HKCU "SOFTWARE\AC3Filter\preset\standard" "voice_control"    1
  WriteRegDWORD HKCU "SOFTWARE\AC3Filter\preset\standard" "clev_lock"        1
  WriteRegDWORD HKCU "SOFTWARE\AC3Filter\preset\standard" "slev_lock"        1
  WriteRegDWORD HKCU "SOFTWARE\AC3Filter\preset\standard" "lfelev_lock"      1
  WriteRegDWORD HKCU "SOFTWARE\AC3Filter\preset\standard" "bass_redir"       1
  
  WriteRegStr   HKCU "SOFTWARE\AC3Filter\preset\standard" "clev"             "1.000000"
  WriteRegStr   HKCU "SOFTWARE\AC3Filter\preset\standard" "slev"             "1.000000"
  WriteRegStr   HKCU "SOFTWARE\AC3Filter\preset\standard" "lfelev"           "1.000000"
  WriteRegStr   HKCU "SOFTWARE\AC3Filter\preset\standard" "master"           "1.000000"
  WriteRegStr   HKCU "SOFTWARE\AC3Filter\preset\standard" "dynrng_power"     "1.000000"
  
  ;; 'LFE boost' perset
  WriteRegDWORD HKCU "SOFTWARE\AC3Filter\preset\LFE boost" "dynrng"           1
  WriteRegDWORD HKCU "SOFTWARE\AC3Filter\preset\LFE boost" "auto_gain"        1
  WriteRegDWORD HKCU "SOFTWARE\AC3Filter\preset\LFE boost" "normalize"        0
  WriteRegDWORD HKCU "SOFTWARE\AC3Filter\preset\LFE boost" "auto_matrix"      1
  WriteRegDWORD HKCU "SOFTWARE\AC3Filter\preset\LFE boost" "normalize_matrix" 0
  WriteRegDWORD HKCU "SOFTWARE\AC3Filter\preset\LFE boost" "expand_stereo"    1
  WriteRegDWORD HKCU "SOFTWARE\AC3Filter\preset\LFE boost" "voice_control"    1
  WriteRegDWORD HKCU "SOFTWARE\AC3Filter\preset\LFE boost" "clev_lock"        1
  WriteRegDWORD HKCU "SOFTWARE\AC3Filter\preset\LFE boost" "slev_lock"        1
  WriteRegDWORD HKCU "SOFTWARE\AC3Filter\preset\LFE boost" "lfelev_lock"      0
  WriteRegDWORD HKCU "SOFTWARE\AC3Filter\preset\LFE boost" "bass_redir"       1
                                                                            
  WriteRegStr   HKCU "SOFTWARE\AC3Filter\preset\LFE boost" "clev"             "1.000000"
  WriteRegStr   HKCU "SOFTWARE\AC3Filter\preset\LFE boost" "slev"             "1.000000"
  WriteRegStr   HKCU "SOFTWARE\AC3Filter\preset\LFE boost" "lfelev"           "3.162278"
  WriteRegStr   HKCU "SOFTWARE\AC3Filter\preset\LFE boost" "master"           "0.707946"
  WriteRegStr   HKCU "SOFTWARE\AC3Filter\preset\LFE boost" "dynrng_power"     "1.000000"

  ;; 'loud' preset
  WriteRegDWORD HKCU "SOFTWARE\AC3Filter\preset\loud" "dynrng"           1
  WriteRegDWORD HKCU "SOFTWARE\AC3Filter\preset\loud" "auto_gain"        1
  WriteRegDWORD HKCU "SOFTWARE\AC3Filter\preset\loud" "normalize"        1
  WriteRegDWORD HKCU "SOFTWARE\AC3Filter\preset\loud" "auto_matrix"      1
  WriteRegDWORD HKCU "SOFTWARE\AC3Filter\preset\loud" "normalize_matrix" 0
  WriteRegDWORD HKCU "SOFTWARE\AC3Filter\preset\loud" "expand_stereo"    1
  WriteRegDWORD HKCU "SOFTWARE\AC3Filter\preset\loud" "voice_control"    1
  WriteRegDWORD HKCU "SOFTWARE\AC3Filter\preset\loud" "clev_lock"        1
  WriteRegDWORD HKCU "SOFTWARE\AC3Filter\preset\loud" "slev_lock"        1
  WriteRegDWORD HKCU "SOFTWARE\AC3Filter\preset\loud" "lfelev_lock"      1
  WriteRegDWORD HKCU "SOFTWARE\AC3Filter\preset\loud" "bass_redir"       1
                                                                  
  WriteRegStr   HKCU "SOFTWARE\AC3Filter\preset\loud" "clev"             "1.000000"
  WriteRegStr   HKCU "SOFTWARE\AC3Filter\preset\loud" "slev"             "1.000000"
  WriteRegStr   HKCU "SOFTWARE\AC3Filter\preset\loud" "lfelev"           "1.000000"
  WriteRegStr   HKCU "SOFTWARE\AC3Filter\preset\loud" "master"           "10.000000"
  WriteRegStr   HKCU "SOFTWARE\AC3Filter\preset\loud" "dynrng_power"     "1.000000"

  ;; Delete shit from old versions
  DeleteRegValue HKLM "Software\Microsoft\Windows\CurrentVersion\Control Panel\Cpls" "AC3Filter"

  ;; Register filter
  UnRegDLL "$INSTDIR\ac3filter.ax"
  RegDll   "$INSTDIR\ac3filter.ax"

  ;; Setup filter
  ;; RegDll "$INSTDIR\ac3filter.ax" "config"

SectionEnd

Section "Start menu shortcuts"
  ;; Create Start Menu shortcuts
  CreateDirectory "$SMPROGRAMS\AC3Filter"
  CreateShortCut  "$SMPROGRAMS\AC3Filter\AC3Filter Config.lnk"         "$SYSDIR\ac3filter.cpl" "" "$INSTDIR\ac3filter.ax"
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
  Delete "$SYSDIR\ac3filter.cpl"

  RMDir  "$INSTDIR\pic"
  RMDir  "$INSTDIR"
SectionEnd
