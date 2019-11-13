#include"ext/ss.hpp"
#include"ext/png.hpp"
#include"df.hpp"
#include"parser.hpp"
Tpl<Cls T>using V2=Arr<T,2>;
// note: +y goes down, +x goes right
// load rc+png->sheet
V load_sheet_rc(Sheet & s, Rng<H> path_rc){
	Parser p{path_rc};
	Token t=p.parse();
	A(t=="sheet");
	A(p.parse().num()==-1);
	while(1){
		t=p.parse();
		if (t=="pad")         s.px=p.parse().flo(), s.py=p.parse().flo();
		el (t=="height")      s.height=p.parse().flo();
		el (t=="width")       s.width=p.parse().flo();
		el (t=="asc")         s.asc=p.parse().flo();
		el (t=="des")         s.des=p.parse().flo();
		el (t=="max_sprites") s.max_sprites(p.parse().num());
		el (t=="name")        p.parse();
		else break;}
	while(t=="sprite"){
		auto &r=s.sprite(p.parse().num());
		while(1){
			t=p.parse();
			if (t=="box")  r.t0=p.parse().flo(), r.t1=p.parse().flo(), r.t2=p.parse().flo(), r.t3=p.parse().flo();
			el (t=="bea")  r.r0=p.parse().flo(), r.r1=p.parse().flo();
			el (t=="dim")  r.r2=p.parse().flo(), r.r3=p.parse().flo();
			el (t=="adv")  r.a0=p.parse().flo();
			el (t=="name") p.parse();
			else break;}}
	if(t!="") pr2("E [")(path_rc)(":")(p.line)(":")(p.col)("] EOF not reached\n");}


// name.rc + name.png -> name.rc.ext, name.df.ext, name.df.png
I4 main(I4 argc, HC* argv[]){
	if(argc!=2) R pr1("Usage: ")(argv[0])(" <path/to/input.rc>\n"),0;

	auto ipath_rc   =strz(argv[1]); A(ipath_rc.sli(-4)==".rc");
	auto ipath_png  =MRng<H>{ipath_rc.sli(0,-4), strz(".png")};
	auto opath_ss   =MRng<H>{ipath_rc.sli(0,-4), strz(".ss")};
	auto opath_rcext=MRng<H>{ipath_rc.sli(0,-4), strz(".rc.ext")};
	auto opath_dfext=MRng<H>{ipath_rc.sli(0,-4), strz(".df.ext")};
	auto opath_dfpng=MRng<H>{ipath_rc.sli(0,-4), strz(".df.png")};

	Sheet s{-1};
	pr1("I ")(ipath_rc.rng())('\n');
	load_sheet_rc(s, ipath_rc.rng());

	MTen<U1,2> tex8b;
	pr1("I ")(ipath_png.rng())('\n');
	png_load_gray8b(tex8b, ipath_png.rng());

	// convert distance field
	MTen<F4,2> texf(tex8b.dim());
	convf(texf.rng(), tex8b.rng());
	auto dim=texf.dim();

	DO(i,s.spr.len()){
		pr1("I processing sprite ")(i)("\n");
		// signed dist field
		auto& p=s.spr[i];
		I4 u0=i4(p.t0*dim[0]);
		I4 u1=i4(p.t1*dim[1]);
		I4 u2=i4(p.t2*dim[0]);
		I4 u3=i4(p.t3*dim[1]);
		Ten<F4,2> box=view(texf.tsl(), {u0,u1}, {u2,u3});
		pr1("I box.pos=")(V2<I4>{u0,u1}.rng())("\n");
		pr1("I box.dim=")(box.dim().rng())("\n");

		if(prod(box.dim())!=0){
			sdt(box.tsl());
			// calc scale: unit per pixel
			F4 sX=p.r2/F4(u2);
			F4 sY=p.r3/F4(u3);
			A(sX==sY);
			p.upp=sX;
			// all sprites on one tex should be in the same scale
			A(s.spr[0].upp=p.upp);}}

	MTen<F4,2> texfx;
	downscale8x(texfx, texf);
	DE(x,texfx.rng()) *x=*x/8.0f;  // rescale field so 1 == 1px on tex
	DO(i,s.spr.len()){auto& p=s.spr[i]; p.upp*=8.0f;}  // and update units

	cn(texfx, 4.0f);

	MTen<U1,2> tex8bx(texfx.dim());
	conv8b(tex8bx.rng(), texfx.rng());
	png_save_gray8b(tex8bx, opath_dfpng);

	MFd out{opath_ss, "cw"};
	save(out.fd())(s);
	R 0;}






/*
REPACK IS OPTIONAL

// x y w h ....
void pack(Rng<V4<I32>> boxes) {

	// total area, max width
	I32 area = 0;
	I32 maxw = 0;
	for (auto b: boxes) {
		area += box[2] * box[3];
		maxw = std::max(maxw, box[2]);
	}

	// sort by insert
	auto ns = enumerate((b1 - b0) * 4)



	// sort the boxes for insertion by height, descending
	std::stable_sort(bs.p0, bs.p1, [](a, b){std::cmp(a[2],b[2])});

	boxes.sort((a, b) => b.h - a.h);

  // aim for a squarish resulting container,
  // slightly adjusted for sub-100% space utilization
  const startWidth = Math.max(Math.ceil(Math.sqrt(area / 0.95)), maxWidth);

  // start with a single empty space, unbounded at the bottom
  const spaces = [{x: 0, y: 0, w: startWidth, h: Infinity}];
  const packed = [];

	Buf<Tup<I32,4>> spaces(boxes.size());

  for (const box of boxes) {
    // look through spaces backwards so that we check smaller spaces first
    for (let i = spaces.length - 1; i >= 0; i--) {
      const space = spaces[i];

      // look for empty spaces that can accommodate the current box
      if (box.w > space.w || box.h > space.h) continue;

      // found the space; add the box to its top-left corner
      // |-------|-------|
      // |  box  |       |
      // |_______|       |
      // |         space |
      // |_______________|
      packed.push(Object.assign({}, box, {x: space.x, y: space.y}));

      if (box.w === space.w && box.h === space.h) {
        // space matches the box exactly; remove it
        const last = spaces.pop();
        if (i < spaces.length) spaces[i] = last;

      } else if (box.h === space.h) {
        // space matches the box height; update it accordingly
        // |-------|---------------|
        // |  box  | updated space |
        // |_______|_______________|
        space.x += box.w;
        space.w -= box.w;

      } else if (box.w === space.w) {
        // space matches the box width; update it accordingly
        // |---------------|
        // |      box      |
        // |_______________|
        // | updated space |
        // |_______________|
        space.y += box.h;
        space.h -= box.h;

      } else {
        // otherwise the box splits the space into two spaces
        // |-------|-----------|
        // |  box  | new space |
        // |_______|___________|
        // | updated space     |
        // |___________________|
        spaces.push({
          x: space.x + box.w,
          y: space.y,
          w: space.w - box.w,
          h: box.h
        });
        space.y += box.h;
        space.h -= box.h;
      }
      break;
    }
    yield {packed, spaces};
  }
}
*/

