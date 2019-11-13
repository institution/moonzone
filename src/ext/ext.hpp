#pragma once
#include<unistd.h> //syscalls
#include<fcntl.h>  //file open flags
#include<cstdint>  //uint8_t..etc
#include<cstdlib>  //abort
#include<linux/limits.h> //PATH_MAX
//#include<signal.h> //raise(SIGTRAP)
#include<errno.h>
#include<cmath>
#include<new> // placement new
#include<typeinfo> // placement new
#define _TOSTR(x) #x
#define TOSTR(x) _TOSTR(x)
#define _CAT(x,y) x ## y
#define CAT(x,y) _CAT(x,y)
#define A(...) ((__VA_ARGS__)?void():fail("E " __FILE__ ":" TOSTR(__LINE__) " A(" #__VA_ARGS__ ")\n"))
#define AX(cond) A(cond)
#ifndef NDEBUG
 #define D(...) __VA_ARGS__
 #define TEST(...) inline static bool CAT(test,__COUNTER__){ [](){__VA_ARGS__ return 0;}() };
#else
 #define D(...) void()
 #define TEST(...)
#endif
#define AS static_assert
#define SA static_assert
#define R return
#define Inl inline
#define Tpl template
#define Cls class
#define C const
#define CE constexpr
#define el else if
#define th this->
#define DO(i,N) for(I4 i=0;i<(N);++i)  // do
#define OD(i,N) for(I4 i=(N);i--!=0;)  // od
//#define DR(i,B,E) for(I32 i=(B);i<(E);++i)  // do range
#define DE(p,l) for(auto *p=(l).ptr();p!=(l).end();++p)  // do each as ptr
#define UNUSED(x) ((x)==(x))
namespace ext{
	// primitive types
	using V=void;
	using H=char;
	using HC=char C;
	using B=bool;
	using I1=int8_t;
	using I2=int16_t;
	using I4=int32_t;
	using I8=int64_t;
	using U1=uint8_t;
	using U2=uint16_t;
	using U4=uint32_t;
	using U8=uint64_t;
	using F4=float; AS(sizeof(float)==4);
	using F8=double; AS(sizeof(double)==8);
	// len cstr
	Inl I4 len(HC *x){I4 i=0; while(*(x+i)) ++i; R i;}
	// perr
	Inl V perr(){
		I4 f=errno; H b[11]; I4 i=11;
		do{b[--i]=f%10, f/=10;}while(f);
		if(errno<0) b[--i]='-';
		::write(2,b,11-i);}
	// fail cstr [errno]
	[[noreturn]] Inl V fail(HC* msg=""){
		::write(2,msg,len(msg));
		if(errno) ::write(2,"E:errno:",8),perr(),::write(2,"\n",1);
		std::abort();}
	// A

	// safecast
	Inl I4 i4(I8 x){A(I8(I4(x))==x); R I4(x);}
	Inl I4 i4(U4 x){A(U4(I4(x))==x); R I4(x);}
	Inl I2 i2(I4 x){A(I4(I2(x))==x); R I2(x);}
	Inl U1 u1(F4 x){x=std::round(x); A(F4(U1(x))==x); R U1(x);}
	Inl I4 i4(F4 x){x=std::round(x); A(F4(I4(x))==x); R I4(x);}
	Inl F4 f4(U4 x){A(U4(F4(x))==x); R F4(x);}
	// rng
	Tpl<Cls T> struct Rng{
		T *p{0}; I4 l{0};
		Rng<T>(T *p, I4 l):p(p),l(l){A(0<=l);}
		Rng<T>(Rng<T> C&)=default;
		explicit Rng<T>()=default;
		T* ptr(){R p;}
		I4 len(){R l;}
		T* end(){R p+l;}
		T& operator[](I4 i){R D(A(p),A(0<=i),A(i<l)),p[i];}
		//T C& operator[](I32 i)C{AD(b!=0),AD(0<=i),AD(i<len());R b[i];}
		// calc slice index
		I4 csi(I4 i){R (i<0)?(A(-l<=i), l+i):(A(i<=l), i);}
		Rng<T> rng(){R *this;}
		Rng<T> rng(I4 i, I4 k){D(A(0<=i),A(0<=k),A(i+k<=l)); R {p+i,k};}
		Rng<T> rng(I4 i){D(A(0<=i),A(i<=l)); R {p+i,l-i};}
		Rng<T> sli(I4 i){I4 ci=csi(i); R rng(ci, l-ci);}
		Rng<T> sli(I4 i, I4 j){I4 ci=csi(i), cj=csi(j); D(A(ci<cj)); R rng(ci, cj-ci);}
	};
	Tpl<Cls T>Rng<T> rng(Rng<T> x, I4 i, I4 l){D(A(0<=i),A(i+l<=x.len())); R {x.ptr()+i,l};}
	// cp rng rng
	Tpl<class T> V cp(Rng<T> a, Rng<T> b){
		T *ap=a.ptr(),*a1=a.end(),*bp=b.ptr(),*b1=b.end();
		while (ap<a1 and bp<b1) *ap=*bp, ++ap, ++bp;
		D(A(bp==b1));}



	// rng =? rng
	Tpl<class T> B operator==(Rng<T> a, Rng<T> b){
		A(a.len()==b.len()); OD(i,a.len()) if(!(a[i]==b[i])) R 0; R 1;}
	// str <- cstr
	Inl Rng<H> str(HC* x){R {const_cast<H*>(x),len(x)};}
	// strz <- cstr
	Inl Rng<H> strz(HC* x){auto l=len(x); A(x[l]=='\0'); R {const_cast<H*>(x),l+1};}
	// cstr <- strz
	Inl HC* cstr(Rng<H> r){R A(*(r.end()-1)=='\0'), r.ptr();}
	// str =? cstr
	Inl B operator==(Rng<H> x, HC* y){
		H *xp=x.ptr(),*x1=x.end();
		while (xp<x1 and *y and *xp==*y) ++xp, ++y;
		R *xp==*y;}
	// cstr =? str
	Inl B operator==(HC* a, Rng<H> b){R b==a;}
	// fd
	struct Fd{
		I4 id{-1};
		Fd()=default;
		Fd(I4 id):id(id){A(0<=id);}
	};
	// seek fd: whence=SEEK_SET|SEEK_CUR|SEEK_END
	Inl V  seek(Fd fd, I4 off, I4 whc=SEEK_SET){auto r=::lseek(fd.id,off,whc); A(r>=0);}
	Inl I4 tell(Fd fd){auto r=::lseek(fd.id,0,SEEK_CUR); R A(r>=0),i4(r);}
	Inl I4 size(Fd fd){auto p=tell(fd); seek(fd,0,SEEK_END); auto r=tell(fd); seek(fd,p); R r;}
	// get fd: r 0 on eof,1 on c read
	Inl B get(Fd fd, H *cp){auto r=::read(fd.id,(V*)cp,1); R A(r>=0),r;}
	// rw fd
	Tpl<Cls T> Inl V rd(Fd fd, Rng<T> xs){I4 nb=sizeof(T)*xs.len(); A(nb==::read (fd.id,(V*)xs.ptr(),nb));}
	Tpl<Cls T> Inl V wr(Fd fd, Rng<T> xs){I4 nb=sizeof(T)*xs.len(); A(nb==::write(fd.id,(V*)xs.ptr(),nb));}
	// _detail
	Inl I4 _fd_open_mode(HC* m){
		// "r"|"w"|"cr"|"cw" -> flags
		I4 f{0}; H c;
		while((c=*m)){
			f|=(c=='c'?O_CREAT:c=='r'?O_RDONLY:c=='w'?O_WRONLY:I4(0));
			++m;}
		R f;}
	// mfd
	struct MFd:Fd{
		MFd()=default;
		MFd(Rng<H> path, HC* mode="r"){open(path,mode);}
		virtual ~MFd(){close();}
		Fd fd(){R *this;}
		// open path mode="r"|"w"|"cr"|"cw" (NO "rw" so far)
		V open(Rng<H> path, HC* mode="r"){id=::open(cstr(path),_fd_open_mode(mode),00644); A(id!=-1);}
		V close(){if(id!=-1) A(::close(id)!=-1), id=-1; }
	};
	// arr
	Tpl<Cls T,I4 N> struct Arr{
		T p[N];
		T*     ptr() {R p;}
		T*     end() {R p+N;}
		CE I4  len()C{R N;}
		Rng<T> rng() {R {p,N};}
		T&                  operator[](I4 i) {R D(A(0<=i),A(i<N)),p[i];}
		T C&                operator[](I4 i)C{R D(A(0<=i),A(i<N)),p[i];}
		Tpl<Cls U> explicit operator Arr<U,N>(){Arr<U,N> r; OD(i,N) r[i]=U((*this)[i]); R r;}
		                    operator Rng<T>()  {R rng();}
	};
	// arr vector ops
	Tpl<Cls T,I4 N> Rng<T>   sli(Arr<T,N> &a, I4 i, I4 j){D(A(0<=i),A(i<=j),A(j<=N)); R {a.ptr()+i,j-i};}
	Tpl<Cls T,I4 N> Arr<T,N> rev(Arr<T,N> C&a) {Arr<T,N> r; OD(i,N) r[i]=a[(N-1)-i]; R r;}
	Tpl<Cls T,I4 N> T        sum(Arr<T,N> C&x) {T r{0}; OD(i,N) r+=x[i]; R r;}
	Tpl<Cls T,I4 N> T        prod(Arr<T,N> C&x){T r{1}; OD(i,N) r*=x[i]; R r;}
	Tpl<Cls T,I4 N> Arr<T,N> operator*(Arr<T,N> C&x, Arr<T,N> C&y){Arr<T,N> r; OD(i,N) r[i]=x[i]*y[i]; R r;}
	Tpl<Cls T,I4 N> Arr<T,N> operator/(Arr<T,N> C&x, Arr<T,N> C&y){Arr<T,N> r; OD(i,N) r[i]=x[i]/y[i]; R r;}
	Tpl<Cls T,I4 N> Arr<T,N> operator+(Arr<T,N> C&x, Arr<T,N> C&y){Arr<T,N> r; OD(i,N) r[i]=x[i]+y[i]; R r;}
	Tpl<Cls T,I4 N> Arr<T,N> operator-(Arr<T,N> C&x, Arr<T,N> C&y){Arr<T,N> r; OD(i,N) r[i]=x[i]-y[i]; R r;}
	Tpl<Cls T,I4 N> Arr<T,N> operator*(Arr<T,N> C&x, T C&y){Arr<T,N> r; OD(i,N) r[i]=x[i]*y; R r;}
	Tpl<Cls T,I4 N> Arr<T,N> operator/(Arr<T,N> C&x, T C&y){Arr<T,N> r; OD(i,N) r[i]=x[i]/y; R r;}
	Tpl<Cls T,I4 N> Arr<T,N> operator+(Arr<T,N> C&x, T C&y){Arr<T,N> r; OD(i,N) r[i]=x[i]+y; R r;}
	Tpl<Cls T,I4 N> Arr<T,N> operator-(Arr<T,N> C&x, T C&y){Arr<T,N> r; OD(i,N) r[i]=x[i]-y; R r;}
	Tpl<Cls T,I4 N> Arr<T,N> operator*(T C&x, Arr<T,N> C&y){Arr<T,N> r; OD(i,N) r[i]=x*y[i]; R r;}
	Tpl<Cls T,I4 N> Arr<T,N> operator/(T C&x, Arr<T,N> C&y){Arr<T,N> r; OD(i,N) r[i]=x/y[i]; R r;}
	Tpl<Cls T,I4 N> Arr<T,N> operator+(T C&x, Arr<T,N> C&y){Arr<T,N> r; OD(i,N) r[i]=x+y[i]; R r;}
	Tpl<Cls T,I4 N> Arr<T,N> operator-(T C&x, Arr<T,N> C&y){Arr<T,N> r; OD(i,N) r[i]=x-y[i]; R r;}
	Tpl<Cls T,I4 N> B operator==(Arr<T,N> C&x, Arr<T,N> C&y){OD(i,N) if(!(x[i]==y[i])) R 0; R 1;}
	Tpl<Cls T,I4 N> B operator!=(Arr<T,N> C&x, Arr<T,N> C&y){OD(i,N) if(  x[i]!=y[i] ) R 1; R 0;}
	Tpl<Cls T,I4 N> B operator<=(Arr<T,N> C&x, Arr<T,N> C&y){OD(i,N) if(!(x[i]<=y[i])) R 0; R 1;}
	Tpl<Cls T,I4 N> B operator< (Arr<T,N> C&x, Arr<T,N> C&y){OD(i,N) if(!(x[i]< y[i])) R 0; R 1;}
	// math
	using std::isinf, std::isnan, std::signbit, std::fabs, std::min;
	F4 C Inf=INFINITY;
	Inl F4 sgn(F4 x){R (x==0)?0:signbit(x)?-1:+1;}
	Tpl<Cls T> Inl T clamp(T x, T mi, T mx){SA(sizeof(T)<=16); R x<mi?mi:x>mx?mx:x;}
	Inl F4 mag(Arr<F4,2> v){R sqrtf(v[0]*v[0]+v[1]*v[1]);}
	// Pr
	Tpl<Cls S> struct Pr{
		S sm;
		Pr(S sm):sm(sm){}
		Tpl<Cls T0> Pr operator()(T0 t0){pr(sm,t0);R *this;}
		Tpl<Cls T0,Cls T1> Pr operator()(T0 t0,T1 t1){pr(sm,t0,t1);R *this;}
		Tpl<Cls T0,Cls T1,Cls T2> Pr operator()(T0 t0,T1 t1,T2 t2){pr(sm,t0,t1,t2);R *this;}
		Tpl<Cls T0,Cls T1,Cls T2,Cls T3> Pr operator()(T0 t0,T1 t1,T2 t2,T3 t3){pr(sm,t0,t1,t2,t3);R *this;}
		Pr n(){pr(sm,'\n'); R *this;}
	};
	Tpl<Cls S> Pr<S> pr(S sm){R Pr(sm);}
	Inl Pr<Fd> pr1{Fd{1}};
	Inl Pr<Fd> pr2{Fd{2}};
	// pr str
	Tpl<Cls S> V pr(S sm, Rng<H> x, I4 w=-1){
		if(x.len()<w){
			Arr<H,256> buf; I4 n=w-x.len(); A(n<=buf.len());
			DO(i,n) buf[i]=' ';
			wr(sm,Rng<H>{buf.ptr(),n});}
		wr(sm,x);}
	// pr cstr
	Tpl<Cls S> V pr(S sm, HC* x, I4 w=-1){pr(sm,str(x),w);}
	// pr h
	Tpl<Cls S> V pr(S sm, H x, I4 w=-1){pr(sm,Rng<H>(&x,1),w);}
	// _detail
	Inl Arr<H,16> _digits{'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
	// pr i4
	Tpl<Cls S> V pr(S sm, I4 x, I4 p=-1, I4 w=-1, I4 b=10){
		Arr<H,256> buf; I4 i=buf.len();
		B s=((x<0)?(x=-x, 1):0);
		while(buf.len()-i<p) buf[--i]='0';
		if(0<p) buf[--i]='.'; //prec
		do buf[--i]=_digits[x%b], x/=b; while(x);
		if(s) buf[--i]='-';
		while(buf.len()-i<w) buf[--i]=' ';  // width
		pr(sm, Rng<H>{buf.ptr()+i,buf.len()-i});}
	// pr u4 TODO: code dup
	Tpl<Cls S> V pr(S sm, U4 x, I4 p=-1, I4 w=-1, I4 b=10){
		Arr<H,256> buf; I4 i=buf.len();
		while(buf.len()-i<p) buf[--i]='0';
		if(0<p) buf[--i]='.'; //prec
		do buf[--i]=_digits[x%b], x/=b; while(x);
		while(buf.len()-i<w) buf[--i]=' ';  // width
		pr(sm, Rng<H>{buf.ptr()+i,buf.len()-i});}
	// pr ptr8 -- 12 hex digits representing 48bits used for adress
	Tpl<Cls S> V pr(S sm, V* x, I4 w=-1){
		AS(sizeof(V*)==8); D(A((U8(x)&U8(0xffff000000000000))==0));
		Arr<H,256> buf; I4 i=buf.len(); U8 y=(U8)x;
		OD(_,12) buf[--i]=_digits[y%16], y/=16;
		while(buf.len()-i<w) buf[--i]=' ';  // width
		pr(sm, Rng<H>{buf.ptr()+i,buf.len()-i});}
	// pr f4
	Tpl<Cls S> V pr(S sm, F4 x, I4 p=-1, I4 w=-1){
		//F4 max:3.4E+38, F4 min:1.4E-45
		Arr<H,256> buf; I4 i=buf.len();
		B s=signbit(x); if(s) x=-x;
		if(isinf(x)) buf[--i]='f',buf[--i]='n',buf[--i]='I';
		el(isnan(x)) buf[--i]='n',buf[--i]='a',buf[--i]='N';
		else{
			p=(p>-1?p:6);
			I4 e=0;
			// TODO: better log10
			if(x==0); el(x<0.000001) while(x<1.0f) x*=10.0f,--e; el(1000000<=x) while(10.0f<=x) x/=10.0f,++e;
			B se=signbit(e); if(se) e=-e;
			I4 xi=I4(x);
			I4 xd=I4((x-xi)*1000000.0f);
			// TODO: rounding
			if(e){
				while(e) buf[--i]=_digits[e%10], e/=10; //exponent
				if(se) buf[--i]='-';
				buf[--i]='e';}
			OD(j,6) (j<p?(buf[--i]=_digits[xd%10]):0), xd/=10; //decimal
			if(p>0) buf[--i]='.';
			do buf[--i]=_digits[xi%10], xi/=10; while(xi); //integral
		}
		if(s) buf[--i]='-';
		pr(sm, Rng<H>{buf.ptr()+i,buf.len()-i});}
	// pr rng
	Tpl<Cls S,Cls T> V pr(S sm, Rng<T> xs, I4 p=-1, I4 w=-1){
		pr(sm,'(');
		if(xs.len()>0){
			DO(i,xs.len()-1) pr(sm,xs[i],p,w),pr(sm,' ');
			pr(sm,xs[xs.len()-1],p,w);}
		pr(sm,')');}
	// cat
	//MRng cat(Rng<H> x0, Rng<H> x1){
	//	R MRng{x0.len()+x1.len()};
	//}

	// memory alc/dlc TODO: stacked heap
	//pr1(typeid(T).name())("\n");
	Inl V pmem(HC* t, V* p, I4 n0, I4 n1){pr1(t)(p)(" ")(n0)("[")(n1)("]\n");}
	Inl V pmem(HC* t, V* p, I4 n0){pr1(t)(p)(" ")(n0)("\n");}
	Inl V pmem(HC* t, V* p){pr1(t)(p)("\n");}
	Inl V* alc_mem(I4 nb){
		V* p=malloc(nb);
		pmem("I @:",p,nb);
		R p;}
	Inl V dlc_mem(V* p){
		A(p!=0);
		pmem("I ~:",p);
		free(p);}
	Tpl<Cls T> T* alc_arr(I4 n){
		I4 nb=n*sizeof(T);
		T* p=(T*)malloc(nb);
		pmem("I @:",p,sizeof(T),n);
		DO(i,n) new ((V*)(p+i)) T;
		R p;}
	Tpl<Cls T> V dlc_arr(T* p, I4 n){
		pmem("I ~:",p,sizeof(T),n);
		DO(i,n) p[i].~T();
		free(p);}
	// mrng
	Tpl<Cls T> struct MRng:Rng<T>{
		MRng()=default;
		explicit MRng(I4 n){alc(n);}
		MRng(MRng<T>& x)=delete;
		explicit MRng(Rng<T> x){alc(x.len()); cp(this->rng(), x);}
		explicit MRng(Rng<T> x, Rng<T> y){
			alc(x.len()+y.len());
			cp(th rng(),        x);
			cp(th rng(x.len()), y);
		}
		V alc(I4 n){D(A(!th p)); th p=alc_arr<T>(n); th l=n;}
		~MRng(){dlc_arr<T>(th p, th l); th p=0, th l=0;}
	};


	// char buf
	struct MBuf;
	struct Buf{
		MBuf &x;
		MBuf *operator->()C{R &x;}
		Buf(MBuf &x):x(x){}
	};
	struct MBuf{
		H* p{}; I4 l{}, z{};
		I4 len(){R l;}
		Rng<H> rng(){R {p,l};}
		operator Rng<H>(){R rng();}
		MBuf(I4 n){alc(n);}
		MBuf(MBuf C&)=delete;
		MBuf(MBuf &&)=delete;
		~MBuf(){//OD(i,l) p[i].~H();
			dlc_mem(p); p=0; z=0; l=0;}
		Buf buf(){R Buf(*this);}
		V alc(I4 n){A(th p==0); th p=(H*)alc_mem(n); th z=n; th l=0;}
	};
	Inl V wr(Buf buf, Rng<H> xs){
		A(xs.len()<=buf->z-buf->l);
		DO(i,xs.len()) buf->p[buf->l++]=xs[i];}
	// save/load serialization
	Tpl<Cls S>struct Save{
		S s;
		Save(S s):s(s){}
		Tpl<Cls T0> Save operator()(T0&t0){save(s,t0);R *this;}
	};
	Tpl<Cls S>struct Load{
		S s;
		Load(S s):s(s){}
		Tpl<Cls T0> Load operator()(T0&t0){load(s,t0);R *this;}
	};
	Tpl<Cls S> Save<S> save(S s){R Save<S>(s);}
	Tpl<Cls S> Load<S> load(S s){R Load<S>(s);}
	// save primitive
	Tpl<Cls S>V save(S s, I1 x){wr(s,Rng<I1>(&x,1));}
	Tpl<Cls S>V save(S s, I2 x){wr(s,Rng<I2>(&x,1));}
	Tpl<Cls S>V save(S s, I4 x){wr(s,Rng<I4>(&x,1));}
	Tpl<Cls S>V save(S s, I8 x){wr(s,Rng<I8>(&x,1));}
	Tpl<Cls S>V save(S s, U1 x){wr(s,Rng<U1>(&x,1));}
	Tpl<Cls S>V save(S s, U2 x){wr(s,Rng<U2>(&x,1));}
	Tpl<Cls S>V save(S s, U4 x){wr(s,Rng<U4>(&x,1));}
	Tpl<Cls S>V save(S s, U8 x){wr(s,Rng<U8>(&x,1));}
	Tpl<Cls S>V save(S s, F4 x){wr(s,Rng<F4>(&x,1));}
	Tpl<Cls S>V save(S s, F8 x){wr(s,Rng<F8>(&x,1));}
	// load primitive
	Tpl<Cls S>V load(S s, I1&x){rd(s,Rng<I1>(&x,1));}
	Tpl<Cls S>V load(S s, I2&x){rd(s,Rng<I2>(&x,1));}
	Tpl<Cls S>V load(S s, I4&x){rd(s,Rng<I4>(&x,1));}
	Tpl<Cls S>V load(S s, I8&x){rd(s,Rng<I8>(&x,1));}
	Tpl<Cls S>V load(S s, U1&x){rd(s,Rng<U1>(&x,1));}
	Tpl<Cls S>V load(S s, U2&x){rd(s,Rng<U2>(&x,1));}
	Tpl<Cls S>V load(S s, U4&x){rd(s,Rng<U4>(&x,1));}
	Tpl<Cls S>V load(S s, U8&x){rd(s,Rng<U8>(&x,1));}
	Tpl<Cls S>V load(S s, F4&x){rd(s,Rng<F4>(&x,1));}
	Tpl<Cls S>V load(S s, F8&x){rd(s,Rng<F8>(&x,1));}
	// save/load arr
	Tpl<Cls S,Cls T,I4 N>V save(S s, Arr<T,N>&x){DO(i,x.len()) save(s,x[i]);}
	Tpl<Cls S,Cls T,I4 N>V load(S s, Arr<T,N>&x){DO(i,x.len()) load(s,x[i]);}
	// save/load mrng
	Tpl<Cls S,Cls T>V save(S s, MRng<T>&x){save(s,x.len()); DO(i,x.len()) save(s,x[i]);}
	Tpl<Cls S,Cls T>V load(S s, MRng<T>&x){I4 l; load(s,l); x.alc(l); DO(i,x.len()) load(s,x[i]);}
	// chdir()--set working dir to executable dir
	V chdir();
}//namespace ext
using namespace ext;


// extra
namespace ext{
	// str =? str
	Inl B eq(HC* a, HC* b){R str(b)==a;}
}

/*// buf
Tpl<Cls T> struct Buf{
	I4 l{0};
	Arr<T,256> xs;
	Buf()=default;
	Buf(Buf C&)=delete;
	I4 len()C{R l;}
	Rng<H> rng(){R{xs.ptr(),l};}
};
Tpl<Cls T,I4 N> V wr(Buf<T,N> &b, T x){A(b.l<N); b.xs[b.l]=x; ++b.l;}
*/


// args: x y z w -> r
// list: xs ys zs ws -> rs
// integer: i(index) j k(count) l(length) n(size,length)
// string: m(message)
// ptr: p q
//   f g h
// types: t u
//   o s(self?)
// local: a b c d    e



// cstr =? cstr
//Inl B eq(char C* s0, char C* s1){while(*s0 and *s1 and *s0==*s1) ++s0, ++s1; R *s0==*s1;}

//Tpl<class T> T sgn(T x){R (T(0)<x)-(x<T(0));}

//Tpl<Cls Y,Cls X> Y bitconv(X x){AS(sizeof(X)==sizeof(Y)); union{X x;Y y;}c; c.x=x; R c.y;}
//I4 _signbit(F4 x){R (bitconv<I4>(x)&(1<<31))>>31;}
//I4 _signbit(I4 x){R (x&(1<<31))>>31;}


/*
	serialization: pointers are saved, objects are saved with adresses? adresses of objects in arrays are resolved based on array address
	save(arr_obj, adr=1) -> obj_type count ptr_adr obj_data0 ...
	save(arr_obj, adr=1) -> obj_type count ptr_adr obj_data0 ...
	on load pointers resolved as forward goto labels in PL, by linked list
	^ so it is possible to consolidate saved data?
	wat about exterior pointers? outside saved data forex to pre-created structures? - on save thay can be marked as exterior,
		+ exterior targets should register their adresses to load infrastructure - but with what id? they dont have an id
		exterior pts can only point to named structs?
*/

//using usize=std::size_t;
//using ssize=std::ssize_t;




