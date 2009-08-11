@echo off

call clean_temp.cmd

del *.pdf  2> nul
del *.htm  2> nul
del *.html 2> nul
del *.css  2> nul

del /a:h *.suo 2>nul
del *.user 2>nul
del *.ncb  2>nul
rmdir /s /q PDF 2> nul
