#!/usr/bin/python

import socket
import time
import datetime
import struct
import StringIO
from threading import Thread
import sys


UDP_TIMESYNC_PORT = 3000 # node listens for timesync packets on port 4003
UDP_REPLY_PORT = 3001 # node listens for reply packets on port 7005

M2_IP = "aaaa::212:4b00:799:9a01"
M1_IP = "aaaa::212:4b00:7b4:a106"

isRunning = True

sock = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM, 0)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
sock.bind(('', UDP_REPLY_PORT))

seq = 0
rec = 0
#f = open('output', 'w')

def udpListenThread():

  # listen on UDP socket port UDP_TIMESYNC_PORT

 
  

  while isRunning:
    
    try:
      rec =1
      data, addr = sock.recvfrom( 1024 )
   #   seq =seq +1
    #  time, rssi = data.split(',')
    #  timestamp = int(time)
    #  utc = datetime.datetime.fromtimestamp(timestamp)
   #   dist = (abs(int(rssi)) - 20)
      #if (dist > 5) :
      #  dist -= 5
    #  dist *= 3
   #   print "Reply from:", addr[0], "UTC[s]:", timestamp, "Localtime:", utc.strftime("%Y-%m-%d %H:%M:%S"), "Distance: ", dist, "cm"
      print "Reply from:", addr[0],"\t", data
     # f.write(addr[0] + "\n")
      
    except socket.timeout:
      print "reply from:", addr[0], "\t", data
      rec =1
      pass
    
def udpSendThread():

  #sock = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM, 0)

  while isRunning:
    if (rec != 1):
      timestamp = int(time.time())
      print "Sending timesync packet with UTC[s]:", timestamp, "Localtime:", time.strftime("%Y-%m-%d %H:%M:%S")
      buf = "%s" % (timestamp)
   # print "TIMESTAMP AS STRING: " + buf
    # send UDP packet to nodes - Replace addresses with your sensortag routing address (e.g. aaaa::<sensortag ID>)
   # sock.sendto(struct.pack("I", timestamp), ("aaaa::212:4b00:7b4:a106", UDP_TIMESYNC_PORT))
      sock.sendto(struct.pack("I", timestamp), ("aaaa::212:4b00:799:9a01", UDP_TIMESYNC_PORT))
      seq = 0
  #  sock.sendto(struct.pack("I", timestamp), ("aaaa::212:4b00:799:c87", UDP_TIMESYNC_PORT))
    sock.sendto(struct.pack("I", timestamp), ("aaaa::212:4b00:688:f83", UDP_TIMESYNC_PORT))
    
    # sleep for 10 seconds
    time.sleep(5)


# start UDP listener as a thread
t1 = Thread(target=udpListenThread)
t1.start()
print "Listening for incoming packets on UDP port", UDP_REPLY_PORT

time.sleep(1)

# start UDP timesync sender as a thread
t2 = Thread(target=udpSendThread)
t2.start()

print "Sending timesync packets on UDP port", UDP_TIMESYNC_PORT
print "Exit application by pressing (CTRL-C)"

try:
  while True:
    # wait for application to finish (ctrl-c)
    time.sleep(1)
except KeyboardInterrupt:
  print "Keyboard interrupt received. Exiting."
  isRunning = False
  sock.close()
  quit()
#  f.close()



