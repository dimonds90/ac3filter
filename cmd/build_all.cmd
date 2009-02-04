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

set OLD_DIR=%CD%
set DIRS=%*
if "%DIRS%"=="" set DIRS=*

for /d %%d in (%DIRS%) do if exist %%d\build.cmd (
  cd %OLD_DIR%\%%d
  echo Building %%d...
  call build.cmd %compiler% %platform% %config%
  if errorlevel 1 goto err
)
goto end

:err
echo Build error!!!
:end
cd %OLD_DIR%
