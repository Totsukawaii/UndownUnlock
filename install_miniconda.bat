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

echo Setup complete.

pause