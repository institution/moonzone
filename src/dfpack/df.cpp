#include"df.hpp"

namespace ext{
	F4 C Far=+1e22; // guard, more than max possible dist
	F4 C Inn=-0.25; // inside shape
	// distance transform horizontal (squared)
	V d2th(Tsl<F4,2> f2){
		//line len
		I4 C N=f2.len(0); A(N>0);
		MRng<F4> l{N};   // current line copy
		MRng<I4> v{N};   // v[k] para position
		MRng<F4> z{N+1}; // z[k]:z[k+1] k-th para is lowest here
		I4 k,i;  // index of rightmost para in lower envelope, next para to add
		DO(j,f2.len()){
			auto f1=f2[j];
			DO(i,l.len()) l[i]=f1[i];
			// insert first para
			v[0]=0;
			z[0]=-Far, z[1]=+Far; // Inf is guard not float_inf
			k=0, i=1;
			// insert rest paras
			while(i<N){
				F4 p0=v[k], p1=i; F4 h0=l[p0], h1=l[p1];
				D(A(h0>=-1),A(h1>=-1));
				F4 z1=0.5f*((h1-h0+(p1*p1-p0*p0))/(p1-p0));
				//if(z1>1e10) z1=Far;  // hmm? TODO
				D(A(z[k]<z1 or k>0));
				if(z[k]<z1){++i, ++k; v[k]=p1, z[k]=z1, z[k+1]=Far;}else{--k;}}
			// sample back info f
			k=0;
			DO(i,N){
				while(F4(i)>z[k+1]) ++k;
				f1[i]=l[v[k]]+pow(F4(i-v[k]),2);}}}
	// map membership:
	// 1.0 -> 0.0
	// 0.9 -> 0.1
	// 0.2 -> 0.8
	// 0.0 -> inf
	F4 mapmem(F4 x){
		D(A(0<=x),A(x<=1.0));
		R (x==0)?Far:1-x;}
	// signed distance transform: membership[0,1] -> sdf[-+dist]
	V sdt(Tsl<F4,2> f){
		if(f.len(0)==0 or f.len(1)==0) R;
		MTen<F4,2> fout(f.dim());
		MTen<F4,2> finn(f.dim());
		DO(j,f.len()){
			auto fj=f[j];
			DO(i,fj.len()){
				auto x=fj[i];
				fout[j][i]=mapmem(x);
				finn[j][i]=mapmem(1-x);}}
		//pr1("I ->fout:")(fout.tsl(),1)("\n");
		//pr1("I ->finn:")(finn.tsl(),1)("\n");
		d2th(finn); d2th(tp(finn.tsl()));
		d2th(fout); d2th(tp(fout.tsl()));
		//pr1("I <-fout:")(fout.tsl(),1)("\n");
		//pr1("I <-finn:")(finn.tsl(),1)("\n");
		DO(j,f.len()){
			auto fj=f[j];
			DO(i,fj.len()){
				auto &x=fj[i];
				x=(x==1)?sqrtf(finn[j][i]):-sqrtf(fout[j][i]);}}}
	// clamp normalize: [-+x] -> [-+mx] -> [0:1]
	V cn(Tsl<F4,2> f2, F4 mx){
		DO(j,f2.len()){auto f1=f2[j];
			DO(i,f1.len()){auto &x=f1[i];
				// clamp
				if(x<=-mx) x=-mx;
				if(+mx<=x) x=+mx;
				// normalize: -mx,+mx -> 0,1
				x=(x+mx)/(2*mx);
				D(A(0<=x),A(x<=1));}}}
	// sum of 2d array
	F4 sum(Tsl<F4,2> y2){
		F4 r{0};
		DO(j,y2.len()){
			auto y1=y2[j];
			DO(i,y1.len()){
				r+=y1[i];}}
		R r;}
	// img8x -> img1x
	V downscale8x(MTen<F4,2> &y2, Tsl<F4,2> x2){
		auto dim=x2.dim();
		A(dim[0]%8==0), A(dim[1]%8==0);
		dim[0]=dim[0]/8, dim[1]=dim[1]/8;
		y2.alc(dim);
		DO(j,y2.len()){
			auto y1=y2[j];
			DO(i,y1.len()){
				auto w2=view(x2, {i*8,j*8}, {8,8});
				y1[i]=sum(w2)/64.0f;}}}
	// convert 0..1->0..255
	V conv8b(Rng<U1> rs, Rng<F4> xs){DO(i,rs.len()) rs[i]=u1(xs[i]*F4(255));}
	// convert 0..255->0..1
	V convf (Rng<F4> rs, Rng<U1> xs){DO(i,rs.len()) rs[i]=F4(xs[i])/F4(255);}
	TEST(
		pr1("I TEST df 1\n");
		MTen<F4,2> img{{2,2}};
		cp(img.rng(), Arr<F4,4>{0,1,0,0}.rng());
		pr1("I img:")(img.tsl(),0)("\n");
		sdt(img);
		pr1("I sdf:")(img.tsl(),0)("\n");
		auto exp=Arr<F4,4>{-1,+1,-sqrtf(2),-1};
		pr1("I ")(img.rng(),1)("=?")(exp.rng(),1)("\n");
		A(img.rng()==exp.rng());
		pr1("I END TEST df 1\n");
	)
	TEST(
		pr1("I TEST df 2\n");
		MTen<F4,2> img{{4,2}};
		cp(img.rng(), Arr<F4,4*2>{0,0,1,1, 0,0,1,1}.rng());
		pr1("I img:")(img.tsl(),0)("\n");
		sdt(img);
		pr1("I sdf:")(img.tsl(),0)("\n");
		auto exp=Arr<F4,4*2>{-2,-1,+1,+2, -2,-1,+1,+2};
		pr1("I ")(img.rng(),1)("=?")(exp.rng(),1)("\n");
		A(img.rng()==exp.rng());
		pr1("I END TEST df 2\n");
	)


}//ext
