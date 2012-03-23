@echo off

rem -------------------------------------------------------
rem List files to be translated
rem -------------------------------------------------------

dir /s /b ..\filter\*.cpp ..\filter\*.h >files.txt

rem -------------------------------------------------------
rem Choose:
rem   1) Build a new translation template
rem   2) Update an existing template
rem   3) Do not modify the template
rem -------------------------------------------------------

if not exist "ac3filter.pot" goto build_new_pot
bin\xgettext -f files.txt --no-location --language=c++ -d ac3filter -o ac3filter_new.pot -k_ -kN_ --add-comments=TRANSLATORS:
fc ac3filter.pot ac3filter_new.pot | perl "-e @diff = <>; exit(-1) if ($#diff > 10);"
if errorlevel 0 goto update_translations

echo Translation template changed
move ac3filter_new.pot ac3filter.pot
goto update_translations

:build_new_pot
echo Building a new translation template
bin\xgettext -f files.txt --no-location --language=c++ -d ac3filter -o ac3filter.pot -k_ -kN_ --add-comments=TRANSLATORS:

rem -------------------------------------------------------
rem Update translations
rem -------------------------------------------------------

:update_translations
echo Updating translation files
for %%f in (*.po) do bin\msgmerge --update --no-fuzzy-matching %%f ac3filter.pot

rem -------------------------------------------------------
rem Cleanup
rem -------------------------------------------------------

del ac3filter_new.pot >nul 2>nul
del files.txt >nul 2>nul
call clean.cmd
