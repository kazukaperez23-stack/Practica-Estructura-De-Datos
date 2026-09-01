@echo off
cd /d "%~dp0"
".\sort_practice.exe" dataset.txt > run_output.txt 2>&1
echo EXITCODE=%ERRORLEVEL%
