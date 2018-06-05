#!/usr/bin/python

import socket
import time
import datetime
import struct
import StringIO
from threading import Thread
import sys


# note that the node will only accept packets from the port that it is sending to...
UDP_OUTGOING_PORT = 3000 # Node listens for packets on this port
UDP_INCOMING_PORT = 3001 # Script listens for packets on this port

M2_IP = "aaaa::212:4b00:799:9a01"
# M1_IP = "aaaa::212:4b00:7b4:a106"
M1_IP = "aaaa::212:4b00:799:9502"


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

def udpListenThread():

  # Listen to UDP_INCOMING_PORT

  while isRunning:
    
    try:
      data, addr = sock.recvfrom( 1024 ) # Addr[0] is IP, Addr[1] is port 
      print addr[0], "sent:\t", data 
      
      # Catch IP of tags not already "synchronised" with our system
      if(addr[0] not in tags):
        tags.append(addr[0])
        print "gottem", tags

    except socket.timeout:
      print "Got error"
      pass
    
def udpSendThread():

  # Send to UDP_OUTGOING_PORT, packets will originate from UDP_INCOMING_PORT

  while isRunning:
    if (rec != 1):
      timestamp = int(time.time())
   #   print "Sending timesync packet with UTC[s]:", timestamp, "Localtime:", time.strftime("%Y-%m-%d %H:%M:%S")
      buf = "%s" % (timestamp)
   # print "TIMESTAMP AS STRING: " + buf
      sock.sendto(struct.pack("I", timestamp), ("aaaa::212:4b00:799:9a01", UDP_OUTGOING_PORT))
      seq = 0
      sock.sendto(struct.pack("I", timestamp), ("aaaa::212:4b00:688:f83", UDP_OUTGOING_PORT))
      sock.sendto(struct.pack("I", timestamp), (M1_IP, UDP_OUTGOING_PORT))
    # sleep for 5 seconds
      time.sleep(5)


# start UDP listener as a thread
t1 = Thread(target=udpListenThread)
t1.daemon = True
print "Listening for packets on UDP port ", UDP_INCOMING_PORT
t1.start()

time.sleep(1)

# start UDP timesync sender as a thread
t2 = Thread(target=udpSendThread)
t2.daemon = True
print "Sending packets to UDP port ", UDP_OUTGOING_PORT, "from port ", UDP_INCOMING_PORT
t2.start()

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



