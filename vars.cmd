set inno_setup=C:\Devel\Inno Setup 5\iscc.exe
set make_src=pkzip25 -add -rec -dir -excl=CVS -excl=Debug -excl=Release -excl=x64 -excl=*.ncb -lev=9 %src_arc% %src_files%
