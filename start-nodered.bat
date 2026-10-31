@echo off
rem Starts Node-RED from the project folder.
set TOOLS=%USERPROFILE%\tools
set PATH=%TOOLS%\node;%TOOLS%\sqlite;%PATH%
cd /d %~dp0
node %TOOLS%\nodered\node_modules\node-red\red.js --userDir %TOOLS%\nodered-data
pause
