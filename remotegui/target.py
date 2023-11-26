#!/bin/env python3

from abc import ABC, abstractmethod
import subprocess

UDEV_PATH="/dev/cuiinput"
#UDEV_PATH="/dev/uinput"


class TargetControl(ABC):

    def __init__(self, connection, target, user="root", debug=False):
        print("[TargetControl]")
        self.connection = connection
        self.user = user
        self.target = target
        self.shell = None
        self.shel_pull = None
        self.uinput = False
        self.debug = debug

        self.setup()
        self.target_check()
        self.target_setup()

    def do_local_cmd(self, command, info=True):
        # This method doesn't support space included arguments
        result = subprocess.run(command.split(), capture_output=True, text=True)
        if result.returncode != 0 and info == True:
            print("Failed to execute:", command)
            print(result.stderr)
        return result

    def do_remote_cmd(self, command, info=True):
        # This method doesn't support space included arguments
        cmd = " ".join([self.shell, command])
        result = subprocess.run(cmd.split(), capture_output=True, text=True)
        if result.returncode != 0 and info == True:
            print("Failed to execute:", cmd)
            print(result.stderr)
        return result

    def pull_file(self, filename):
        cmd = " ".join([self.shell_pull + filename, "."])
        result = subprocess.run(cmd.split(), capture_output=True, text=True)
        if result.returncode != 0:
            print("Failed to execute:", cmd)
        return result

    def setup(self):
        cmd = " ".join(["which", self.connection])
        result = subprocess.run(cmd.split(), capture_output=True, text=True)
        if result.returncode != 0:
            print("Not found connection tool:", self.connection)
            exit()
        if "adb" in self.connection:
            self.shell      = " ".join([self.connection, "-s", self.target, "shell"])
            self.shell_pull = " ".join([self.connection, "-s", self.target, "pull "])
        elif "ssh" == self.connection:
            self.shell      = " ".join([self.connection, self.user+"@"+self.target])
            self.shell_pull = " ".join(["scp", self.user+"@"+self.target+":"])
        else:
            print("Unkown connection tool:", self.connection)
            exit()

    def target_check(self):
        if self.do_remote_cmd("ls").returncode == 0:
            print("Connection check: Success")
        else:
            print("Connection check: Failed")
            exit()
        if self.do_remote_cmd("ls " +  UDEV_PATH, info=False).returncode == 0:
            print("Target uinput: available")
            self.uinput = True
        else:
            print("Target uinput: unavailable")
            self.uinput = False

    @abstractmethod
    def target_setup(self):
        pass

    @abstractmethod
    def is_ready(self):
        pass

    @abstractmethod
    def get_screenshot(self):
        pass

    @abstractmethod
    def start_recorde(self):
        pass

    @abstractmethod
    def stop_recorde(self):
        pass

    @abstractmethod
    def get_recorde_data(self):
        pass

    @abstractmethod
    def touch_screen(self, xy):
        pass

    @abstractmethod
    def swipe_screen(self, xy_begin, xy_end, period):
        pass


class WestonControl(TargetControl):

    def __init__(self, connection, target, user="root", debug=False):
        print("[WestonControl]")
        super().__init__(connection=connection, target=target, user=user, debug=debug)
        self.xdg_runtime_dir = None
        self.wayland_socket = None
        self.weston_debug = False
        self.get_screen_command = "weston-screenshooter"
        self.temporary_filename = "/tmp/screen.png"

        self.target_check()
        self.is_ready()

    def target_check(self):
        # Check wether compositor is launched with debug
        if self.do_remote_cmd("journalctl --no-pager | grep weston | grep -- --debug").returncode == 0:
            self.weston_debug = True
        else:
            self.weston_debug = False
            return
        # Get XDG_RUNTIME_DIR
        result = self.do_remote_cmd("dirname $(find /run/user -name wayland-[0-9] | head -n1)")
        if result.returncode == 0:
            self.xdg_runtime_dir = result.stdout
        else:
            self.xdg_runtime_dir = None
        # Get wayland socket filename
        result = self.do_remote_cmd("basename $(find /run/user -name wayland-[0-9] | head -n1)")
        if result.returncode == 0:
            self.wayland_socket = result.stdout
        else:
            self.wayland_socket = None

    def target_setup(self):
        # If target is not ready, use this method to be ready.
        print("[WestonControl] Not implemented: target_setup")

    def is_ready(self):
        if self.weston_debug == False or self.xdg_runtime_dir == None or self.wayland_socket == None:
            print("Target it not ready:")
            print("    launched with '--debug':", self.weston_debug)
            print("    XDG_RUNTIME_DIR:", self.xdg_runtime_dir)
            print("    WAYLAND_DISPLAY:", self.wayland_socket)
            return False
        else:
            return True

    def get_screenshot(self):
        if self.is_ready() == False:
            return
        self.do_remote_cmd("rm /home/root/*png", info=False)
        result = self.do_remote_cmd(
                "XDG_RUNTIME_DIR="+self.xdg_runtime_dir + " " + "WAYLAND_DISPLAY="+self.wayland_socket + " " + self.get_screen_command)
        if result.returncode != 0:
            return
        result = self.do_remote_cmd("mv /home/root/*png" + " " + self.temporary_filename)
        if result.returncode != 0:
            return
        result = self.pull_file(self.temporary_filename)
        if result.returncode != 0:
            return
        print("Success: get_screenshot")

    def start_recorde(self):
        print("[WestonControl] Not implemented: start_recorde")

    def stop_recorde(self):
        print("[WestonControl] Not implemented: stop_recorde")

    def get_recorde_data(self):
        print("[WestonControl] Not implemented: get_recorde_data")

    def touch_screen(self, xy):
        print("[WestonControl] Not implemented: touch_screen")

    def swipe_screen(self, xy_begin, xy_end, period=0.1):
        print("[WestonControl] Not implemented: swipe_screen")


class AGLControl(WestonControl):

    def __init__(self, connection, target, user="root", debug=False):
        print("[AGLControl]")
        super().__init__(connection=connection, target=target, user=user, debug=debug)
        self.get_screen_command = "agl-screenshooter"


class AndroidControl(TargetControl):

    def __init__(self, connection, target, user="root", debug=False):
        print("[AndroidControl]")
        super().__init__(connection=connection, target=target, user=user, debug=debug)

    def target_setup(self):
        print("[AndroidControl] Not implemented: target_setup")

    def is_ready(self):
        print("[AndroidControl] Not implemented: is_ready")

    def get_screenshot(self):
        print("[AndroidControl] Not implemented: get_screenshot")

    def start_recorde(self):
        print("[AndroidControl] Not implemented: start_recorde")

    def stop_recorde(self):
        print("[AndroidControl] Not implemented: stop_recorde")

    def get_recorde_data(self):
        print("[AndroidControl] Not implemented: get_recorde_data")

    def touch_screen(self, xy):
        print("[AndroidControl] Not implemented: touch_screen")

    def swipe_screen(self, xy_begin, xy_end, period=0.1):
        print("[AndroidControl] Not implemented: swipe_screen")


if __name__ == "__main__":
    target = AGLControl("ssh", "192.168.1.10")
    target.get_screenshot()

    #AndroidControl("adb", "123456")


