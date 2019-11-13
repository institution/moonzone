t=lambda x,y: (type(x),type(y))
def mul(x,y):
	if t(x,y) == (int,int):
		return x*y
	if t(x,y) == (int,str):
		if x==0: return 0
		if x==1: return y
		return '({}*{})'.format(x,y)
	if t(x,y) == (str,int):
		if y==0: return 0
		if y==1: return x
		return '({}*{})'.format(x,y)
	if t(x,y) == (str,str):
		return '({}*{})'.format(x,y)

def add(x,y):
	if t(x,y) == (int,int):
		return x+y
	if t(x,y) == (int,str):
		if x==0: return y
		return '({}+{})'.format(x,y)
	if t(x,y) == (str,int):
		if y==0: return x
		return '({}+{})'.format(x,y)
	if t(x,y) == (str,str):
		return '({}+{})'.format(x,y)

def matmul(A,B):
	# A: MxN
	# B: NxK
	M=len(A)
	N=len(A[0])
	assert N==len(B)
	K=len(B[0])
	C=[[-1 for k in range(K)] for m in range(M)]
	for m in range(M):
		for k in range(K):
			s = 0
			for i in range(N):
				s = add(s, mul(A[m][i], B[i][k]))
			C[m][k]=s
	return C
def matmulvec(A,B):
	# A: MxN
	# B: M
	M=len(A)
	N=len(A[0])
	assert M==len(B)
	C=[-1 for _ in range(M)]
	for m in range(M):
		s=0
		for i in range(N):
			s=add(s, mul(A[m][i], B[i]))
		C[m]=s
	return C

def pp(A):
	for a in A:
		print('| ', ', '.join(map(str,a)) )
	print()


A=[
	['A0','A1','A2'],
	['A3','A4','A5'],
	[   0,   0,   1],
]
B=[
	['B0','B1','B2'],
	['B3','B4','B5'],
	[   0,   0,   1],
]
print('')
pp(matmul(A,B))


print('rot')
A=[
	['A0','A1','A2'],
	['A3','A4','A5'],
	[   0,   0,   1],
]
B=[
	['c', '-s',   0],
	['s',  'c',   0],
	[   0,   0,   1],
]
pp(matmul(A,B))

print('mov')
A=[
	['A0','A1','A2'],
	['A3','A4','A5'],
	[   0,   0,   1],
]
B=[
	[1,0,'x'],
	[0,1,'y'],
	[0,0,  1],
]
pp(matmul(A,B))


print('scl')
A=[
	['A0','A1','A2'],
	['A3','A4','A5'],
	[   0,   0,   1],
]
B=[
	['sx',0,0],
	[0,'sy',0],
	[0,0,   1],
]
pp(matmul(A,B))


print('scl rot mov')
A=[
	['s0',0,0],
	[0,'s1',0],
	[0,   0,1],
]
B=[
	['r0', '-r1',   0],
	['r1',  'r0',   0],
	[   0,     0,   1],
]
C=[
	[1,  0, 'p0'],
	[0,  1, 'p1'],
	[0,  0,    1],
]
D=[
	[1,  0, 'd0'],
	[0,  1, 'd1'],
	[0,  0,    1],
]
pp(matmul(matmul(matmul(A,B),C),D))


print('rot2')
A=[
	['r0', '-r1',   0],
	['r1',  'r0',   0],
	[   0,     0,   1],
]
B=[
	['d0', '-d1',   0],
	['d1', 'd0',   0],
	[   0,   0,   1],
]
pp(matmul(A,B))



print('mov local')
A=[
	['r0s0', '-r1s0', 0],
	['r1s1',  'r0s1', 0],
	[     0,       0, 1],
]
B=[
	['x0'],
	['x1'],
	[1],
]
pp(matmul(A,B))
