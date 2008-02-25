@echo off
if not exist "lang\ac3filter.pot" goto build_new_pot
intl\bin\xgettext -f ac3filter_intl.files --no-location --language=c++ -d ac3filter -o ac3filter_new.pot -p lang -k_ -kN_ --add-comments=TRANSLATORS:
fc Lang\ac3filter.pot Lang\ac3filter_new.pot | perl "-e @diff = <>; exit(-1) if ($#diff > 10);"
if errorlevel 0 goto update_translations

echo Translation template changed
move Lang\ac3filter_new.pot Lang\ac3filter.pot
goto update_translations

:build_new_pot
echo Building translation template
intl\bin\xgettext -f ac3filter_intl.files --no-location --language=c++ -d ac3filter -o ac3filter.pot -p lang -k_ -kN_ --add-comments=TRANSLATORS:

:update_translations
echo Updating translation files
for %%f in (lang\*.po) do intl\bin\msgmerge --update --no-fuzzy-matching %%f lang\ac3filter.pot
for %%f in (lang\*.po) do intl\bin\msgfmt lang\%%~nf.po -o lang\%%~nf.mo

del lang\ac3filter_new.pot >nul 2>nul
