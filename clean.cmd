@echo off
for /d %%d in (*) do if exist %%d\clean.cmd (cd %%d && (call clean.cmd & cd ..))
