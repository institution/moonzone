#include"ext/mat.hpp"
#include"ext/png.hpp"
Tpl<Cls T>using V2=Arr<T,2>;

V sum(Tsl<F4,2> y2)
	F4 r{0};
	DO(j,y2.len()){
		auto y1=y2[j];
		DO(i,y1.len()){
			r+=y1[i];}}
	R r;}

// y2 <- x2 / 8x8
V downscale8x(MRng<F4,2> &y2, Tsl<F4,2> x2){
	auto dim=x.dim();
	A(dim[0]%8==0), A(dim[1]%8==0);
	dim[0]=dim[0]/8, dim[1]=dim[1]/8;
	y2.alc(dim);
	DO(j,y2.len()){
		auto y1=y2[j];
		DO(i,y1.len()){
			auto w2=view(x2, {i*8,j*8}, {8,8});
			y1[i]=sum(w2)/64.0f;}}}

// name.rc + name.png -> name.ss (D:name.df.png)
I4 main(I4 argc, HC* argv[]){
	if(argc!=2) R pr1("Usage: ")(argv[0])(" <path/to/input.rc>\n"),0;

	auto ipath_png=strz(argv[1]); A(ipath_rc.sli(-5)==".png");
	auto opath_png=MRng<H>{ipath_rc.sli(0,-5), strz(".df.png")};

	MTen<F4,2> src, dst;
	png_load_gray8b(src, opath_png);
	downscale8x(dst,src.tsl());
	png_save_gray8b(dst, opath_png);

	png_castf_gray8b
	//downscale8x(s.tex)
	DO(i,ys.len()) ys[i]=F4(xs[i])/F4(255);


	DO(i,xs.len()) xs[i]=u1(ys[i]*F4(255));

	// convert sheet.tex to distance field
	MTen<F4,2> ftex(s.tex.dim());
	auto xs=s.tex.rng();
	auto ys=ftex.rng();
	DO(i,ys.len()) ys[i]=F4(xs[i])/F4(255);
	DO(i,s.spr.len()){
		pr1("I processing sprite ")(i)("\n");
		auto& p=s.spr[i];
		Ten<F4,2> box=view(ftex.tsl(), V2<I4>{i4(p.t0), i4(p.t1)}, V2<I4>{i4(p.t2), i4(p.t3)});
		pr1("I box.dim=")(box.dim().rng())("\n");
		ndt(box.tsl(), 6.0);}

	//MTen



	MFd out{opath_ss, "cw"};
	save(out.fd())(s);

	R 0;}





