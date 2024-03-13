@echo off
cd /d "%~dp0"

title Controller

SET InstallDir=%CD%\Miniconda

SET PATH=%InstallDir%;%InstallDir%\Scripts;%InstallDir%\Library\bin;%PATH%

call activate.bat lockdown

if %errorlevel% == 1 (
  echo Environment 'lockdown' not found. Creating...
  conda create -n lockdown python=3.10 -y
  call activate.bat lockdown
  call pip install -r requirements.txt
)

echo Running...
python controller.py

pause
