import random

def filter(data: bytes) -> bytes: 
	mutable_bytes = bytearray(data)
	for i in range(0, len(data) -1):
		mutable_bytes[i] = random.randint(0, 255)
	return bytes(mutable_bytes)


