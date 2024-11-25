#!/usr/bin/env python

i = 0
login = "qbornet"


start = ord(login[3]) ^ 0x1337 + 0x5EEDED;
print(f"value to start with {start}")
while i < len(login):
    start += (start ^ ord(login[i])) % 0x539
    i += 1
print(f"end value {start}")
