#!/usr/bin/python

import socket
import time
import datetime
import struct
from threading import Thread
import sys
import time
import math
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
pitch  = 0
#Starting roll and pitch
kalmanX = Kalman()
kalmanY = Kalman()
kalAngleX = 0
kalAngleY = 0

gyroXangle = 0
gyroYangle = 0
timer = 0
gyroXrate = 0
gyroYrate = 0

def udpListenThread():
    seq = 0
    gyroX = 0

    while isRunning:
        
        try:
            #data, addr = sock.recvfrom( 1024 ) # Addr[0] is IP, Addr[1] is port 
            #data_arr = data.decode().split(',') #seq, myo, Gyro, Acc
            seq += 1
            accX = 100
            accY = 0
            accZ = 0
            gyroY = 0
            gyroZ = 0
            data = [seq, seq %20,gyroX, gyroY, gyroZ, accX, accY, accZ]
            kalman_iterate(accX, accY, accZ, gyroX, gyroY, gyroZ)

            if ((seq % 1) == 0):
                #print (addr[0][-10:], "sent:\t", data)
                print(data)
                gyroX += 1
               # gyroX = (gyroX - 180) %180 + 180
                #gyroY -= 1
                print_stats()
                
            if ((seq % 20) == 0):
                kalman_setup(accX, accY, accZ, gyroX, gyroY, gyroZ)
                accX += 20
              #  accX = (accX -180) % 180 + 180
                
            
            # Catch IP of tags not already "synchronised" with our system
            #if(addr[0] not in tags):
              #  tags.append(addr[0])
             #   print ("Tags: ", tags)
            time.sleep(1)

        except socket.timeout:
            print ("Got error")
            pass

def kalman_setup(accX, accY, accZ, gyroX, gyroY, gyroZ):
    global timer
    global kalmanX
    global kalmanY
    global roll
    global pitch
    global kalAngleX
    global kalAngleY
    global gyroXangle
    global gyroYangle
    global gyroXrate
    global gyroYrate
    roll = calc_roll(accX, accY, accZ)
    pitch = calc_pitch(accX, accY, accZ)
    #Starting roll and pitch
    kalmanX.setAngle(roll)
    kalmanY.setAngle(pitch)

    gyroXangle = roll
    gyroYangle = pitch
    timer = time.time() * 1000000

def kalman_iterate(accX, accY, accZ, gyroX, gyroY, gyroZ):
    global timer
    global kalmanX
    global kalmanY
    global roll
    global pitch
    global kalAngleX
    global kalAngleY
    global gyroXangle
    global gyroYangle
    global gyroXrate
    global gyroYrate
    dt = (time.time() * 1000000 - timer) / 1000000
    timer = time.time() * 1000000
    roll = calc_roll(accX, accY, accZ)
    pitch = calc_pitch(accX, accY, accZ)

    gyroXrate = gyroX / 20  #These should be in deg/s
    gyroYrate = gyroY /20

    if ((roll < -90 and kalAngleX > 90) or (roll > 90 and kalAngleX < -90)):
        kalmanX.setAngle(roll)
    else:
        kalAngleX = kalmanX.getAngle(roll, gyroXrate, dt)
    
    if (abs(kalAngleX) > 90):
        gyroYrate = -gyroYrate
        kalAngleY = kalmanY.getAngle(pitch, gyroYrate, dt)

    gyroXangle += gyroXrate * dt # Calculate gyro angle without any filter
    gyroYangle += gyroYrate * dt

    if (gyroXangle < -180  or gyroXangle > 180):
        print("gyro and kal same")
        gyroXangle = kalAngleX
    if (gyroYangle < -180 or gyroYangle > 180):
        gyroYangle = kalAngleY

def print_stats():
    print("X\tRoll: ", roll, "\tGyro: ", gyroXangle, "\tKalAngle: ", kalAngleX)
    print("Y\tPitch: ", pitch, "\tGyro: ", gyroYangle, "\tKalAngle: ", kalAngleY)

def calc_roll(accX, accY, accZ):
    return math.atan2(accY, accZ) * 180/math.pi

def calc_pitch(accX, accY, accZ):
    try:
        return math.atan(-accX / math.sqrt(accY * accY * accZ * accZ)) * 180/math.pi
    except:
        return pitch



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



