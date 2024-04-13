import psutil  # You'll need to install psutil: pip install psutil
from injector import Injector
import time
import os

injector = Injector()

# Enter your DLL path.
path_dll = os.path.join(os.getcwd(), "DLLHooks.dll")

# check if the DLL exists
if not os.path.exists(path_dll):
    path_dll = os.path.join(os.getcwd(), "DLLHooks/Release/DLLHooks.dll")
    if not os.path.exists(path_dll):
        raise FileNotFoundError("DLLHooks.dll not found. Either place the DLL in the same directory as this script or in the DLLHooks/Release directory.")


# Substring to look for in process names
target_substring = "LockDownBrowser"

# check if lockdown browser is running, if it is, end the process
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

print("Waiting for a LockDown Browser process to start...")
# Loop to wait for the process to start
while True:
    break_flag = False
    for proc in psutil.process_iter(['name']):
        proc_name = proc.name()
        if target_substring in proc_name:
            pid = proc.pid
            print(f"Found {proc_name} with PID: {pid}")
            try:
                # Once the process is found:
                injector.load_from_pid(pid)
                injector.inject_dll(path_dll)
                injector.unload()
                break_flag = True
                break
            except Exception as e:
                print(e)
                print("DLL injection failed. Retrying...")
    if break_flag:
        break

print("DLL injected successfully!")