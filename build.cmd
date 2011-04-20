@echo off

rem -------------------------------------------------------
echo Building projects

set PROJECTS=acm filter intl tools\ac3config tools\spdif_test
call cmd\build_all %* %PROJECTS%
if errorlevel 1 goto fail

rem -------------------------------------------------------
echo Building translations...

cd lang
call update.cmd
if errorlevel 1 goto fail

call build.cmd
if errorlevel 1 goto fail
cd ..

echo All OK!
goto end

:fail
echo Build failed!
error 2>nul
:end
