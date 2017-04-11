"""
Released under the MIT-license:

Copyright (c) 2010 Earl Marcus

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
import socket
import random
import os

#Return Values
CTF_SUCCESS	=	0
CTF_FAIL	=	-1
CTF_INTERR	=	-2

def pretty(array):
	val = []
	for i in array:
		print ord(i),
		
	print ""

def validate_daemon(ip,port,valid_flag):
	#print "Trying to connect to %s:%s" % (ip, port)
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	try:
		s.connect((ip,port))
		buffer_length = random.randint(40,60)
		buff = []
		buff.append(buffer_length)
		for i in range(buffer_length):
			buff.append(random.randint(0,255))
		
		buff[2] = 0xDE
		buff[6] = 0x7E
		
		packed_send = ''.join([chr(i) for i in buff])
		print buff
		#Checksum before
		checksum_send = 0
		for i in buff: 
			checksum_send += i
		
		s.send(packed_send)
		returned_buffer = s.recv(1024)
		s.close()
		print returned_buffer
		
		#Checksum after
		checksum_recv = 0
		for i in returned_buffer:
			checksum_recv += ord(i)
			
		if checksum_send != checksum_recv:
			print "FAIL"
			return CTF_FAIL
		
		print "PASS"
		return CTF_SUCCESS
	except Exception, e:
		raise
	finally:
		s.close()
		
	return CTF_FAIL
	
def exploit_daemon(ip,port):
	return CTF_INTERR
	
	
if __name__ == '__main__':
	validate_daemon("172.16.122.132",17999,"0")