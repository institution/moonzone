#include"df.hpp"
#include"ext/png.hpp"
I4 main(I4 argc, HC* argv[]){
	if(argc!=2) R pr1("Usage: ")(argv[0])(" <path/to/input.png>\n"),0;
	auto ipath=strz(argv[1]);
	A(ipath.sli(-5)==".png");
	auto opath=MRng<H>{ipath.sli(0,-5), strz(".df.png")};

	pr1("I i:")(ipath.rng())("\n");
	pr1("I o:")(opath.rng())("\n");

	MTen<U1,2> im1;
	png_load_gray8b(im1, ipath);
	png_save_gray8b(im1, MRng<H>{ipath.sli(0,-5), strz(".input.png")});


	MTen<F4,2> im2(im1.dim());
	auto xs=im1.rng();
	auto ys=im2.rng();
	DO(i,ys.len()) ys[i]=F4(xs[i])/F4(255);
	ndt(im2.tsl(), 4.0);
	DO(i,xs.len()) xs[i]=u1(ys[i]*F4(255));
	png_save_gray8b(im1, opath);


	R 0;}
