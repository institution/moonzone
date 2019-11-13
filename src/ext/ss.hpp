#pragma once
#include"ext/mat.hpp"
struct Sprite{
	F4 t0{0},t1{0},t2{0},t3{0}; // tex pos[uv], tex dim[uv]
	F4 r0{0},r1{0},r2{0},r3{0}; // render pos[u] (bearing), render dim[u]
	F4 a0{0};                   // advance[u]
	F4 upp{0};                  // units per pixel

	Sprite& box(F4 x,F4 y,F4 w,F4 h){t0=x,t1=y,t2=w,t3=h;
		A(0.0f<=x),A(x<=1.0f), A(0.0f<=y),A(y<=1.0f), A(0.0f<=w),A(w<=1.0f), A(0.0f<=h),A(h<=1.0f);
		R *this;}
	Sprite& bea(F4 x,F4 y){r0=x,r1=y;R *this;}
	Sprite& dim(F4 w,F4 h){r2=w,r3=h;R *this;}
	Sprite& adv(F4 x){a0=x;R *this;}
};
Tpl<Cls S> V save(S f, Sprite &x){save(f)(x.t0)(x.t1)(x.t2)(x.t3)(x.r0)(x.r1)(x.r2)(x.r3)(x.a0)(x.upp);}
Tpl<Cls S> V load(S f, Sprite &x){load(f)(x.t0)(x.t1)(x.t2)(x.t3)(x.r0)(x.r1)(x.r2)(x.r3)(x.a0)(x.upp);}
Tpl<Cls S> V pr(S s, Sprite &p, I4 id=-1){
	pr(s)("sprite ")(id)
	(" box ")(p.t0)(" ")(p.t1)(" ")(p.t2)(" ")(p.t3)
	(" bea ")(p.r0)(" ")(p.r1)(" dim ")(p.r2)(" ")(p.r3);}

struct Sheet{
	U4 bom{1};        // should be==1
	I4 texu{-1};      // assigned texture unit
	F4 px{0},py{0};   // default padding [unit]
	F4 height{0};     // defaut line height [unit]
	F4 width{0};      // approx width of a character, 1ex [unit]
	F4 asc{0},des{0}; // position of baseline [unit], des unused
	MRng<Sprite> spr; // sprites
	MTen<U1,2> tex;

	Sheet(){texu=-1;}
	Sheet(I4 texu):texu(texu){}
	Sheet(Sheet C&)=delete;
	Sheet& max_sprites(I4 x){spr.alc(x);R *this;}
	Sprite& sprite(I4 i){R spr[i];}
};
Tpl<Cls S> V save(S f, Sheet &x){
	A(x.bom==1), save(f)(x.bom); save(f)(x.texu)(x.px)(x.py)(x.height)(x.width)(x.asc)(x.des)(x.spr);}
Tpl<Cls S> V load(S f, Sheet &x){
	load(f)(x.bom), A(x.bom==1);
	I4 tu{-1}; load(f)(tu);
	if(x.texu==-1) x.texu=tu;
	load(f)(x.px)(x.py)(x.height)(x.width)(x.asc)(x.des)(x.spr);}
