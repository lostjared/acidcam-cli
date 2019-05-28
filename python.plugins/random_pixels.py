import random, sys
sys.path.append("/usr/local/lib")
from metacall import metacall

def array_size(width: int, height: int) -> int:
    global screen_width
    global screen_height
    global pitch_value
    screen_width = width
    screen_height = height
    pitch_value = width*3
    return 0

def filter(matrix):
    for z in range(0, screen_height-1):
        for i in range(0, screen_width-1):
            value = metacall('matrix_getpixel',matrix, i, z)
            metacall('matrix_setpixel_bgr',matrix, i, z, random.randint(0,255),random.randint(0, 255),random.randint(0,255))
    return 0


