rem @echo off
echo ------------------------
echo Register %1
set filter=%1\ac3filter.ax
if not exist "%filter%" set filter=%1\ac3filter64.ax

regsvr32 /s /u "%filter%"
regsvr32 /s "%filter%"
