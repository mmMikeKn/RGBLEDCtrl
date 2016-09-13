#!/usr/bin/python
import sys
import math
import random
import colorsys
from PIL import Image, ImageDraw

sys.stdout.write('static const uint8_t fireColors[256][3]={')
colors = []
for i in xrange(256):	
	c = colorsys.hls_to_rgb(i/255.0/6, min(1.0, i*0.8/255), 1.0)
	c_rgb = (int(c[0]*255.0), int(c[1]*255.0), int(c[2]*255.0))
	sys.stdout.write('{%d,%d,%d}, ' % c_rgb)
#	sys.stdout.write('/*'+str(i)+'*/')
	colors.append(c_rgb)
sys.stdout.write('};')

color = (0, 0, 0)
img = Image.new('RGB', (256*2, 500), color)
dr = ImageDraw.Draw(img)
for i in xrange(len(colors)):
	dr.rectangle((i*2,0,i*2+2,100), fill=colors[i])

ofs = 128
for i in xrange(1, 80):
	for j in xrange(1,i):	        
		fireLevel = int(round(255 - j*255.0/i))
		if j > 4:
			 fireLevel=fireLevel+ 10-random.randint(0,20)
		if fireLevel > 255:
			fireLevel=255
		if fireLevel < 0:
			fireLevel=0
                c_rgb = colors[fireLevel];
		dr.rectangle( ((ofs+j-1)*2, 101+i*3, (ofs+j)*2+2, 101+i*3+3), fill=c_rgb)
		dr.rectangle( ((ofs-j-1)*2, 101+i*3, (ofs-j)*2+2, 101+i*3+3), fill=c_rgb)
	

img.save("fire.png")