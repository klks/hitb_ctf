import SocketServer
import codecs
import random
import base64
import time
import socket

flag = "FLAGFLAGFLAGFLAG"	#Replace with flag

class RequestHandler(SocketServer.BaseRequestHandler ):
	def setup(self):
		print self.client_address, 'connected!'
		self.request.send("OH HAI!!! YOU CAN HAS FLAG IN 5 SEKANDS (42527)");
		self.request.close()
		#Establish a connect back
		time.sleep(6)
		try:
			print "Sending connect back flag to " , self.client_address[0]
			s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
			s.connect((self.client_address[0], 42527))	#Port connect back will happen on
			s.send(flag)
			s.close()
		except:
			pass

#server host is a tuple ('host', port)
server = SocketServer.ThreadingTCPServer(('', 61345), RequestHandler)
server.serve_forever()
