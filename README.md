# UndownUnlock
Respondus LockDown Browser cheats hacks (No VM required)

## Features

 - :white_check_mark: Alt tab with `ctrl + left` and `ctrl + right`, focus and unfocus with `ctrl + down` and `ctrl + up`
 - :white_check_mark: Screenshots `ctrl + shift + s`
 - :white_check_mark: Kill Respondus button `delete`

## DLL Bypasses

 - :white_check_mark: Bypass switch window detection
 - :white_check_mark: Bypass always focus LockDown Browser
 - :white_check_mark: Bypass close blacklisted applications
 - :white_check_mark: Bypass clear clipboard

## Not implemented
 - Unlock win + f1-f12 keys - LockDownBrowser.dll
 - Bypass LockDown Browser DRM - take screenshots of LockDown Browser

## How to use

1. Download the zip file from the Releases [here](https://github.com/Totsukawaii/UndownUnlock/releases/download/v1.0.2/UndownUnlock-v1.0.2.zip) (contains the dll file)

2. Double click `install_miniconda.bat`, make sure the file path doesn't have spaces or brackets

3. Right click and run `run_controller.bat` and `run_injector.bat` in Administrator mode and wait for the installations to complete if it's your first time running.

4. Launch Respondus.

5. :warning: If dll has successfully been injected a messagebox saying 'Injected' will appear. If dll has not been injected alt-tab will be detected and LockDownBrowser will close.

6. use `ctrl+up` to show other windows, followed by `ctrl+left` and `ctrl+right` to alt-tab, use `ctrl+down` to focus back on lockdown browser to type (or when the teacher is coming)

7. Once you exit LockDownBrowser, use `esc` to restore windows.

## General Issues

- **Is this detected? Will I be banned?**
  - *Use at your own risk.*

- **Why can't I type in LockDown Browser?**
  - *Press the down arrow, and click out of an answer box and click back in.*

- **Why does the drop down box not work?**
  - *Drop down boxes don't work during the test because of the focus hooks. Use the up and down arrows to choose options.*

- **Why is the screen white?**
  - *Press the down arrow to focus back on LockDown Browser.*

- **Why can't I screenshot in LockDownBrowser**
  - *You can't screenshot in LockDownBrowser, because of DRM protection any screenshot appear black.*

- **I'm stuck, how do I close the LockDown Browser?**
  - *If the controller is running, press the delete key.*
  
- **My taskbar disappeared. How do I make it come back?**
  - *If the controller is running, press F10. Otherwise, open LockDown Browser and manually close it again.*

- **Why do I get a warning of swiping tabs during the exam?**
  - *The DLL was not injected.*

- **Why is LockDown Browser still asking me to close my application?**
  - *Just click the 'close' button; the application will not actually close.*

- **Is the DLL malware?**
  - *Yes.*

## TODO

- Fake virtual camera & mic
- Bypass DRM

## References

coding > ChatGPT 4

injector > https://github.com/numaru/injector

https://systemfailu.re/2020/11/14/lockdownbrowser-analysis/

https://github.com/melotic/ThreateningYeti
