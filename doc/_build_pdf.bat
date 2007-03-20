@echo off
if "%1" == "" goto usage
if "%1" == "all" goto build_all
if not exist "%1" goto no_file
if "%1" == "common_packages.tex" goto inc_file
if "%1" == "common_style.tex" goto inc_file
goto build_file

:build_file
echo Building file %1
call _clear.bat
pdflatex %1
pdflatex %1
pdflatex %1
call _clear.bat
goto end

:build_all
echo Building all files
call _clear_output.bat
for %%f in (*.tex) do call _build %%f 
call _clear.bat
goto end

:no_file
echo File %1 does not exists
goto end

:inc_file
echo File %1 is include file (do not build)
goto end

:usage
echo Usage
echo   _build_pdf file.tex - build pdf for the given tex file
echo   _build_pdf all      - build pdf for all tex files
goto end

:end
