@echo off
echo ------------------------
echo Register %1 version
copy /b intl\bin\iconv.dll %1\
if "%1" == "debug" copy /b %windir%\system32\msvcrtd.dll %1\
if "%1" == "DEBUG" copy /b %windir%\system32\msvcrtd.dll %1\
regsvr32 /s /u %1\ac3filter.ax
regsvr32 /s %1\ac3filter.ax
