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
import sys
import os
import socket
import time
import struct
import pcap
import string

#udp port 43450

p = None
HOST = None

protocols={socket.IPPROTO_TCP:'tcp',
           socket.IPPROTO_UDP:'udp',
           socket.IPPROTO_ICMP:'icmp'}

def dumphex(s):
	bytes = map(lambda x: '%.2x' % x, map(ord, s))
	for i in xrange(0,len(bytes)/16):
		print '    %s' % string.join(bytes[i*16:(i+1)*16],' ')
	print '    %s' % string.join(bytes[(i+1)*16:],' ')

def decode_ip_packet(s):
	d={}
	d['version']=(ord(s[0]) & 0xf0) >> 4
	d['header_len']=ord(s[0]) & 0x0f
	d['tos']=ord(s[1])
	d['total_len']=socket.ntohs(struct.unpack('H',s[2:4])[0])
	d['id']=socket.ntohs(struct.unpack('H',s[4:6])[0])
	d['flags']=(ord(s[6]) & 0xe0) >> 5
	d['fragment_offset']=socket.ntohs(struct.unpack('H',s[6:8])[0] & 0x1f)
	d['ttl']=ord(s[8])
	d['protocol']=ord(s[9])
	d['checksum']=socket.ntohs(struct.unpack('H',s[10:12])[0])
	d['source_address']=pcap.ntoa(struct.unpack('i',s[12:16])[0])
	d['destination_address']=pcap.ntoa(struct.unpack('i',s[16:20])[0])
	if d['header_len']>5:
		d['options']=s[20:4*(d['header_len']-5)]
	else:
		d['options']=None
	d['data']=s[4*d['header_len']:]
	return d

def ParseVictim(pktlen, data, timestamp):
	global HOST
	if not data:
		return
	if data[12:14]=='\x08\x00':
		decoded=decode_ip_packet(data[14:])
	print '\n%s.%f %s > %s' % (time.strftime('%H:%M', time.localtime(timestamp)), timestamp % 60, decoded['source_address'], decoded['destination_address'])
	for key in ['version', 'header_len', 'tos', 'total_len', 'id', 'flags', 'fragment_offset', 'ttl']:
		print '  %s: %d' % (key, decoded[key])
	print '  protocol: %s' % protocols[decoded['protocol']]
	print '  header checksum: %d' % decoded['checksum']
	print '  data:'
	dumphex(decoded['data'][8:])
	HOST = decoded['source_address']

	#Dump encrypted flag to file
	f = open("encPacket.dmp", "wb")
	f.write(decoded['data'][8:])
	f.close()

def WaitForVictim():
	global p, HOST

	while HOST == None:
		p.dispatch(1, ParseVictim)

def BruteTalkKey(IP,PORT):
	for x in xrange(0,256):
		send_data = struct.pack("BBB", x, 0, 0)
		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		s.connect((IP,PORT))
		s.send(send_data)
		data = s.recv(1024)
		s.close()
		if len(data) == 4:
			print "bTalkKey = %X" % x
			return x

if __name__ == "__main__":

	if len(sys.argv) < 2:
		print "usage: solution.py <interface>"
		sys.exit(0)

	#setup sniffer
	dev = sys.argv[1]
        p = pcap.pcapObject()
        net, mask = pcap.lookupnet(dev)
        p.open_live(dev, 1600, 0, 100)
        p.setfilter("udp port 43450", 0, 0)

	PORT = 17999              # The same port as used by the server

	while 1:
		HOST = None
		WaitForVictim()

		print "HOST : %s" % HOST
		bTalkKey = BruteTalkKey(HOST,PORT)
		full_key = ""
		for bytes in xrange(8):
			cur_bits = ""
			for bits in xrange(8):
				send_data = struct.pack("BBB", bTalkKey, bytes, bits)
				s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
				s.connect((HOST, PORT))
				s.send(send_data)
				data = s.recv(1024)
				if len(data) == 4:
					cur_bits += str(ord(data[3]))
				s.close()
			print cur_bits
			full_key += chr(int(cur_bits, 2))

		print "lSeed = %s" % full_key
		#Call the decoder program
		os.system("./solution_pt2 %s encPacket.dmp" % full_key)
		#sys.exit(1)

