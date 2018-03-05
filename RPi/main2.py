import time
import threading

from android import Android
import global_settings as gs


if __name__ == "__main__":
    """
    For final implementation, only android thread will be initialized first.
    RPI, Algorithm and Arduino thread will only be initialized when command
    received from Android thread.
    """
    # TODO: android.py file that connects with Android device using bluetooth
    android_thread = threading.Thread(target=Android)
    android_thread.daemon = True
    android_thread.start()

    while 1:
        time.sleep(1)
