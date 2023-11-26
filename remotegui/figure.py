#!/bin/env python3

import matplotlib.pyplot as plt
from matplotlib.backend_bases import MouseButton
from matplotlib.backend_bases import KeyEvent
from PIL import Image
from time import sleep

import test


class FigureControl:

    def __init__(self, filename=None, debug=False):
        print("FigureControl init: start")
        fig = plt.figure()
        fig.show()
        ax = fig.add_subplot(1,1,1)

        self.fig = fig
        self.ax = ax
        self.last_event = None
        self.line_data = None
        self.line_drawing = False

        self.im = None
        self.screen_filename = filename
        self.screen_data = None
        self.screen_drawing = False

        self.click_start = None
        self.last_ax_xy = None

        self.alive = True
        self.debug = debug

        self.register_callback()
        self.show_image()
        print("FigureControl init: end")

    def register_callback(self):
        self.fig.canvas.mpl_connect('button_press_event',   self.cb_button_press)
        self.fig.canvas.mpl_connect('button_release_event', self.cb_button_release)
        self.fig.canvas.mpl_connect('motion_notify_event',  self.cb_motion)
        self.fig.canvas.mpl_connect('key_press_event',      self.cb_key_press)
        self.fig.canvas.mpl_connect('key_release_event',    self.cb_key_release)
        self.fig.canvas.mpl_connect('scroll_event',         self.cb_scroll)
        self.fig.canvas.mpl_connect('close_event',          self.cb_figure_close)

    def show_image(self):
        if self.debug == True:
            screen_image = test.create_fake_image(1920, 1080)
        else:
            #print("Not implemented")
            screen_image = Image.open(self.screen_filename)
        if self.im == None:
            self.im = self.ax.imshow(screen_image)
        else:
            self.im.set_data(screen_image)
        self.fig.canvas.draw()
        print("show_image: updated")

    def wait_close(self):
        plt.show()

    def close(self):
        self.alive = False
        print("Figure is closed")

    def line_draw(self, x, y):
        if self.line_drawing == False:
            self.line_drawing = True
            self.line_data = self.ax.plot(x, y, "k-").pop(0)
        else:
            self.line_data.set_data(x, y)
        self.fig.canvas.draw()

    def line_remove(self):
        if self.line_drawing == True:
            self.line_drawing = False
            self.line_data.remove()
            self.fig.canvas.draw()

    def cb_figure_close(self, event):
        self.close()

    def cb_button_press(self, event):
        self.last_event = event
        self.show_status(event)
        if event.inaxes == None:
            self.click_start = None
        elif event.button == MouseButton.LEFT:
            self.click_start = (event.xdata, event.ydata)

    def cb_button_release(self, event):
        self.last_event = event
        self.show_status(event)
        self.line_remove()
        self.click_start = None

    def cb_motion(self, event):
        self.last_event = event
        self.show_status(event)
        if event.inaxes != None:
            self.last_ax_xy = (event.xdata, event.ydata)
        if self.click_start != None:
            self.line_draw(
                    [self.click_start[0], self.last_ax_xy[0]],
                    [self.click_start[1], self.last_ax_xy[1]]
                    )

    def cb_key_press(self, event):
        self.last_event = event
        self.show_status(event)

    def cb_key_release(self, event):
        self.last_event = event
        self.show_status(event)

    def cb_scroll(self, event):
        self.last_event = event
        self.show_status(event)

    # https://matplotlib.org/stable/users/explain/figure/event_handling.html
    def show_status(self, event):
        if self.debug == True:
            print("================")
            if KeyEvent is type(event):
                print("keyboard: ------")
                print("    key:", event.key)
            else:
                print("fig: -----------")
                print("    x:", event.x)
                print("    y:", event.y)
                print("ax: ------------")
                print("    inaxes:", event.inaxes)
                print("    x:", event.xdata)
                print("    y:", event.ydata)
                print("click: ---------")
                print("    button", event.button)
                print("    isDoubleClick:", event.dblclick)
            print("")

if __name__ == '__main__':
    figure = FigureControl(debug=True)
    print("Waiting figure close...")
    figure.wait_close()
    print("Exiting...")


