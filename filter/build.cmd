@echo off

for %%f in (*.sln) do set solution=%%f
if "%solution%"=="" goto err_solution

set platform=Win32
set config=Release

if /i "%1"=="Win32" set platform=Win32&& shift
if /i "%1"=="x64" set platform=x64&& shift
if /i "%1"=="Debug" set config=Debug&& shift
if /i "%1"=="Release" set config=Release&& shift
if not "%1"=="" goto usage

call vcvarsall 2>nul
vcbuild /? >nul 2>nul
if errorlevel 1 goto :err_vcbuild

:build
vcbuild %solution% "%config%|%platform%"
goto end

:err_solution
echo Error: no solution file found
set errorlevel=1
goto end

:err_vcbuild
echo Error: vcbuild not found
set errorlevel=1
goto end

:usage
echo %~n0 [platform] [configuration]
echo where
echo   paltform: Win32 or x64
echo   configuration: Debug or Release
set errorlevel=1

:end
