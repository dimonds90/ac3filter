@echo off
echo ------------------------
echo Register %1 version
regsvr32 /s /u %1\ac3filter.ax
regsvr32 /s %1\ac3filter.ax
