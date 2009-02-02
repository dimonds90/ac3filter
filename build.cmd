@echo off
for /d %%d in (*) do if exist %%d\build.cmd (
cd %%d
call build.cmd %*
if errorlevel 1 goto err
cd ..)
goto end

:err
echo Build error!!!
:end
