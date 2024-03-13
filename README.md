# UndownUnlock
Respondus LockDown Browser cheats

## Features

 - :white_check_mark: Alt tab with `ctrl + left` or `ctrl + right` 
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

1. Download the zip file from releases (contains the dll file)

2. Run `install_miniconda.bat`

3. Run both batch files `run_controller.bat` and `run_injector.bat` in Administrator mode

5. Launch Respondus.

6. :warning: If dll has successfully been injected a messagebox saying 'Injected' will appear. If dll has not been injected alt-tab will be detected and LockDownBrowser will close.

7. Use `ctrl+left` and `ctrl+right` to alt-tab, use `ctrl+up` to unfocus lockdown browser, use `ctrl+down` to focus lockdown browser to type (or when the teacher is coming)

## Q&A

Q: Is this detected? Will I be banned?
A: Use at your own risk.

Q: Why can't I type in LockDown Browser?
A: Press the down arrow, and click out of a answer box and click back in.

Q: Why does the drop down box not work?
A: Drop down boxes don't work during the test because of the focus hooks, but you can use the up and down arrows to choose the options of the drop down boxes.

Q: Why is the screen white?
A: Press the down arrow to focus back on LockDown Browser.

Q: I'm stuck, how do I close the LockDown Browser?
A: If then controller is running press the delete key.

Q: My taskbar disappeared. How do I make it come back?
A: If the controller is running press f10, else open LockDown Browser and manually close it again.

Q: Why do I get the warning of swiping tabs during the exam?
A: The dll was not injected.

Q: Why is LockDown Browser still asking me to close my application?
A: Just click the 'close' button, the application will not actually close.

Q: Is the dll malware?
A: Yes.

## TODO

- Fake virtual camera & mic
- Bypass DRM

## References

coding > ChatGPT 4

injector > https://github.com/numaru/injector

https://systemfailu.re/2020/11/14/lockdownbrowser-analysis/

https://github.com/melotic/ThreateningYeti