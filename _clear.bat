@echo off
del *.aps 2> nul
del *.ncb 2> nul
del *.plg 2> nul
del *.dep 2> nul
del *.opt 2> nul
rmdir /s /q debug 2> nul
rmdir /s /q perf 2> nul
del release\*.bsc 2> nul
del release\*.exp 2> nul
del release\*.idb 2> nul
del release\*.lib 2> nul
del release\*.obj 2> nul
del release\*.pch 2> nul
del release\*.pdb 2> nul
del release\*.res 2> nul
del release\*.sbr 2> nul
del release\*.ilk 2> nul
del release\*.asm 2> nul
del release\*.map 2> nul

cd chineese_patch
call _clear.bat
cd ..

cd ac3config
call _clear.bat
cd ..

cd spdif_test
call _clear.bat
cd ..

cd ACM
call _clear.bat
cd ..
