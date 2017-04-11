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
import sys, os
import time
import thread
import base64
from subprocess import call
from prepare_source import PrepareSource

CTF_DEBUG = True
threadLock = None
logLock = None
lastTime = None
mem_shellcode = None
timer_limit = 15

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
	
def GenerateNewShellcode():
	global mem_shellcode, lastTime, timer_limit

	badboy_shell = base64.b64encode("\xCC")
	
	#If <15s since last call, return old results
	if lastTime != None and (time.time() - lastTime < timer_limit):
		Log("Still in %d timer limit, returning prev shellcode - len[%d]" % (timer_limit, len(mem_shellcode)))
		return mem_shellcode
		
	Log("Generating new shellcode")
	
	#Remove old shellcode & src file file
	try:
		os.remove("shellcode.txt")
		os.remove("test.exe")
	except:
		pass
	
	if CTF_DEBUG == False:
		#Junk source code
		PrepareSource()

	try:
		#Compile
		if os.name == "nt":
			if CTF_DEBUG == False:
				call("vcvars32.bat & cl.exe /Od /Gz /nologo test.cpp & test.exe", shell=True)
			else:
				call("vcvars32.bat & cl.exe /Od /Gz /nologo src\\test.cpp & test.exe", shell=True)
		elif os.name == "nix":
			pass
		elif os.name == "mac":
			pass
		else:
			Log("Unknown Platform Error, returning badboy shellcode")
			return badboy_shell
			
	except OSError, e:
		Log("Compile Error, returning badboy shellcode")
		return badboy_shell
	
	f = open("shellcode.txt", "r")
	mem_shellcode = base64.b64encode(f.read())
	f.close()
	
	#Update last time
	lastTime = time.time()
	return mem_shellcode
	
def CantTouchThis():
	global threadLock
	
	threadLock.acquire()
	Log("New Request Recieved")
	shellcode = GenerateNewShellcode()
	threadLock.release()
	return shellcode
	
# Restrict to a particular path.
class RequestHandler(SimpleXMLRPCRequestHandler):
    rpc_paths = ('/KLKSRPC',)
	
if __name__ == "__main__":
	threadLock = thread.allocate_lock()
	logLock = thread.allocate_lock()
	
	#Startup message
	Log("CantTouchThis Server starting up...")
	
	# Create server
	server = SimpleXMLRPCServer(("0.0.0.0", 60888), logRequests=False, requestHandler=RequestHandler)
	server.register_introspection_functions()

	# Register functions
	server.register_function(CantTouchThis, "CantTouchThis")
	
	server.serve_forever()
	