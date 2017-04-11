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
# Ref
# http://en.wikipedia.org/wiki/Befunge
# http://packages.python.org/PyFunge/index.html
# http://github.com/programble/befungee

import sys
import os
import time
import random

opcode_matrix = None		#Holds whatever
app_stack = []
cur_pos = None
cur_dir = None				#Keeps track of current direction (X[LeftRight],Y[UpDown])

app_debug = False

class VMError(Exception):
	def __str__(self):
		return repr(self.value)


#Check if we can move to a specific location within opcode_matrix
def VMNextDirection():
	global cur_pos, cur_dir
	
	if app_debug: print "CurPos : X:%d,Y:%d -" % (cur_pos[0], cur_pos[1]),
	
	if cur_dir == ">":
		#Check if we need to wrap around
		if cur_pos[0]+1 >= len(opcode_matrix[cur_pos[1]]):
			cur_pos[0] = 0
		else:
			cur_pos[0] += 1
	elif cur_dir == "<":
		if 0 > cur_pos[0]-1:
			cur_pos[0] = len(opcode_matrix[cur_pos[1]])-1
		else:
			cur_pos[0] -= 1
	elif cur_dir == "^":
		if 0 > cur_pos[1]-1:
			cur_pos = len(opcode_matrix)
		else:
			cur_pos[1] -= 1
		#Check if X exists in the new line, if not keep going up
			if cur_pos[0] >= len(opcode_matrix[cur_pos[1]]):
				VMNextDirection()
	elif cur_dir == "v":
		if cur_pos[1]+1 >= len(opcode_matrix):
			cur_pos[1] = 0
		else:
			cur_pos[1] += 1
		#Check if X exists in the new line, if not keep going down
			if cur_pos[0] >= len(opcode_matrix[cur_pos[1]]):
				VMNextDirection()
		
	if app_debug: print "NewPos : X:%d,Y:%d" % (cur_pos[0], cur_pos[1])

def VMPopAB():
	global app_stack
	#Check stack length
	if len(app_stack) < 2:
		raise VMError("VMPopAB : Not enough elements on stack to pop")
	A = app_stack.pop()
	B = app_stack.pop()
	
	return (A,B)
	
def VMPop():
	global app_stack
	if len(app_stack) == 0: return 0
	return app_stack.pop()
	
def VMPush(val):
	global app_stack
	app_stack.append(val)
	
def VMGetInstr():
	global opcode_matrix, cur_pos
	x,y = cur_pos
	return opcode_matrix[y][x]
	
def file_exists(filename):
	if not os.path.exists(filename):
		print "Make sure %s exists" % filename
		sys.exit(1)
		
def PrintStack():

	for item in app_stack:
		print hex(ord(item))
	
def StackSize():
	return " Stack_Size(%d) Direction(%s)" % (len(app_stack), cur_dir)
	
if __name__ == "__main__":
	if len(sys.argv) != 2:
		print "Usage : parser.py <input_file>"
		sys.exit(1)
	
	#Set X/Y to 0,0	
	cur_pos = [0,0]
	
	#Set direction to >
	cur_dir = ">"
	
	input_file = sys.argv[1]

	#Check if input file can be read
	file_exists(input_file)
	
	#Read everything into a multidimensional array
	f = open(input_file)
	opcode_matrix = f.read().split("\n")
	f.close()
	
	if app_debug:
		for line in opcode_matrix:
			print line
			
	VMRet = None
	VMStringStart = False
	
	#Interprete
	while(1):
		instr = VMGetInstr()
		if app_debug: print "Interpreting : " + instr
		
		if VMStringStart:
			if instr != "\"":
				VMPush(ord(instr))
			else:
				VMStringStart = not VMStringStart	
			if app_debug: print "VMStr" + StackSize()
			
		elif instr in "0123456789":					#0-9 	Push this number on the stack
			VMPush(int(instr))
			if app_debug: print "VMPush " + instr + StackSize()
			
		elif instr == "+":							#Addition: Pop a and b, then push a+b
			a,b = VMPopAB()
			VMPush(a+b)
			if app_debug: print "VMAdd" + StackSize()
			
		elif instr == "-":							#Subtraction: Pop a and b, then push b-a
			a,b = VMPopAB()
			VMPush(b-a)
			if app_debug: print "VMSub" + StackSize()
			
		elif instr == "*":							#Multiplication: Pop a and b, then push a*b
			a,b = VMPopAB()
			VMPush(a*b)
			if app_debug: print "VMMul" + StackSize()
			
		elif instr == "/":							#Integer division: Pop a and b, then push b/a, rounded down. If a is zero, ask the user what result they want.
			a,b = VMPopAB()
			if a == 0:
				raise VMError("VMDiv : Division by 0")
			VMPush(b/a)
			if app_debug: print "VMDiv" + StackSize()
			
		elif instr == "%":							#Modulo: Pop a and b, then push the remainder of the integer division of b/a. If a is zero, ask the user what result they want.
			a,b = VMPopAB()
			if a == 0:
				raise VMError("VMMod : Division by 0")
			VMPush(b%a)
			if app_debug: print "VMMod" + StackSize()
			
		elif instr == "!":							#Logical NOT: Pop a value. If the value is zero, push 1; otherwise, push zero.
			VMRet = VMPop()
			if VMRet == 0:
				VMPush(1)
			else:
				VMPush(0)
			if app_debug: print "VMNot" + StackSize()
			
		elif instr == "`":							#Greater than: Pop a and b, then push 1 if b>a, otherwise zero.
			a,b = VMPopAB()
			if b>a:
				VMPush(1)
			else:
				VMPush(0)
			if app_debug: print "VMGrTh" + StackSize()
				
		elif instr == ">":							#Start moving right
			cur_dir = ">"
			if app_debug: print "VMMoveRight" + StackSize()
			
		elif instr == "<":							#Start moving left
			cur_dir = "<"
			if app_debug: print "VMMoveLeft" + StackSize()
			
		elif instr == "^":							#Start moving up
			cur_dir = "^"
			if app_debug: print "VMMoveUp" + StackSize()
			
		elif instr == "v":							#Start moving down
			cur_dir = "v"
			if app_debug: print "VMMoveDown" + StackSize()
			
		elif instr == "?":							#Start moving in a random cardinal direction
			cur_dir = ['>', 'v', '<', '^'][random.randint(0, 3)]
			if app_debug: print "VMMoveRandom" + StackSize()
			
		elif instr == "_":							#Pop a value; move right if value=0, left otherwise
			VMRet = VMPop()
			if VMRet == 0:
				cur_dir = ">"
			else:
				cur_dir = "<"
			if app_debug: print "VMPopLeft" + StackSize()
			
		elif instr == "|":							#Pop a value; move down if value=0, up otherwise
			VMRet = VMPop()
			if VMRet == 0:
				cur_dir = "v"
			else:
				cur_dir = "^"
			if app_debug: print "VMPopUp" + StackSize()
			
		elif instr == "\"":							#Start string mode: push each character's ASCII value all the way up to the next "
			VMStringStart = not VMStringStart
			if app_debug: print "VMStr" + StackSize()
			
		elif instr == ":":							#Duplicate value on top of the stack
			VMRet = VMPop()
			VMPush(VMRet)
			VMPush(VMRet)
			if app_debug: print "VMDup" + StackSize()
			
		elif instr == "\\":							#Swap two values on top of the stack
			a,b = VMPopAB()
			VMPush(a)
			VMPush(b)
			if app_debug: print "VMSwap" + StackSize()
			
		elif instr == "$":							#Pop value from the stack
			VMPop()
			if app_debug: print "VMPop" + StackSize()
			
		elif instr == ".":							#Pop value and output as an integer
			VMRet = VMPop()
			sys.stdout.write(str(int(VMRet)))
			if app_debug: print "VMPopInt" + StackSize()
			
		elif instr == ",":							#Pop value and output as ASCII character
			VMRet = VMPop()
			sys.stdout.write(chr(VMRet))
			if app_debug: print "VMPopASC" + StackSize()
			
		elif instr == "#":							#Trampoline: Skip next cell
			VMNextDirection()
			if app_debug: print "VMSkip" + StackSize()
			
		elif instr == "p":							#A "put" call (a way to store a value for later use). Pop y, x and v, then change the character at the position (x,y) in the program to the character with ASCII value v
			raise VMError("VMPut : Not Implemented")
			if app_debug: print "VMPut" + StackSize()
			
		elif instr == "g":							#A "get" call (a way to retrieve data in storage). Pop y and x, then push ASCII value of the character at that position in the program
			raise VMError("VMGet : Not Implemented")
			if app_debug: print "VMGet" + StackSize()
			
		elif instr == "&":							#Ask user for a number and push it
			x = raw_input()
			try:
				VMPush(int(x))
			except ValueError:
				VMPush(0)
			if app_debug: print "VMInputInt" + StackSize()
			
		elif instr == "~":							#Ask user for a character and push its ASCII value
			x = sys.stdin.read(1)
			VMPush(ord(x))
			if app_debug: print "VMInputChr" + StackSize()
			
		elif instr == "@":							#End program
			if app_debug: print "VMEnd" + StackSize()
			break
			
		else:
			#print "Unknown instr, skipping" + StackSize()
			pass
		VMNextDirection()
		#time.sleep(0.001)