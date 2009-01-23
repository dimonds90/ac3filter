@echo off
if "%1" == "" goto usage
if "%1" == "all" goto build_all
if not exist "%1" goto no_file
if "%1" == "common_packages.tex" goto inc_file
if "%1" == "common_style.tex" goto inc_file
if "%1" == "common_rus.tex" goto inc_file
goto build_file

:build_file
echo Building file %1
call clean_temp.cmd
htlatex %1 "common"
call clean_temp.cmd
goto end

:build_all
echo Building all files
call clean_temp.cmd
for %%f in (*.tex) do call build_html.cmd %%f 
call clean_temp.cmd
goto end

:no_file
echo File %1 does not exists
goto end

:inc_file
echo File %1 is include file (do not build)
goto end

:usage
echo Usage
echo   build_html file.tex - build html for the given tex file
echo   build_html all      - build html for all tex files
goto end

:end
