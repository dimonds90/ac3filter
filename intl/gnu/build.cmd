@echo off
set USR=%CD%\usr

rmdir /s /q %USR%
rmdir /s /q libiconv-1.9.2-mod
rmdir /s /q gettext-0.13.1-mod

gzip -dc libiconv-1.9.2-mod.tar.gz | tar -xv
gzip -dc gettext-0.13.1-mod.tar.gz | tar -xv


cd libiconv-1.9.2-mod
nmake -f Makefile.msvc NO_NLS=1 MFLAGS=-MT PREFIX=%USR%
nmake -f Makefile.msvc NO_NLS=1 MFLAGS=-MT PREFIX=%USR% install
cd ..

cd gettext-0.13.1-mod
nmake -f Makefile.msvc MFLAGS=-MT PREFIX=%USR%
nmake -f Makefile.msvc MFLAGS=-MT PREFIX=%USR% install
cd ..

mkdir include
mkdir lib
xcopy /s /i /y %USR%\include include\%1
xcopy /s /i /y %USR%\lib lib\%1

rmdir /s /q libiconv-1.9.2-mod
rmdir /s /q gettext-0.13.1-mod
rmdir /s /q %USR%
