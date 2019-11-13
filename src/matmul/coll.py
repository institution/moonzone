"""
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
"""
import pygame, sys
from pygame.locals import *
from pygame.math import Vector2 as V
import math
W,H=800,600

# poly
class P:
	def __init__(s, vs):
		s.vs=vs
	def draw(s,w,c):
		pygame.draw.polygon(w,c,s.vs)

class AB:
	def __init__(s,p0,p1):
		s.x0=min(p0[0],p1[0])
		s.x1=max(p0[0],p1[0])
		s.y0=min(p0[1],p1[1])
		s.y1=max(p0[1],p1[1])
	def intersect(s,p):
		return s.x0<=p[0]<=s.x1 and s.y0<=p[1]<=s.y1


pygame.init()
win=pygame.display.set_mode((W,H),0,32)
pygame.display.set_caption('Coll')
win.fill((255,255,255))


def intersect_ss(a,b):
	A1=a[1][1]-a[0][1]
	B1=a[0][0]-a[1][0]
	C1=A1*a[0][0]+B1*a[0][1]

	A2=b[1][1]-b[0][1]
	B2=b[0][0]-b[1][0]
	C2=A2*b[0][0]+B2*b[0][1]

	det=A1*B2-A2*B1
	if det!=0:
		xy=((B2*C1-B1*C2)/det,(A1*C2-A2*C1)/det)
		if AB(*a).intersect(xy) and AB(*b).intersect(xy):
			return xy
	return None


def point_in_poly(t,poly):
	vs=poly.vs
	i,j=0,len(vs)-1
	r=False;
	while i<len(vs):
		vi=vs[i]
		vj=vs[j]
		if (vi[1]>t[1])!=(vj[1]>t[1]) and (t[0]<(vj[0]-vi[0])*(t[1]-vi[1])/(vj[1]-vi[1])+vi[0]):
			r=not r
		j=i
		i+=1
	return r

def intersect_ps(poly,seg):
	rs=[]
	vs=poly.vs
	i,j=0,(len(poly.vs)-1)
	while i<len(vs):
		e=intersect_ss(seg, (vs[i],vs[j]));
		if e:
			rs.append(e)
		j=i; i+=1
	return rs

def intersect_pp(p,q):
	rs=[]
	i,j=0,(len(p.vs)-1)
	while i<len(p.vs):
		p_seg=p.vs[i],p.vs[j]
		rs.extend(intersect_ps(q, p_seg));
		j=i; i+=1
	return rs


def cw_sort(vs):
	# find centroid
	cx=0
	cy=0
	for v in vs:
		cx+=v[0]
		cy+=v[1]
	cx/=len(vs)
	cy/=len(vs)
	# sort
	return sorted(vs, key=lambda v: math.atan2(v[1]-cy, v[0]-cx))




def draw_points(win,vs,c):
	pix = pygame.PixelArray(win)
	for x,y in vs:
		pix[x][y]=c
	#for j in range(H):
	#	for i in range(W):
	#		if point_in_poly((i,j),q):
	#			pix[i][j]=c
	del pix

def main():
	p=P([(146, 0), (291, 106), (236, 277), (56, 277), (0, 106)])
	q=P([(50,50),(300,50),(300,120),(50,110)])


	q.draw(win,(0,0,255,200))
	p.draw(win,(0,255,0,200))

	rs=[]

	for t in q.vs:
		if point_in_poly(t,p): rs.append(t)
	for t in p.vs:
		if point_in_poly(t,q): rs.append(t)

	rs.extend(intersect_pp(p,q))

	rs=cw_sort(rs)

	r=P(rs)

	if len(r.vs)>=3:
		r.draw(win, (100,50,0))


main()


pygame.display.update()
while True:
	for event in pygame.event.get():
		if event.type == QUIT:
			pygame.quit()
			sys.exit()

