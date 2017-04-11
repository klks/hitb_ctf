#Ref http://svn.python.org/projects/python/trunk/Demo/sockets/broadcast.py
# Send UDP broadcast packets
MYPORT = 32485

import sys, time
from socket import *

s = socket(AF_INET, SOCK_DGRAM)
s.bind(('', 0))
s.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)

while 1:
    data = "OH HAI!, THE FLAG IS 0123456789ABCDEF9876543210FEDCBA"	#Insert flag here
    s.sendto(data, ('<broadcast>', MYPORT))
    time.sleep(10)
