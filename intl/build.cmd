@echo off

set compiler=vc9
set platform=Win32
set config=Release

if /i "%1"=="vc6"     set compiler=vc6& shift
if /i "%1"=="vc9"     set compiler=vc9& shift

if /i "%1"=="Win32"   set platform=Win32& shift
if /i "%1"=="x64"     set platform=x64& shift

if /i "%1"=="Debug"   set config=Debug& shift
if /i "%1"=="Release" set config=Release& shift
if not "%1"=="" goto usage

goto %compiler%

rem -------------------------------------------------------
rem Visual Studio 97

:vc6
for %%f in (*.dsw) do set workspace=%%~nf
if "%workspace%"=="" goto err_workspace

set msdev=%MSDevDir%\bin\msdev.exe
if not exist "%msdev%" goto err_msdev
%msdev% %workspace%.dsw /make "%workspace% - Win32 %config%"
goto end

rem -------------------------------------------------------
rem Visual Studio 2008

:vc9
for %%f in (*.sln) do set solution=%%f
if "%solution%"=="" goto err_solution

call vcvars32 2>nul
call vcvarsall 2>nul
vcbuild /? >nul 2>nul
if errorlevel 1 goto :err_vcbuild

vcbuild %solution% "%config%|%platform%"
goto end

rem -------------------------------------------------------
rem Error messages and usage

:err_solution
echo Error: no solution (*.sln) file found.
error 2>nul
goto end

:err_workspace
echo Error: no workspace (*.dsw) file found.
error 2>nul
goto end

:err_msdev
echo Error: msdev.exe not found.
error 2>nul
goto end

:err_vcbuild
echo Error: vcbuild.exe not found.
error 2>nul
goto end

:usage
echo %~n0 [compiler] [platform] [configuration]
echo where
echo   compiler: vc6 (Visual Studio 97) or vc9 (VisualStudio 2008)
echo   paltform: Win32 or x64
echo   configuration: Debug or Release
error 2>nul

:end
