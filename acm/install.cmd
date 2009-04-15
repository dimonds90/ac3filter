@echo off
set platform=Win32
set sysdir=%windir%\system32
if /i "%1" == "Win32" set platform=Win32&shift
if /i "%1" == "x64" set platform=x64&shift
set file=%1
shift
if not "%1" == "" goto usage

if "%platform%" == "x64" if "%PROCESSOR_ARCHITECTURE%" == "x86" goto err_install_64
if "%platform%" == "Win32" if "%PROCESSOR_ARCHITECTURE%" == "AMD64" set sysdir=%windir%\syswow64

copy /b /y %file% %sysdir%
if errorlevel 1 goto err
echo %file% installed successfully...
goto end

:err
echo %f installation failed
goto fail

:err_install_64
echo Cannot install x64 module on 32bit platform
goto end

:usage
echo Usage: %~n0 [platform] file
echo   platform: Win32 (default) or x64
echo   file: file to be copied to the windows system folder
goto fail

:fail
error 2>nul
:end
