set LANG=%1\Lang

rmdir /q /s %LANG% 2>nul
mkdir %LANG%

for %%f in (*.po) do (mkdir %LANG%\%%~nf\LC_MESSAGES && bin\msgfmt %%~nf.po -o %LANG%\%%~nf\LC_MESSAGES\ac3filter.mo)
copy *.po %LANG%
copy ac3filter.pot %LANG%
