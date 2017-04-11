#!/usr/bin/python
"""
Released under the MIT-license:

Copyright (c) 2011, Earl Marcus Tan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
"""
from SimpleXMLRPCServer import SimpleXMLRPCServer
from SimpleXMLRPCServer import SimpleXMLRPCRequestHandler
from datetime import datetime
import SocketServer
import thread
import time
import random
import copy
import struct

msg = "By the time you are reading this, the key used to decode this message has been blacklisted by the server.\nPS:This message will not change"
encMsg = ""
key = ""
threadLock = None
logLock = None
lastTime = None
rotateTime = 300 #5 minutes
rng = None

def Log(msg):
	global logLock
	logLock.acquire()
	m = "[%.2d:%.2d] " % time.localtime()[3:5]
	m += msg + "\n"
	log = open("Log.txt", 'a')
	log.write(m)
	log.close()
	print m,
	logLock.release()
	
def blacklistKey(key):
	#Connect to db and put key in blacklist
	pass
	
def completed_number(prefix, length):
	global rng
	ccnumber = prefix
	
	# generate digits
	while len(ccnumber) < (length - 1):
		digit = rng.choice(['0',  '1', '2', '3', '4', '5', '6', '7', '8', '9'])
		ccnumber.append(digit)

	# Calculate sum 
	sum = 0
	pos = 0

	reversedCCnumber = []
	reversedCCnumber.extend(ccnumber)
	reversedCCnumber.reverse()

	while pos < length - 1:
		odd = int( reversedCCnumber[pos] ) * 2
		if odd > 9:
			odd -= 9
		sum += odd
		if pos != (length - 2):
			sum += int( reversedCCnumber[pos+1] )
		pos += 2

    # Calculate check digit
	checkdigit = ((sum / 10 + 1) * 10 - sum) % 10
	ccnumber.append( str(checkdigit) )

	return ''.join(ccnumber)

def generateKey():
	global rng, key
	PrefixList16 = [['2', '0', '1', '4'],
					['2', '1', '4', '9'],
					['3', '0', '8', '8'],
					['3', '0', '9', '6'],
					['3', '1', '1', '2'],
					['3', '1', '5', '8'],
					['3', '3', '3', '7'],
					['3', '5', '2', '8'],
					['3', '0', '0'],
					['3', '0', '1'],
					['3', '0', '2'],
					['3', '0', '3'],
					['3', '6'],
					['3', '8'],
					['4', '5', '3', '9'], 
					['4', '5', '5', '6'], 
					['4', '9', '1', '6'],
					['4', '5', '3', '2'], 
					['4', '9', '2', '9'],
					['4', '4', '8', '6'],
					['4', '7', '1', '6'],
					['4'] 
					]
					
	ccnumber = copy.copy( rng.choice(PrefixList16) )
	key = completed_number(ccnumber, 16)
	Log("Key : %s" % (key,))
	blacklistKey(key)
	
def encryptMessage():
	global msg, key, encMsg
	
	encMsg = ""
	i = 0
	for m in msg:
		if i == 16:
			i = 0
		k = key[i]
		encMsg += struct.pack("<B", ord(m) ^ ord(k))
		i += 1
	
def mangleKey(key):
	temp = key
	
	#4716-75903-17-79307
	#K-L-K-S
	#K4716-759L03-17K-793S07
	
	r = random.randint(0, 4)
	k = temp[:r] + "K" + temp[r:r+(4-r)] + "-"
	temp = temp[4:]
	
	r = random.randint(0, 5)
	k += temp[:r] + "L" + temp[r:r+(5-r)] + "-"
	temp = temp[5:]
	
	r = random.randint(0, 2)
	k += temp[:r] + "K" + temp[r:r+(2-r)] + "-"
	temp = temp[2:]
	
	r = random.randint(0, 5)
	k += temp[:r] + "S" + temp[r:r+(5-r)]
	
	#Mangle key1
	#print k
	return k
	
def getKey():
	global threadLock, key
	threadLock.acquire()
	k = mangleKey(key)
	threadLock.release()
	return k
	
def getMessage():
	global threadLock
	threadLock.acquire()
	m = encMsg
	threadLock.release()
	return m
	
def tcpWorkerThread():
	server = SocketServer.TCPServer(("0.0.0.0", 60701), MyTCPHandler)
	server.serve_forever()

def keyRotateThread():
	global threadLock, lastTime, rotateTime
	while 1:
		new_time = time.time()
		#Is it time to rotate key?
		if lastTime == None or (new_time - lastTime > rotateTime):
			lastTime = new_time
			threadLock.acquire()
			generateKey()
			encryptMessage()
			threadLock.release()
	
class MyTCPHandler(SocketServer.BaseRequestHandler):
	def handle(self):
		threadLock.acquire()
		Log("Send EncMsg : %s" % self.client_address[0])
		self.request.send(encMsg)
		threadLock.release()
		
# Restrict to a particular path.
class RequestHandler(SimpleXMLRPCRequestHandler):
    rpc_paths = ('/KLKSRPC',)

if __name__ == "__main__":

	#RandomNumberGenerator
	rng = random.Random()
	rng.seed()
	
	threadLock = thread.allocate_lock()
	logLock = thread.allocate_lock()
	
	#Startup message
	Log("Server starting up")
	
	#Key management thread
	thread.start_new_thread(keyRotateThread,())
	
	# Create socket listening thread
	thread.start_new_thread(tcpWorkerThread,())
	
	# Create server
	server = SimpleXMLRPCServer(("0.0.0.0", 60702), logRequests=False, requestHandler=RequestHandler)
	server.register_introspection_functions()

	# Register functions
	server.register_function(getKey, "getKey")
	server.register_function(getMessage, "getMessage")

	server.serve_forever()
	