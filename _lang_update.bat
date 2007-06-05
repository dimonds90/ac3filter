@echo off
intl\bin\xgettext -f ac3filter_intl.files -d ac3filter -o ac3filter.pot -p lang -k_ -kN_ --add-comments=TRANSLATORS:
for %%f in (lang\*.po) do intl\bin\msgmerge --update %%f lang\ac3filter.pot
for %%f in (lang\*.po) do intl\bin\msgfmt lang\%%~nf.po -o lang\%%~nf.mo
