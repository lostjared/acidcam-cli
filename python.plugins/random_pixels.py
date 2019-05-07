import random

import random

def array_size(width: int, height: int) -> int:
    global screen_width
    global screen_height
    global pitch_value
    screen_width = width
    screen_height = height
    pitch_value = width*3
    return 0

def filter(data: bytes) -> bytes:
    mutable_bytes = bytearray(data)
    for z in range(0, screen_height-1):
        for i in range(0, screen_width-1):
            mutable_bytes[(z * pitch_value)+(i*3)] = random.randint(0, 255) # B
            mutable_bytes[(z * pitch_value)+(i*3)+1] = random.randint(0, 255) # G
            mutable_bytes[(z * pitch_value)+(i*3)+2] = random.randint(0, 255) # R
    return bytes(mutable_bytes)


