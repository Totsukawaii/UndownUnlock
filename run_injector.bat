@echo off
cd /d "%~dp0"

title Inject

SET InstallDir=%CD%\Miniconda

SET PATH=%InstallDir%;%InstallDir%\Scripts;%InstallDir%\Library\bin;%PATH%

call activate.bat lockdown32

if %errorlevel% == 1 (
  echo Environment 'lockdown32' not found. Creating...
  set CONDA_FORCE_32BIT=1
  conda create -n lockdown32 python=3.7 -y
  call activate.bat lockdown32
  call pip install -r requirements32.txt
)

echo Running...
python inject.py

pause
