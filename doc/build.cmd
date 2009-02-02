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
pdflatex %1
pdflatex %1
pdflatex %1
call clean_temp.cmd
htlatex %1 "common"
call clean_temp.cmd
goto end

:build_all
echo Building all files
call clean_temp.cmd
for %%f in (*.tex) do call build.cmd %%f 
call clean_temp.cmd
goto end

:no_file
echo File %1 does not exists
set errorlevel=1
goto end

:inc_file
echo File %1 is include file (do not build)
set errorlevel=1
goto end

:usage
echo Usage
echo   build file.tex - build pdf and html for the given tex file
echo   build all      - build pdf and html for all tex files
set errorlevel=1
goto end

:end
echo errorlevel=%errorlevel%