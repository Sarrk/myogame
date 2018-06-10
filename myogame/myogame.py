#!/usr/bin/python

import socket
import time
import datetime
import struct
from threading import Thread
import sys

import pandas as pd
from random import randrange
import os
import numpy as np

# multithreading
from queue import LifoQueue

# gym imports
import os
os.environ['DYLD_FALLBACK_LIBRARY_PATH'] = '/lib:/usr/lib:/usr/bin/lib:/' + os.environ['DYLD_FALLBACK_LIBRARY_PATH'];

import pyglet
import gym
import time

# pygame imports
import pygame
import sys
import random
from pygame.locals import *
 
# init pygame
pygame.init()
pygame.key.set_repeat(1, 40)

# init game env
env = gym.make('MountainCar-v0')
env.reset()


# note that the node will only accept packets from the port that it is sending to...
UDP_OUTGOING_PORT = 3000 # Node listens for packets on this port
UDP_INCOMING_PORT = 3001 # Script listens for packets on this port

FOOT_TAG = "aaaa::212:4b00:799:9a01"
RIGH_HAND_TAG = "aaaa::212:4b00:7b4:a106"
LEFT_HAND_TAG = "aaaa::212:4b00:799:9502"


isRunning = True

# Create the socket that will listen for packets from the node
# Note that we don't really care what port the packets come from
sock = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM, 0)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
sock.bind(('', UDP_INCOMING_PORT))

seq = 0
rec = 0

#f = open('output', 'w')

tags = []

buffer = []

def udpListenThread():
    printCtr = 0
    ipa = 0
    # Listen to UDP_INCOMING_PORT
    f = open("raw_data.csv", "w")

    filenames = []

    while isRunning:
        
        try:
            data, addr = sock.recvfrom( 1024 ) # Addr[0] is IP, Addr[1] is port 
            data_arr = data.decode().split(',')
            seq = int(data_arr[0])
            
            now = time.time()
            freq='100ms'
            index = pd.DatetimeIndex(end=now * 1e9, 
                                     freq=pd.Timedelta(freq), 
                                     periods=1)
            df = pd.DataFrame({ "myo":[int(data_arr[1])],
                                "gyro_x":[int(data_arr[2])], 
                                "gyro_y":[int(data_arr[3])], 
                                "gyro_z":[int(data_arr[4])], 
                                "acc_x":[int(data_arr[5])], 
                                "acc_y":[int(data_arr[6])], 
                                "acc_z":[int(data_arr[7])]},
                                index=index)
            df.set_index(index, inplace=True)

            q.put(df)

            # set directory
            directory = "data/" + str(addr[0][-10:])
            if not os.path.exists(directory):
                os.makedirs(directory)

            new_filename = directory + "/" + str(index[0]) + ".csv"
            df.to_csv(new_filename)
            filenames.append(new_filename)
            if (len(filenames) >= 1000):
                #remove old file
                oldest = filenames.pop(0)
                os.remove(oldest)

            if ((seq % 5) == 0):
                print (addr[0][-10:], "sent:\t", data)
                ipa = addr[0]
                printCtr = 0
            printCtr+= 1
            
            # Catch IP of tags not already "synchronised" with our system
            if(addr[0] not in tags):
                tags.append(addr[0])
                print ("Tags: ", tags)

        except socket.timeout:
            print ("Got error")
            pass
    

def udpSendThread():
  # Send to UDP_OUTGOING_PORT, packets will originate from UDP_INCOMING_PORT

    while isRunning:
        if (rec != 1):
            # sleep for 5 seconds
            time.sleep(5)

# SIMULATION CODE
# --------------------------------------------------------------
def moving_av(val, frame):
    if (len(buffer) >= frame):
        buffer.pop(0)
    
    buffer.append(val)
    return np.mean(buffer)

def getInputKeys(command):
    if (not q.empty()):
        command = 1

        df = q.get()
        myo = df['myo'][0]
        av = moving_av(myo, 2)
        print (myo, av)

        if (av > 1000):
            command = 2

        for event in pygame.event.get():
            if event.type == KEYDOWN:
                if event.key == K_LEFT:
                    # print("LEFT")
                    command = 0
                elif event.key == K_RIGHT:
                    # print("RIGHT")
                    command = 2
                elif event.key == K_q:
                    pygame.quit()
                    sys.exit()
    return command

def updateGameEnv():
    print('\n\n\n-----------------------------------REHAB GAME v0.1-----------------------------------')
    print('Instuctions: \nUse your feet in an alternating pattern to climb your way to the top on the hill.\nCapture the flag to win!')
    print('-------------------------------------------------------------------------------------')
    command = 1
    while True:
        command = getInputKeys(command)
        env.render()
        observation, reward, done, info = env.step(command)
        if (observation[0] == 0.6):
            response = input("You Win! Start Again? (Y/N)\n")
            if (response == 'Y'):
                env.reset()
            else:
                print("Thanks for playing!\n\n\n")
                pygame.quit()
                sys.exit()
        # time.sleep(0.01)


# --------------------------------------------------------------
q = Queue()

# start UDP listener as a thread
t1 = Thread(target=udpListenThread)
t1.daemon = True
print ("Listening for packets on UDP port ", UDP_INCOMING_PORT)
t1.start()

time.sleep(1)

# start UDP timesync sender as a thread
t2 = Thread(target=udpSendThread)
t2.daemon = True
print ("Sending packets to UDP port ", UDP_OUTGOING_PORT, "from port ", UDP_INCOMING_PORT)
t2.start()

updateGameEnv()

print ("Exit application by pressing (CTRL-C)")

try:
    while True:
        # wait for application to finish (ctrl-c)
        time.sleep(1)
except KeyboardInterrupt:
    print ("Keyboard interrupt received. Exiting.")
    isRunning = False
    sock.close()
    quit()
    #  f.close()



