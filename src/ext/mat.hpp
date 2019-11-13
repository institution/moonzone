#pragma once
#include"ext/ext.hpp"
namespace ext{
	// detail
	Tpl<Cls T> Inl T &_ref(T &x){R x;}
	Tpl<Cls T> Inl T C&_cref(T C&x){R x;}
	//forward ten shape
	Tpl<Cls T,I4 N> struct Ten;
	// ten slice
	Tpl<Cls T,I4 N> struct Tsl{
		T *o{}; Ten<T,N> *t;
		I4 len(){R t->len();}
		I4 skp(){R t->skp();}
		I4 len(I4 i){R t->len(i);}
		I4 skp(I4 i){R t->skp(i);}
		Arr<I4,N> dim(){R t->dim();}
		Arr<I4,N> skt(){R t->skt();}
		decltype(auto) _deref(T *p){if CE(N==1) R _ref(*p); else R Tsl<T,N-1>{p,t->sub()};}
		decltype(auto) operator[](I4 i){D(A(0<=i),A(i<len())); R _deref(o+i*skp());};
		T *ptr(){R t->b;}
		T *end(){R t->e;}
		Ten<T,N> ten(){R Ten<T,N>{t->b,t->e,o,t->kl};}
		Tsl<T,N> tsl(){R Tsl<T,N>{o,t};}
		I4 ind(T *tt){R t->ind(tt);}
	};
	//ten shape
	struct KL{I4 k{},l{};}; // skip len
	Tpl<Cls T,I4 N> struct Ten{
		T *b{},*e{},*o{}; Arr<KL,N> kl{};
		Ten<T,N>()=default;
		Ten<T,N>(T *b, T *e, T *o, Arr<KL,N> kl):b(b),e(e),o(o),kl(kl){}
		Ten<T,N>(T *b, T *e, T *o, Arr<I4,N> d, Arr<I4,N> p):b(b),e(e),o(o){
			DO(i,N) kl[i]={p[i],d[i]};}
		explicit Ten<T,N>(Ten<T,N>C&)=default;
		Ten<T,N-1>* sub(){R reinterpret_cast<Ten<T,N-1>*>(this);}
		Tsl<T,N> tsl(){R {o,this};}
		operator Tsl<T,N>(){R tsl();}
		Rng<T> rng(){R {b,i4(e-b)};}
		T *ptr(){R th b;}
		T *end(){R th e;}
		I4 &len(I4 i){R kl[i].l;}
		I4 &skp(I4 i){R kl[i].k;}
		I4 len(){R len(N-1);}
		I4 skp(){R skp(N-1);}
		Arr<I4,N> dim(){Arr<I4,N> r; DO(i,N) r[i]=len(i); R r;}
		Arr<I4,N> skt(){Arr<I4,N> r; DO(i,N) r[i]=skp(i); R r;}
		decltype(auto) _deref(T *p){if CE(N==1) R _ref(*p); else R Tsl<T,N-1>{p,sub()};}
		decltype(auto) operator[](I4 i){D(A(0<=i),A(i<len())); R _deref(o+i*skp());};
		I4 ind(T *t){R t-b;}
	};
	Tpl<Cls T,I4 N> Inl Ten<T,N> tp(Tsl<T,N> t){
		auto r{t.ten()}; r.kl=rev(r.kl); R Ten<T,N>(r);}
		//R {t.b(), t.e(), t.o, rev(t.dim()), rev(t.skt())};
	Tpl<Cls T,I4 N> Inl Ten<T,N> flip(Tsl<T,N> t, I4 i){
		auto r{t.ten()}; r.skp(i)=-t.skp(i); r.o+=t.skp(i)*t.len(i-1); R Ten<T,N>(r);}
	Tpl<Cls T,I4 N> Inl Ten<T,N> rev(Tsl<T,N> t){
		auto r{t.ten()}; r.skp(N-1)=-t.skp(N-1); r.o+=t.skp()*(t.len()-1); R Ten<T,N>(r);}
	Tpl<Cls T,I4 N> Inl Ten<T,N> map(Rng<T> m, Arr<I4,N> d){
		Ten<T,N> r; I4 a=1; DO(i,N){r.d(i)=d[i]; r.p(i)=a; a*=d[i];}
		A(a==m.len()); r.b=m.begin(); r.e=m.end(); r.o=m.begin(); R Ten<T,N>(r);}
	Tpl<Cls T,I4 N>Inl Ten<T,N> view(Tsl<T,N> t, Arr<I4,N> xpos, Arr<I4,N> xdim){
		A(xpos+xdim<=t.dim());
		R Ten<T,N>{t.ptr(), t.end(), t.o+sum(xpos*t.skt()), xdim, t.skt()};}
	// managed ten
	Tpl<Cls T,I4 N> struct MTen:Ten<T,N>{
		MTen()=default;
		MTen(MTen C&)=delete;
		MTen(Arr<I4,N> d){alc(d);}
		V alc(Arr<I4,N> d){
			A(th b==0); I4 a=1; DO(i,N){th len(i)=d[i]; th skp(i)=a; a*=d[i];}
			A(a>0); th b=alc_arr<T>(a); th e=th b+a; th o=th b;}
		virtual ~MTen(){dlc_arr<T>(th b,th e-th b); th b=th e=th o=0;}
	};
	Tpl<Cls S,Cls T,I4 N> Inl V pr(S sm, Tsl<T,N> t, I4 w=-1, I4 p=-1){
		pr(sm,'(');
		if(t.len()>0){
			DO(i,t.len()-1) pr(sm,t[i],w,p),pr(sm,' ');
			pr(sm, t[t.len()-1],w,p);}
		pr(sm,')');}
	// save/load
	Tpl<Cls S,Cls T,I4 N>V save(S s, MTen<T,N>&x){
		auto d=x.dim(); save(s,d);
		auto r=x.rng(); DO(i,r.len()) save(s,r[i]);}
	Tpl<Cls S,Cls T,I4 N>V load(S s, MTen<T,N>&x){
		Arr<I4,N> d; load(s,d); x.alc(d);
		auto r=x.rng(); DO(i,r.len()) load(s,r[i]);}







}//ext
