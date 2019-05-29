
import sys
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
    print("fitler here..\n")
    return 0

