"""
Released under the MIT-license:

Copyright (c) 2010, Earl Marcus

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
import SocketServer
import codecs
import random
import base64

url_base = "http://google.com/robots.txt"	#Replace with flag
	
class EchoRequestHandler(SocketServer.BaseRequestHandler ):
	def setup(self):
		print self.client_address, 'connected!'

	def handle(self):
		rnd_sid = str(random.randrange(1382, 239783624))
		new_url = base64.encodestring(" SID:" + rnd_sid + " " + url_base + " SID:" + rnd_sid)
		new_url = codecs.encode(new_url, "rot13")
		self.request.send(new_url);
		self.request.close()

#server host is a tuple ('host', port)
server = SocketServer.ThreadingTCPServer(('', 42865), EchoRequestHandler)
server.serve_forever()
