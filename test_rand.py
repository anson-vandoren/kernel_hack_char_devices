#!/usr/bin/python3

import random

SAMPLE_SIZE = 1000

headcount = 0
coinflips = []

for i in range(SAMPLE_SIZE):
    newflip = random.randint(0,1)
    if newflip == 0:
        headcount += 1
    coinflips.append(newflip)

print(f"Heads: {headcount}")
print(f"Tails: {SAMPLE_SIZE - headcount}")
