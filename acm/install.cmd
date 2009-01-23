@echo off
set DIR=%1
if "%OS%" == "Windows_NT" goto WinNT

:Win9x
copy /b %DIR%\ac3filter.acm %windir%\system
goto exit

:WinNT
copy /b %DIR%\ac3filter.acm %windir%\system32
goto exit

:exit
