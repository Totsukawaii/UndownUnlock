import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QRubberBand
from PyQt5.QtCore import Qt, QRect, QPoint, QSize
from PIL import ImageGrab
import pywinctl as pwc
import keyboard
import win32gui
import win32con
import win32clipboard
import win32process
import sys
from io import BytesIO
import time
import psutil

window_blacklist = ["", "Program Manager", "Microsoft Text Input Application", "LGControlCenterRTManager", "Media Player"]

# Snipping Tool Class from the first script
class SnippingTool(QMainWindow):
    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):
        self.setWindowTitle('Snipping Tool')
        self.setWindowOpacity(0.5)
        self.setWindowFlags(Qt.WindowStaysOnTopHint)
        self.showFullScreen()
        self.rubber_band = QRubberBand(QRubberBand.Rectangle, self)
        self.origin = QPoint()

    def keyPressEvent(self, event):
        if event.key() == Qt.Key_Escape:
            self.close()

    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            self.origin = event.pos()
            self.rubber_band.setGeometry(QRect(self.origin, QSize()))
            self.rubber_band.show()

    def mouseMoveEvent(self, event):
        if not self.origin.isNull():
            self.rubber_band.setGeometry(QRect(self.origin, event.pos()).normalized())

    def mouseReleaseEvent(self, event):
        if event.button() == Qt.LeftButton:
            self.capture()

    def send_to_clipboard(self, image):
        output = BytesIO()
        image.convert('RGB').save(output, 'BMP')
        data = output.getvalue()[14:]
        output.close()

        win32clipboard.OpenClipboard()
        win32clipboard.EmptyClipboard()
        win32clipboard.SetClipboardData(win32clipboard.CF_DIB, data)
        win32clipboard.CloseClipboard()

    def capture(self):
        rect = self.rubber_band.geometry()
        self.close()
        x, y, w, h = rect.x(), rect.y(), rect.width(), rect.height()
        img = ImageGrab.grab(bbox=(x, y, x+w, y+h))
        # save to clipboard
        self.send_to_clipboard(img)

# Global dictionary to store window positions and sizes
window_positions = {}

def set_window_on_top(hwnd):
    # Check if the window is minimized
    if win32gui.IsIconic(hwnd):
        # Restore the window to normal or maximized state
        win32gui.ShowWindow(hwnd, win32con.SW_RESTORE)
    win32gui.SetWindowPos(hwnd, win32con.HWND_TOPMOST, 0, 0, 0, 0,
                          win32con.SWP_NOMOVE | win32con.SWP_NOSIZE | win32con.SWP_SHOWWINDOW)

def set_window_not_on_top(hwnd):
    win32gui.SetWindowPos(hwnd, win32con.HWND_NOTOPMOST, 0, 0, 0, 0,
                          win32con.SWP_NOMOVE | win32con.SWP_NOSIZE | win32con.SWP_SHOWWINDOW)

def cycle_right(event = None):
    global current_overlay_index
    current_overlay_index += 1
    cycle_windows_and_set_top(direction="right")

def cycle_left(event = None):
    global current_overlay_index
    current_overlay_index -= 1
    cycle_windows_and_set_top(direction="left")

def get_all_app_windows():
    windows = pwc.getAllWindows()
    windows = [window for window in windows if hasattr(window, "title")
               and window._parent == 0
               and window.title not in window_blacklist]
    return windows

def is_exe_window(hwnd):
    try:
        _, pid = win32process.GetWindowThreadProcessId(hwnd)
        process = psutil.Process(pid)
        return process.exe().endswith('.exe')
    except Exception as e:
        print(e)
        return False

def cycle_windows_and_set_top(direction="right"):
    try:
        global current_overlay_index
        windows = get_all_app_windows()

        if len(windows) == 0:
            print("No valid windows found.")
            return

        windows.sort(key=lambda x: x._hWnd)
        
        next_window_index = current_overlay_index % len(windows)
        next_window = windows[next_window_index]

        hwnd = next_window._hWnd
        if is_exe_window(hwnd):
            try:
                next_window.activate()
            except Exception as e:
                print(e)
                cycle_windows_and_set_top()

            print("Switching to", next_window.title)
            set_window_on_top(hwnd)
        else:
            if direction == "right":
                current_overlay_index += 1
            else:
                current_overlay_index -= 1
            cycle_windows_and_set_top()
    except Exception as e:
        print(e)


def minimize_all_windows(event=None):
    try:
        global window_positions  # Access the global dictionary
        windows = get_all_app_windows()
        for window in windows:
            if "Respondus LockDown Browser" != window.title:
                hwnd = window._hWnd
                is_maximized = window.isMaximized  # Check if the window is maximized
                # Save the window's current position, size, and maximized state before minimizing
                rect = win32gui.GetWindowRect(hwnd)
                window_positions[hwnd] = (rect, is_maximized)  # Store position, size, and state
                window.minimize()
        # focus on LockDownBrowser
        for window in windows:
            if "Respondus LockDown Browser" == window.title:
                window.activate()
                hwnd = window._hWnd
                set_window_on_top(hwnd)
        print("Minimizing all windows...")
    except Exception as e:
        print(e)


def unminimize_all_windows(event=None):
    try:
        time.sleep(0.1)  # Wait for the function to hook
        cycle_windows_and_set_top()
    except Exception as e:
        print(e)
    
def cleanup_and_exit(event=None):
    try:
        windows = get_all_app_windows()
        hwnds = [window._hWnd for window in windows]
        for hwnd in hwnds:
                set_window_not_on_top(hwnd)
        print("Exiting...")
        sys.exit()  # Use sys.exit() to exit the program
    except Exception as e:
        print(e)

def close_respondus(event=None):
    try:
        # Close LockDownBrowser
        target_substring = "LockDownBrowser"
        for proc in psutil.process_iter(['name']):
            proc_name = proc.name()
            if target_substring in proc_name:
                pid = proc.pid
                print(f"Found {proc_name} with PID: {pid}")
                try:
                    # Once the process is found: end the process
                    proc.kill()
                except Exception as e:
                    print(e)
                    print("Failed to close " + proc_name)
    except Exception as e:
        print(e)


def show_taskbar(event=None):
    try:
        # Show the taskbar
        hwnd = win32gui.FindWindow("Shell_traywnd", None)
        win32gui.ShowWindow(hwnd, win32con.SW_SHOW)
    except Exception as e:
        print(e)

def hide_taskbar(event=None):
    try:
        # Hide the taskbar
        hwnd = win32gui.FindWindow("Shell_traywnd", None)
        win32gui.ShowWindow(hwnd, win32con.SW_HIDE)
    except Exception as e:
        print(e)

def show_snipping_tool(event=None):
    try:
        app = QApplication(sys.argv)  # Ensure this only runs once
        snipping_tool = SnippingTool()
        app.exec_()
    except Exception as e:
        print(e)

current_overlay_index = 0 

# Modify keyboard listener setup to include Ctrl + Shift + S for the Snipping Tool
keyboard.add_hotkey("ctrl+shift+s", show_snipping_tool)
# Keyboard listener
keyboard.add_hotkey("ctrl+right", cycle_right)
keyboard.add_hotkey("ctrl+left", cycle_left)
keyboard.add_hotkey("ctrl+down", minimize_all_windows)
keyboard.add_hotkey("ctrl+up", unminimize_all_windows)
keyboard.on_press_key("esc", cleanup_and_exit)
# key del to close respondus
keyboard.on_press_key("delete", close_respondus)
keyboard.on_press_key("f8", hide_taskbar)
keyboard.on_press_key("f10", show_taskbar)

keyboard.wait()

