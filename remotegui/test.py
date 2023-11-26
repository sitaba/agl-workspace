#!/bin/env paython3

import numpy as np
from PIL import Image

# https://note.nkmk.me/python-numpy-generate-gradation-image/
def get_gradient_2d(start, stop, width, height, is_horizontal):
    if is_horizontal:
        return np.tile(np.linspace(start, stop, width), (height, 1))
    else:
        return np.tile(np.linspace(start, stop, height), (width, 1)).T

def get_gradient_3d(width, height, start_list, stop_list, is_horizontal_list):
    result = np.zeros((height, width, len(start_list)), dtype=np.float64)
    for i, (start, stop, is_horizontal) in enumerate(zip(start_list, stop_list, is_horizontal_list)):
        result[:, :, i] = get_gradient_2d(start, stop, width, height, is_horizontal)
    return result

def create_fake_image(width, height):
    array = get_gradient_3d(width, height, (0, 0, 192), (255, 255, 64), (True, False, False))
    #array = get_gradient_3d(width, height, (0, 0, 0), (255, 255, 64), (False, True, False))
    return Image.fromarray(np.uint8(array))

