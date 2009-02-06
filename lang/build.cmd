@echo off
set LANG=lang

rmdir /q /s "%LANG%" 2>nul
mkdir "%LANG%"

for %%f in (*.po) do (
  mkdir "%LANG%\%%~nf\LC_MESSAGES"
  bin\msgfmt "%%f" -o "%LANG%\%%~nf\LC_MESSAGES\ac3filter.mo")

copy "*.po" "%LANG%"
copy "ac3filter.pot" "%LANG%"
