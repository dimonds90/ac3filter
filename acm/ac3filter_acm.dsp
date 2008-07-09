# Microsoft Developer Studio Project File - Name="ac3filter_acm" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ac3filter_acm - Win32 Debug Libc
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ac3filter_acm.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ac3filter_acm.mak" CFG="ac3filter_acm - Win32 Debug Libc"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ac3filter_acm - Win32 Release Libc" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ac3filter_acm - Win32 Debug Libc" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ac3filter_acm - Win32 Release Libc"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ac3filter_acm___Win32_Release_Libc"
# PROP BASE Intermediate_Dir "ac3filter_acm___Win32_Release_Libc"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_Libc"
# PROP Intermediate_Dir "Release_Libc"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /Ob2 /I "..\valib\valib" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AC3FILTER_ACM_EXPORTS" /FR /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /Ob2 /I "..\..\valib\valib" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AC3FILTER_ACM_EXPORTS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x417 /d "NDEBUG"
# ADD RSC /l 0x417 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 msvcrt.lib oldnames.lib winmm.lib kernel32.lib /nologo /dll /machine:I386 /nodefaultlib /out:"Release/ac3filter.acm"
# SUBTRACT BASE LINK32 /map
# ADD LINK32 libc.lib oldnames.lib winmm.lib kernel32.lib /nologo /dll /machine:I386 /nodefaultlib /out:"Release_Libc/ac3filter.acm"
# SUBTRACT LINK32 /map
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=_copy_release.bat
# End Special Build Tool

!ELSEIF  "$(CFG)" == "ac3filter_acm - Win32 Debug Libc"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ac3filter_acm___Win32_Debug_Libc"
# PROP BASE Intermediate_Dir "ac3filter_acm___Win32_Debug_Libc"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_Libc"
# PROP Intermediate_Dir "Debug_Libc"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\valib\valib" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AC3FILTER_ACM_EXPORTS" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\..\valib\valib" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AC3FILTER_ACM_EXPORTS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x417 /d "_DEBUG"
# ADD RSC /l 0x417 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libc.lib oldnames.lib winmm.lib kernel32.lib /nologo /dll /incremental:no /debug /debugtype:both /machine:I386 /nodefaultlib /out:"Debug/ac3filter.acm" /pdbtype:sept
# ADD LINK32 libc.lib oldnames.lib winmm.lib kernel32.lib user32.lib /nologo /dll /incremental:no /debug /debugtype:both /machine:I386 /nodefaultlib /out:"Debug_Libc/ac3filter.acm" /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=_copy_debug.bat
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "ac3filter_acm - Win32 Release Libc"
# Name "ac3filter_acm - Win32 Debug Libc"
# Begin Source File

SOURCE=.\ac3filter_acm.cpp
# End Source File
# Begin Source File

SOURCE=.\ac3filter_acm.def
# End Source File
# Begin Source File

SOURCE=.\dbglog.cpp
# End Source File
# Begin Source File

SOURCE=.\dbglog.h
# End Source File
# Begin Source File

SOURCE=.\decoder.cpp
# End Source File
# Begin Source File

SOURCE=.\decoder.h
# End Source File
# Begin Source File

SOURCE=.\msacmdrv.h
# End Source File
# End Target
# End Project
