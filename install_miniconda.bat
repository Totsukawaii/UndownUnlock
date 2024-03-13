@echo off
SET MinicondaInstaller=Miniconda3-latest-Windows-x86_64.exe
SET InstallDir=%CD%\Miniconda

:: Check if curl or wget is available
where curl >nul 2>nul
IF %ERRORLEVEL% EQU 0 (
    SET DownloadCmd=curl -L -o %MinicondaInstaller%
) ELSE (
    where wget >nul 2>nul
    IF %ERRORLEVEL% EQU 0 (
        SET DownloadCmd=wget -O %MinicondaInstaller%
    ) ELSE (
        echo Neither curl nor wget is available. Please download Miniconda manually.
        exit /b
    )
)

:: Download Miniconda Installer
echo Downloading Miniconda...
%DownloadCmd% https://repo.anaconda.com/miniconda/%MinicondaInstaller%

:: Install Miniconda
echo Installing Miniconda to %InstallDir%...
start /wait "" %MinicondaInstaller% /InstallationType=JustMe /RegisterPython=0 /S /D=%InstallDir%

:: Set up Miniconda path temporarily for the current script
SET PATH=%InstallDir%;%InstallDir%\Scripts;%InstallDir%\Library\bin;%PATH%

:: Create new virtual environment with Python 3.10
echo Creating a new virtual environment with Python 3.10...
conda create -n lockdown python=3.10 -y
call "%InstallDir%\Scripts\activate.bat" lockdown
pip install -r requirements.txt

:: Create a new virtual environment with Python 3.7
echo Creating a new virtual environment with Python 3.7...
Set CONDA_FORCE_32BIT=1
conda create -n lockdown32 python=3.7 -y
call "%InstallDir%\Scripts\activate.bat" lockdown32
pip install -r requirements32.txt


echo Setup complete.

pause