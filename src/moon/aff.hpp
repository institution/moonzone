#pragma once
#include"ext/ext.hpp"
namespace ext{
	Tpl<Cls T>using V2=Arr<T,2>;
	Tpl<Cls T>using V3=Arr<T,3>;
	using Vf=V2<F4>;
	Inl Vf make_rot(F4 angle){R {cos(angle),sin(angle)};}
	Inl Vf make_rot(Vf v){F4 l=mag(v); R {v[0]/l,v[1]/l};}
	struct Aff{
		// pos(x,y) rot(c,s), scl(x,y)
		Vf p{0.0f,0.0f}, r{1.0f,0.0f}, s{1.0f,1.0f};
		// color
		Arr<U1,4> c{0,0,0,255};
		// global
		V pos(Vf d){p=p+d;}
		V rot(Vf d){A(0);}
		V scl(Vf d){s=s*d;}
		// local
		V posl(Vf d){
			p[0]+=r[0]*s[0]*d[0]-r[1]*s[0]*d[1];
			p[1]+=r[1]*s[1]*d[0]+r[0]*s[1]*d[1];}
		V rotl(Vf d){r=Vf{r[0]*d[0]-r[1]*d[1], r[1]*d[0]+r[0]*d[1]};}
		V scll(Vf d){s=s*d;}
		//V col(Arr<U1,3> c){DO(i,3) th c[i]=c[i]; c[3]=255;}
		V col(Arr<U1,4> c){DO(i,4) th c[i]=c[i];}

		// apply to point
		Vf app(Vf x){
			R{r[0]*s[0]*x[0] - r[1]*s[1]*x[1] + p[0],
			  r[1]*s[0]*x[0] + r[0]*s[1]*x[1] + p[1]};}

		V rst(){p={0,0}, r={1,0}, s={1,1};}
	};

	Inl Vf rotapp(Vf r, Vf d){
		R {r[0]*d[0]-r[1]*d[1], r[1]*d[0]+r[0]*d[1]};}
	Inl Vf rotvec(Vf r, Vf v){
		R {r[0]*v[0]-r[1]*v[1], r[1]*v[0]+r[0]*v[1]};}


}//ext
