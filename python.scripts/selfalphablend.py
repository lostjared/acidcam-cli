import random, sys
sys.path.append("/usr/local/lib")
from metacall import metacall

alpha = 1.0
dir = 1

def array_size(width: int, height: int) -> int:
    global screen_width
    global screen_height
    global pitch_value
    screen_width = width
    screen_height = height
    pitch_value = width*3
    return 0

def filter(matrix):
    global alpha
    global dir
    for z in range(0, screen_height-1):
        for i in range(0, screen_width-1):
            value = metacall('matrix_getpixel',matrix, i, z)
            metacall('matrix_setpixel_bgr',matrix, i, z, value[0]*alpha,value[1]*alpha,value[2]*alpha)
    if(dir == 1):
        alpha += 0.1
        if(alpha >= 10.0):
            alpha = 10.0
            dir = 0
    elif(dir == 0):
        alpha -= 0.1
        if(alpha <= 1.0):
            alpha = 1.0
            dir = 1
    return 0
