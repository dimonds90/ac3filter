@call ..\cmd\clean_vc.cmd %*
rmdir /q /s Debug_Libc 2>nul
rmdir /q /s Release_Libc 2>nul
