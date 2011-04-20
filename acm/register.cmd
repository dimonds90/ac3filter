@echo off

set dir=%~dp0
set platform=Win32
set config=Release
set reg_key=HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Drivers32

if /i "%1"=="Win32"   set platform=Win32& shift
if /i "%1"=="x64"     set platform=x64& shift
if /i "%1"=="Debug"   set config=Debug& shift
if /i "%1"=="Release" set config=Release& shift

if "%platform%" == "Win32" set file=%config%\ac3filter.acm
if "%platform%" == "x64" set file=%platform%\%config%\ac3filter64.acm

if not exist "%file%" goto err_file

if "%platform%" == "x64" if "%PROCESSOR_ARCHITECTURE%" == "x86" goto err_register_x64
if "%platform%" == "Win32" if "%PROCESSOR_ARCHITECTURE%" == "AMD64" set reg_key=HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Microsoft\Windows NT\CurrentVersion\Drivers32

echo REGEDIT4> register.reg
echo [%reg_key%]>>register.reg
echo "msacm.ac3filter"="%dir:\=\\%%file:\=\\%">>register.reg
regedit /s register.reg

if errorlevel 1 goto err
echo %file% registered successfully...
goto end

:err_register_x64
echo Cannot register x64 module on 32bit platform
goto fail

:err_file
echo %file% not found
goto fail

:fail
error 2>nul
:end
del register.reg 2> nul
