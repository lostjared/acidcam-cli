
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
    filter_num = 1104
    # acidcam -l to list different filter index numbers
    metacall("call_filter",filter_num, matrix)
    return 0
