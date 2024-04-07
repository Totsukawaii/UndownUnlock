@echo off
setlocal enabledelayedexpansion
cd /d "%~dp0"

SET MinicondaInstaller=Miniconda3-latest-Windows-x86_64.exe
SET InstallDir=%CD%\Miniconda

:: Check if Miniconda is already installed
if exist "%InstallDir%" (
    echo Miniconda is already installed.
) else (
    :: Check if curl or wget is available
    where curl >nul 2>nul
    IF !ERRORLEVEL! EQU 0 (
        SET DownloadCmd=curl -L -o !MinicondaInstaller!
    ) ELSE (
        where wget >nul 2>nul
        IF !ERRORLEVEL! EQU 0 (
            SET DownloadCmd=wget -O !MinicondaInstaller!
        ) ELSE (
            echo Neither curl nor wget is available. Please download Miniconda manually.
            exit /b
        )
    )

    :: Download Miniconda Installer
    echo Downloading Miniconda...
    echo !DownloadCmd!
    !DownloadCmd! https://repo.anaconda.com/miniconda/!MinicondaInstaller!

    :: Install Miniconda
    echo Installing Miniconda to !InstallDir!...
    start /wait "" !MinicondaInstaller! /InstallationType=JustMe /RegisterPython=0 /S /D=!InstallDir!

    echo Miniconda setup complete.
)


SET PATH=%InstallDir%;%InstallDir%\Scripts;%InstallDir%\Library\bin;%PATH%

call activate.bat lockdown

if %errorlevel% == 1 (
  echo Environment 'lockdown' not found. Creating...
  conda create -n lockdown python=3.10 -y
  call activate.bat lockdown
  call pip install -r requirements.txt
)


:: Run Injector
title Inject
call activate.bat lockdown32

if %errorlevel% == 1 (
  echo Environment 'lockdown32' not found. Creating...
  set CONDA_FORCE_32BIT=1
  conda create -n lockdown32 python=3.7 -y
  call activate.bat lockdown32
  call pip install -r requirements32.txt
)

echo Running Injector...
python inject.py

:: Run Controller
title Controller
call activate.bat lockdown

if %errorlevel% == 1 (
  echo Environment 'lockdown' not found. Creating...
  conda create -n lockdown python=3.10 -y
  call activate.bat lockdown
  call pip install -r requirements.txt
)

echo Running Controller...
python controller.py

pause
