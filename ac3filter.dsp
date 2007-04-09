# Microsoft Developer Studio Project File - Name="ac3filter" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ac3filter - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ac3filter.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ac3filter.mak" CFG="ac3filter - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ac3filter - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ac3filter - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "ac3filter"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ac3filter - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AC3FILTER_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /O2 /Ob2 /I "..\valib\valib" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AC3FILTER_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x417 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 strmbase.lib msvcrt.lib oldnames.lib quartz.lib winmm.lib comctl32.lib kernel32.lib shell32.lib gdi32.lib user32.lib advapi32.lib uuid.lib dsound.lib ole32.lib oleaut32.lib /nologo /dll /pdb:none /machine:I386 /nodefaultlib /out:"Release/ac3filter.ax"
# SUBTRACT LINK32 /map
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=_reg.bat release
# End Special Build Tool

!ELSEIF  "$(CFG)" == "ac3filter - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AC3FILTER_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\valib\valib" /D "WIN32" /D "DEBUG" /D "_DEBUG" /D DBG=1 /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AC3FILTER_EXPORTS" /Fr /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 strmbasd.lib msvcrtd.lib msvcprtd.lib oldnames.lib quartz.lib winmm.lib comctl32.lib kernel32.lib gdi32.lib shell32.lib user32.lib advapi32.lib uuid.lib dsound.lib ole32.lib oleaut32.lib /nologo /dll /debug /machine:I386 /nodefaultlib /out:"Debug/ac3filter.ax" /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=_reg.bat debug
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "ac3filter - Win32 Release"
# Name "ac3filter - Win32 Debug"
# Begin Group "ac3filter"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ac3config.cpp
# End Source File
# Begin Source File

SOURCE=.\ac3filter.cpp
# End Source File
# Begin Source File

SOURCE=.\ac3filter.def
# End Source File
# Begin Source File

SOURCE=.\ac3filter.h
# End Source File
# Begin Source File

SOURCE=.\ac3filter_dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ac3filter_dlg.h
# End Source File
# Begin Source File

SOURCE=.\ac3filter_ver.h
# End Source File
# Begin Source File

SOURCE=.\com_dec.cpp
# End Source File
# Begin Source File

SOURCE=.\com_dec.h
# End Source File
# Begin Source File

SOURCE=.\controls.cpp
# End Source File
# Begin Source File

SOURCE=.\controls.h
# End Source File
# Begin Source File

SOURCE=.\guids.cpp
# End Source File
# Begin Source File

SOURCE=.\guids.h
# End Source File
# Begin Source File

SOURCE=.\register.cpp
# End Source File
# Begin Source File

SOURCE=.\registry.cpp
# End Source File
# Begin Source File

SOURCE=.\registry.h
# End Source File
# Begin Source File

SOURCE=.\resource_ids.h
# End Source File
# Begin Source File

SOURCE=.\rot.cpp
# End Source File
# Begin Source File

SOURCE=.\rot.h
# End Source File
# Begin Source File

SOURCE=.\translate.cpp
# End Source File
# Begin Source File

SOURCE=.\translate.h
# End Source File
# Begin Source File

SOURCE=.\tray.cpp
# End Source File
# Begin Source File

SOURCE=.\tray.h
# End Source File
# End Group
# Begin Group "decss"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\decss\CSSauth.cpp
# End Source File
# Begin Source File

SOURCE=.\decss\CSSauth.h
# End Source File
# Begin Source File

SOURCE=.\decss\CSSscramble.cpp
# End Source File
# Begin Source File

SOURCE=.\decss\CSSscramble.h
# End Source File
# Begin Source File

SOURCE=.\decss\DeCSSInputPin.cpp
# End Source File
# Begin Source File

SOURCE=.\decss\DeCSSInputPin.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ac3filter.bmp
# End Source File
# Begin Source File

SOURCE=.\ac3filter.ico
# End Source File
# Begin Source File

SOURCE=.\ac3filter.rc
# End Source File
# End Target
# End Project
