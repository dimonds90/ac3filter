@echo off
echo ------------------------
echo Register performance-measure version
regsvr32 /s /u perf\ac3filter.ax
regsvr32 /s perf\ac3filter.ax
