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
import xmlrpclib
import socket
import struct

def is_luhn_valid(cc):
    num = [int(x) for x in str(cc)]
    return sum(num[::-2] + [sum(divmod(d * 2, 10)) for d in num[-2::-2]]) % 10 == 0
	
#Get encrypted message
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect(("127.0.0.1", 60701))
eMsg = sock.recv(1024)
sock.close()

print "Encrypted message size : " + str(len(eMsg))

#Get key
s = xmlrpclib.ServerProxy('http://localhost:60702/KLKSRPC')
key = s.getKey()

print "Key from server : " + key

cleanKey = ""
for k in key:
	if k.isdigit():
		cleanKey += k

#Clean Key
print "CleanKey : " + cleanKey

#Verify Key
if is_luhn_valid(cleanKey):
	print "Key : Valid"
else:
	print "Key : Invalid"
	
#Decrypt message
dMsg = ""
i = 0
for m in eMsg:
	if i == 16:
		i = 0
	k = cleanKey[i]
	dMsg += struct.pack("<B", ord(m) ^ ord(k))
	i += 1
	
print "Decryped Msg : " + dMsg