#!/bin/env python3

from matplotlib.backend_bases import MouseButton, KeyEvent

from figure import FigureControl
from target import WestonControl, AGLControl, AndroidControl


class RemoteGui(FigureControl):

    def __init__(self, platform, connection, target, user="root", debug=False):

        if platform == "weston":
            self.tgt = WestonControl(connection, target, user, debug)
        elif platform == "agl-compositor":
            self.tgt = AGLControl(connection, target, user, debug)
        elif paltform == "android":
            self.tgt = AndroidControl(connection, target, user, debug)
        else:
            print("Not supported platform:", platform)
            exit()
        self.tgt.get_screenshot()

        super().__init__("screen.png", debug)
        self.is_recording = False
        #self.register_callback()
        #self.show_image()

    def cb_button_press(self, event):
        FigureControl.cb_button_press(self, event)
        #print("RemoteGui: press")

    def cb_button_release(self, event):
        #print("RemoteGui: release")
        click_start = self.click_start
        click_last  = self.last_ax_xy
        FigureControl.cb_button_release(self, event)
        if event.button == MouseButton.RIGHT:
            print("Screen update")
            self.tgt.get_screenshot()
            self.show_image()
        elif event.button == MouseButton.LEFT and click_start != None:
            if click_start == click_last:
                print("Touch input")
                self.tgt.touch_screen(click_start)
            else:
                print("Swipe input")
                self.tgt.swipe_screen(click_start, click_last)

    def cb_key_press(self, event):
        FigureControl.cb_key_press(self, event)
        #print("RemoteGui: key")
        if event.key == 'r':
            if self.is_recording == False:
                print("Start recording")
                self.is_recording = True
                self.tgt.start_recorde()
            else:
                print("Stop recording")
                self.is_recording = False
                self.tgt.stop_recorde()
                self.tgt.get_recorde_data()


if __name__ == "__main__":

    controller = RemoteGui("agl-compositor", "ssh", "192.168.1.10")
    controller.wait_close()

