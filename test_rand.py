#!/usr/bin/python3

import os
import random


def test_random():
    random.seed(None)  # seed comes from /dev/urandom
    SAMPLE_SIZE = 1000

    headcount = 0
    coinflips = []

    for i in range(SAMPLE_SIZE):
        newflip = random.randint(0,1)
        if newflip == 0:
            headcount += 1
        coinflips.append(newflip)

    print(f"\tHeads: {headcount}")
    print(f"\tTails: {SAMPLE_SIZE - headcount}")
    return headcount

if __name__ == '__main__':
    print("building rootkit")
    os.system('make')

    print("\n--testing random generation before--")
    results = [test_random() for x in range(3)]
    same_heads = all([result == results[0] for result in results])
    print(f"Same results each time: {same_heads}")

    print("\n**inserting rootkit**\n")
    os.system('sudo insmod char_interfering.ko')
    
    print("--testing random generation after--")
    results = [test_random() for x in range(3)]
    same_heads = all([result == results[0] for result in results])
    print(f"Same results each time: {same_heads}")

    print("unloading rootkit")
    os.system('sudo rmmod char_interfering')

