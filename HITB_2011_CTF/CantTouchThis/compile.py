import os
import sys
from subprocess import call
from prepare_source import PrepareSource

#Remove old shellcode & src file file
try:
	os.remove("shellcode.txt")
	os.remove("test.exe")
except:
	pass
	
#Junk source code
PrepareSource()

try:
	#Compile
	if os.name == "nt":
		call("vcvars32.bat & cl.exe /Od /Gz /nologo test.cpp & test.exe", shell=True)
	elif os.name == "nix":
		pass
	elif os.name == "mac":
		pass
	else:
		sys.exit(-1)
except OSError, e:
	print "Call error"
	sys.exit(-1)
	
#Check if shellcode file exists
sys.exit(0)