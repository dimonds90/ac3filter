@call ..\cmd\clean_vc.cmd %*
rmdir /s /q stat-debug 2> nul
rmdir /s /q stat-release 2> nul
rmdir /s /q stat-debug-x64 2> nul
rmdir /s /q stat-release-x64 2> nul
rmdir /s /q bin
