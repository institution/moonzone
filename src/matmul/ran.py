seed=0;

m=2^31-1
a=48271
c=0

def rand():
	global seed
	seed=(a*seed+c)%m;
	return seed;

for x in range(20):
	print(rand())
