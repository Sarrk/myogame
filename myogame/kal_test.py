#!/usr/bin/python

import socket
import time
import datetime
import struct
from threading import Thread
import sys

from kal import Kalman


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
roll = 0
pitch = 0
#Starting roll and pitch
kalmanX = Kalman()
kalmanY = Kalman()
kalAngleX = 0
kalAngleY = 0

gyroXangle = 0
gyroYangle = 0
timer = 0

def udpListenThread():

    while isRunning:
        
        try:
            data, addr = sock.recvfrom( 1024 ) # Addr[0] is IP, Addr[1] is port 
            data_arr = data.decode().split(',') #seq, myo, Gyro, Acc
            seq = int(data_arr[0])
            accX = int(data_arr[5])
            accY = int(data_arr[6])
            accZ = int(data_arr[7])
            gyroX = int(data_arr[2])
            gyroY = int(data_arr[3])
            gyroZ = int(data_arr[4])
            loop(accX, accY, accZ, gyroX, gyroY, gyroZ)

            if ((seq % 5) == 0):
                print (addr[0][-10:], "sent:\t", data)
                print_stats(roll, gyroXangle, kalAngleX, pitch, gyroYangle, kalAngleY)
            if ((seq % 20) == 0):
                kalman_setup(accX, accY, accZ, gyroX, gyroY, gyroZ)
            
            # Catch IP of tags not already "synchronised" with our system
            if(addr[0] not in tags):
                tags.append(addr[0])
                print ("Tags: ", tags)

        except socket.timeout:
            print ("Got error")
            pass

def kalman_setup(accX, accY, accZ, gyroX, gyroY, gyroZ):
    roll = calc_roll(accX, accY, accZ)
    pitch = calc_pitch(accX, accY, accZ)
    #Starting roll and pitch
    kalmanX.setAngle(roll)
    kalmanY.setAngle(pitch)

    gyroXangle = roll
    gyroYangle = pitch
    timer = Sys.time()

def loop(accX, accY, accZ, gyroX, gyroY, gyroZ):
    dt = (Sys.time() - timer) #FIND UNITS FOR this
    timer = Sys.time()
    roll = calc_roll(accX, accY, accZ)
    pitch = calc_pitch(accX, accY, accZ)

    gyroXrate = gyroX * 100 #These should be in deg/s
    gyroYrate = gyroY * 100

    if ((roll < -90 and kalAngleX > 90) or (roll > 90 and kalAngleX < -90)):
        kalmanX.setAngle(roll)
    else:
        kalAngleX = kalmanX.getAngle(roll, gyroXrate, dt)
    
    if (Math.abs(kalAngleX) > 90):
        gyroYrate = -gyroYrate
        kalAngleY = kalmanY.getAngle(pitch, gyroYrate, dt)

    gyroXangle += gyroXrate * dt # Calculate gyro angle without any filter
    gyroYangle += gyroYrate * dt

    if (gyroXangle < -180 or gyroXangle > 180):
        gyroXangle = kalAngleX
    if (gyroYangle < -180 or gyroYangle > 180):
        gyroYangle = kalAngleY

def print_stats(roll, gyroXangle, kalAngleX, pitch, gyroYangle, kalAngleY):
    print("X\tRoll: ", roll, "\tGyro: ", gyroXangle, "\tKalAngle: ", kalAngleX)
    print("Y\tPitch: ", pitch, "\tGyro: ", gyroYangle, "\tKalAngle: ", kalAngleY)

def calc_roll(accX, accY, accZ):
    return atan2(accY, accZ) * RAD_TO_DEG

def calc_pitch(accX, accY, accZ):
    return atan(-accX / sqrt(accY * accY * accZ * accZ)) * RAD_TO_DEG



def udpSendThread():
  # Send to UDP_OUTGOING_PORT, packets will originate from UDP_INCOMING_PORT

    while isRunning:
        if (rec != 1):
            # sleep for 5 seconds
            time.sleep(5)

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



