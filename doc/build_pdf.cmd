@echo off
if "%1" == "" goto build_all
if not exist "%1" goto err_no_file
if exist "%~dpn1.pdf" goto warn_built

if "%1" == "common_packages.tex" goto end
if "%1" == "common_style.tex" goto end
if "%1" == "common_rus.tex" goto end

echo Building file %1
call clean_temp.cmd
pdflatex %1
pdflatex %1
pdflatex %1
call clean_temp.cmd
if not exist %~dpn1.pdf goto err_pdf
goto end

:build_all
echo Building all .tex files
for %%f in (*.tex) do (
  call build_pdf.cmd %%f
  if errorlevel 1 goto end)
goto end

:err_pdf
echo %1 : error: cannot build PDF
goto fail

:err_no_file
echo %1 : error: file does not exist
goto fail

:warn_built
echo %1 is already built
goto end

:fail
error 2>nul
:end
