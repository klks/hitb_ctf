"""
Released under the MIT-license:

Copyright (c) 2009,2010 Earl Marcus

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
import sys
import socket
import time
import random
import thread
from threading import Thread
from Queue import Queue

time_min = 1

def_IPList = []

#Used for threading
ip_queue = Queue()
printLock  = None

#Generate random packet w/ size (anywhere between 30-1024)
def GeneratePacket():
	size = random.randint(30,256)
	packet = ""
	for i in xrange(size):
		packet += chr(random.randint(0,255))
	return (packet,size)

def SpamThread(ip):
	global ports
	for port in ports:
		packet, size = GeneratePacket()
		#Spam
		printStr("Spamming %s:%s with data size %d" % (ip, port, size))
		try:
			s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
			s.connect((ip, int(port)))
			s.send(packet)
			s.close()
		except:
			printStr("Failed on %s port %s" % (ip, port))

def printStr(string):
	global printLock
	printLock.acquire()
	print string
	printLock.release()

def workerThread(q):
	ip = q.get()
	while 1:
		SpamThread(ip)
		#time.sleep(time_min*60)
                time.sleep(30)
	q.task_done()

if __name__ == "__main__":
	random.seed()
	
	#Load IP's
	f = open("ips.txt")
	ips = f.read().split()
	f.close()

	#Load Ports
	f = open("ports.txt")
	ports = f.read().split()
	f.close()

	#Acquire locks
	printLock = thread.allocate_lock()
		
	#Start kicking off threads
	for i in xrange(10):
		worker = Thread(target=workerThread, args=(ip_queue,))
		worker.setDaemon(True)
		worker.start()

	for ip in ips:
		ip_queue.put(ip)
		
	ip_queue.join()
