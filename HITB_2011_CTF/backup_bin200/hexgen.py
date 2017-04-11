import random

print "unsigned char buffer[] = {",

for i in xrange(256):
	print hex(random.randint(0,4294967295)) + ",",

print "};"