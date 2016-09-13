#!/usr/bin/python
import sys
import math

FFT_SIZE=1024
sys.stdout.write('static const uint16_t hammingWindow['+str(FFT_SIZE)+']={');
for i in xrange(FFT_SIZE):
	if i != 0:	
		sys.stdout.write(',')
	w = 0.52836-0.46164*math.cos(2*math.pi*i/(FFT_SIZE-1))
	w = w * 0x7FFF
	sys.stdout.write(str(int(round(w))))
sys.stdout.write('};')