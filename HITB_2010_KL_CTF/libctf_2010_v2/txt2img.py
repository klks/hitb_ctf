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
#References
#http://www.rkblog.rk.edu.pl/w/p/django-and-captcha-images/
#http://tophattaylor.blogspot.com/2009/06/python-captcha.html
#http://blog.objectgraph.com/index.php/2007/01/19/pil-on-captcha-transparency-and-masking-for-captcha/
import Image,ImageDraw,ImageFont,ImageOps
import os
import sys
import random
import string

# ----------------------- Settings -----------------------------------------
width = 2000
height = 80
charNum = 4
fontSize = 36
chImgW = fontSize+10
chImgH = fontSize+10
lineNum = random.randint(2,12)
font_path = ".\\Hand Writting Font Pack\\"
font = [ImageFont.truetype(font_path+"MLSJN.TTF",fontSize),
		ImageFont.truetype(font_path+"ROYFRG__.TTF",fontSize),
		ImageFont.truetype(font_path+"ALMAGRO_.TTF",fontSize)
		]
		
if __name__ == "__main__":
	if len(sys.argv) != 2:
		print "Usage : txt2img.py <input_txt>"
		sys.exit(1)
		
	flag_txt = sys.argv[1]
	
	charNum = len(flag_txt)
	interval =  width/charNum
	
	#Seed prng
	random.seed(None)
	
	# Create a background image
	image = Image.new('RGB', (width, height), (220,210,190))
	draw = ImageDraw.Draw(image)
	
	i=0
	for c in flag_txt:
		# Create a small image to hold one character. Background is black
		charImg = Image.new('RGB',(chImgW, chImgH),(0,0,0))
		tmpDraw = ImageDraw.Draw(charImg)
		
		# Draw text on this image
		tmpDraw.text((3, 1), c, font = font[random.randint(0,len(font)-1)], fill = (random.randint(20,150), random.randint(20,140), random.randint(160,200)))
		
		# Rotate a little bit, do some trick if you want
		charImg = charImg.rotate(random.randint(-30,30))
		
		# Create a mask which is same size of the small image
		mask = Image.new('L',(chImgW, chImgH),0) 
		mask.paste(charImg,(0,0))
	
		# Generate Random X Y
		hpos = 10 + (i*interval + random.randint(10,interval-10))
		vpos = random.randint(10, 20)
		
		# Paste twice for the visibility
		image.paste(charImg,(hpos,vpos),mask)
		image.paste(charImg,(hpos,vpos),mask)

		i += 1
	
	# ----------------------- Draw a couple of lines -----------------------------------------
	for i in range(0,lineNum):
		draw.line(
				(random.randint(6,width-6),random.randint(3,height-3),random.randint(6,width-6),random.randint(3,height-3)),
				fill = (random.randint(80,150), random.randint(80,220), random.randint(160,220))
				)
			
	#Greyscale
	image = ImageOps.grayscale(image)
	#Save image to files
	image.save(".\\flags\\"+flag_txt, "JPEG")