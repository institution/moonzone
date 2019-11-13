A
B
possible speparating axes = sum edges A, edges B

def is_collision
	min_overlap
	take each axis a
		project A and B onto a
		if projections overlap
			min_overlap = min(min_overlap, overlap)
		else:
			return separating axis a

	return min_overlap





# distance for collision handling
import math
from math import sqrt
import numpy as np
import random
import scipy.misc as smp

# Create a 1024x1024x3 array of 8 bit unsigned integers
data = np.zeros( (500,500,3), dtype=np.float )

#-a
#//-sqrt(a*a)

class S:
	def __init__(self,p,a,b):
		self.p=p
		self.ha=a/2
		self.hb=b/2
	def d(self,q):
		x=q[0]-self.p[0]
		y=q[1]-self.p[1]
		d=pow(x/self.ha,4) + pow(y/self.hb,4)
		return d
	def dd(self,q):
		x=q[0]-self.p[0]
		y=q[1]-self.p[1]
		dd=(4*pow(x,3)/pow(self.ha,4), 4*pow(y,3)/pow(self.hb,4))
		l=(abs(dd[0])+abs(dd[1]))**(1/4)
		return dd[0]/l, dd[1],l





ss=[]
ss.append(S((100,100),20,40))
ss.append(S((300,400),70,30))


for y in range(500):
	for x in range(500):
		v=min(s.d((x,y)) for s in ss)
		if v<1.0:
			data[y,x]=1.0
		else:
			data[y,x]=0.0
		data[y,x]=v


def add(q,p):
	return q[0]+p[0], q[1]+p[1]

def neg(q):
	return -q[0], -q[1]


# iter
x=200
y=130
for i in range(800):
	data[int(y),int(x)]=0.5
	dd=neg(add(ss[0].dd((x,y)), ss[1].dd((x,y))))
	print(dd)
	x+=dd[0]*0.01
	y+=dd[1]*0.01

data[int(y),int(x)]=1


img = smp.toimage(data)       # Create a PIL image
img.show()                      # View in default viewer
