#!/usr/bin/env python
# gopherd.py - A Simple Gopher Server
# By Maxin B. John (maxinbjohn@gmail.com)
#
# This little program illustrates the gopher protocol implementation
# using python with the help of SocketServer module.
#
# The gopher protocol is a very simple TCP-based protocol;which listen
# on port number 70.The Internet Gopher protocol is designed for
# distributed document search and retrieval.
#
# The Internet Gopher protocol is designed primarily to act as a
# distributed document delivery system.  While documents (and services)
# reside on many servers, Gopher client software presents users with a
# hierarchy of items and directories much like a file system. 

#To secure this setup, some stuff will be needed,
#Listing of directories is not allowed
# Jailed to "files_served" folder/directory
#Normalizing input recieved

import SocketServer
import string
import os
import thread
from time import localtime
import unicodedata

logLock  = None

valid_chars = ".%s%s" % (string.ascii_letters, string.digits)

def removeDisallowedFilenameChars(filename):
	return ''.join(c for c in filename if c in valid_chars)

def CommandLog(msg):
	global logLock
	logLock.acquire()
	log = open("UserCommands.txt", 'a')
	log.write("[%.2d:%.2d] " % localtime()[3:5])
	log.write(msg + "\n")
	log.close()
	print msg.strip("\n")
	logLock.release()
	
class GopherHandler(SocketServer.StreamRequestHandler):
	def handle(self):
		# Read a line of text, limiting it to 512 bytes.
		# This will prevent someone trying to crash the server machine
		# by sending megabytes of data.
		searchstring=self.rfile.readline(512)
		CommandLog("%s : %s" % (self.client_address[0], searchstring))
		# Remove any leading and trailing whitespace, including the
		# trailing newline.
		searchstring=string.strip(searchstring)
		
		#Normalize searchstring
		searchstring = removeDisallowedFilenameChars(searchstring)
		
		# invoking the get_directory_info to get the return string
		info = self.get_directory_info(searchstring)
		# send the data back to gopher client
		self.wfile.write(info)
    
	# The following method takes a string containing the searchstring
	# This function will search the directory and provide the information
	# in Gopher Protocol format. Later will be expanded to contain the searching
	# logic based on the searchstring.

	def get_directory_info(self, searchstring):
		info = ""
		dir_path= os.getcwd() + "/files_served/"
		
		#Testing whether the request is for a particular file or folder
		#if (searchstring == "" or os.path.isdir(searchstring)== 1):
		if (searchstring == ""):
			#Return a string containing the file and directory information in gopher protocol format.
			file_list= os.listdir(dir_path)
			for fileName in file_list:
				if os.path.isfile(fileName) == 1:
					info += "0" + fileName + "\t" + fileName + "\thitbctf.jail\t70\r\n"
				elif os.path.isdir(fileName) == 1:
					info += "1" + fileName + "\t" + fileName + "\thitbctf.jail\t70\r\n"
				else:
					info += "2" + fileName + "\t" + fileName + "\thitbctf.jail\t70\r\n"
			return "%s\r\n" % (info,)
		else:
			fileName = searchstring
			CommandLog("Searching for : %s" % (dir_path + fileName,))
			if os.path.isfile(dir_path + fileName) == 1:
				info = self.handle_file(dir_path + fileName)
			else:
				info = "nothing"
			return "%s\r\n" % (info,)

	# Function to handle the request for a file
	def handle_file(self, fileName):
		try:
			CommandLog("Opening : %s" % (fileName,))
			fd = open(fileName,"r")
			textcontent = fd.read() + "\r\n"   
		except:
			textcontent = "nothing"
		return "%s \r\n" %(textcontent,)
       
# If this script is being run directly, it'll start acting as a gopher
# daemon. The following "if" statement is the usual Python style for
# running code only as a stand alone script.

if __name__== "__main__":
	logLock = thread.allocate_lock()
	
	try:
		# Create an instance of our server class
		server = SocketServer.ThreadingTCPServer( ("", 70), GopherHandler)
		# Enter an infinite loop, waiting for requests and then servicing them.
		server.serve_forever()

	# For graceful exit when ^c is used. 
	except KeyboardInterrupt:
		print "Received Keyboard interrput: Exiting..."
		exit()