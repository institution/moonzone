/**
Create bitmap fonts
```
# search for font file
$ fc-match --format='%{file}\n' DejaVuSans
/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf
# run
$ ./pixfont /usr/share/fonts/truetype/dejavu/DejaVuSans.ttf
```
**/
#include<ft2build.h>
#include FT_FREETYPE_H
#include"ext/ext.hpp
#include"ext/png.hpp"
#define FT_CALL(FT_FNAME, ...)if(auto e=FT_FNAME(__VA_ARGS__))fail(#FT_FNAME": {}",e)
/// dst[pos]<-bm
void copy_bitmap_to(Rng<U8> dst, Arr<I4,2> ddim, Arr<I4,2> pos, FT_Bitmap const& bm){
	print("INFO: copy_bitmap_to: {} {} {} {}\n",
		pos[0], pos[1], I4(bm.width), I4(bm.rows)
	);
	A(I4(bm.pixel_mode)==FT_PIXEL_MODE_GRAY);
	A(I4(bm.palette_mode)==0);
	U8 * line_p=bm.buffer; // ptr to current row
	U8 * byte_p=nullptr;   // ptr to current byte
	// i j -- bm pos
	for (U32 j=0; j<bm.rows; ++j){
		byte_p=line_p;
		for (U32 i=0; i<bm.width; ++i){
			// x y -- dst pos
			I4 x=pos[0]+i, y=pos[1]+j;
			dst[y*ddim[0]+x]=255-(*byte_p);
			++byte_p;
		}
		line_p+=bm.pitch;
	}
}
int main(int argc, char * argv[]){
	chdir();
	A(argc==2); // argv[1] -- font path
	I4 const verbose{1};
	I4 const pixsize{32};
	print("INFO: pixsize {}\n", pixsize);
	// load font
	FT_Library library;
	FT_CALL(FT_Init_FreeType, &library);
	FT_Face face;
	FT_CALL(FT_New_Face, library, argv[1]/*"LiberationSans-Regular.ttf"*/, 0, &face);
	Arr<char,PATH_MAX> tmp{0};
	Arr<char,255> fontname_buf{0};
	Rng<char> fontname=format(fontname_buf.rng(),
		"{}-{}", face->family_name, face->style_name
	);
	// open rc file
	// note: rc coordinate system is: y->up,x->right,origin=left,down
	auto name_rc=format(tmp.rng(), "res/{}.rc", fontname);
	print("INFO: file_rc: {}\n", name_rc);
	File rc_file(name_rc, "w");

	if (verbose > 1) {
		print("num_faces {}\n", face->num_faces);
		print("num_glyphs {}\n", face->num_glyphs);
		print("face_flags {}\n", face->face_flags);
		print("units_per_EM {}\n", face->units_per_EM);
		print("num_fixed_sizes {}\n", face->num_fixed_sizes);
		print("available_sizes");
		for (I4 i=0; i<face->num_fixed_sizes; ++i){
			FT_Bitmap_Size & a = face->available_sizes[i];
			print(" ({} {})", a.width, a.height);
		}
		print("\n");
    }

    /*
    1/72th of an inch
    DPI dot/inch * 16 1/72 inch
    16 * DPI/72 dot
    */
	FT_CALL(FT_Set_Char_Size,
		face,
		0,          // char_width in 1/64th of points
		pixsize*64, // char_height in 1/64th of points, 1 point = 1/72 inch
		72,         // x dpi
		72          // y dpi
	);

	auto & m = face->size->metrics;
	I4 f_asc = m.ascender >> 6;
	I4 f_desc = m.descender >> 6;
	I4 f_height = m.height >> 6;
	I4 f_width = f_height / 2;
	if (verbose>0){
		print("ascender {}\n", f_asc);
		print("descender {}\n", f_desc);
		print("height {}\n", f_height);
		print("width {}\n", f_width);
	}
	print("INFO: write rc header\n");
	print(&rc_file, "sheet _\n");
	print(&rc_file, "name \"{}\"\n", fontname);
	print(&rc_file, "max_sprites 128\n");
	print(&rc_file, "height {}\n", f_height);
	print(&rc_file, "width {}\n", f_width);
	print(&rc_file, "asc {}\n", f_asc);
	print(&rc_file, "des {}\n", f_desc);
	print(&rc_file, "pad 2 0\n");
	print(&rc_file, "\n");
	/*FT_CALL(FT_Set_Pixel_Sizes,
		face,
		0,      // pix width
		16      // pix height
	);*/
	print("INFO: begin main loop\n");
    auto chars=str(
		" !\"#$%&'()*+,-./"
		"0123456789"
		":;<=>?@"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"[\\]^_`"
		"abcdefghijklmnopqrstuvwxyz"
		"{|}~"
	);
	// out image
	//I4 xx=pixsize*16, yy=pixsize*16;
	I4 const xx=1024, yy=1024;
	print("INFO: output image dim: {} {}\n", xx, yy);
	Arr<U8,xx*yy> out{0}; DOi(xx*yy, out[i]=255);
	// loop chars
    Arr<I4,2> sep{4,4};
	Arr<I4,2> pos=sep;
	Arr<I4,2> gly_pos{0};
	I4 curr_line_height=0;
	for (auto charcode: chars){
		// render glyph
		print("INFO: render glyph: {}\n", I4(charcode));
		auto glyph_index=FT_Get_Char_Index(face, charcode);
		FT_CALL(FT_Load_Glyph, face, glyph_index, FT_LOAD_DEFAULT);
		FT_CALL(FT_Render_Glyph, face->glyph, FT_RENDER_MODE_NORMAL);
		// glyph
		auto gly = face->glyph;
		auto * met = &gly->metrics;
		auto & bm = gly->bitmap;
		auto adv = gly->advance.x >> 6;
		// increase position
		if (bm.width>0 and bm.rows>0){
			curr_line_height = std::max(curr_line_height, I4(bm.rows));
			if (pos[0]+sep[0]+bm.width>=xx) {
				pos[0]=sep[0];
				pos[1]+=sep[1]+curr_line_height;
				curr_line_height=0;
			}
			gly_pos=pos;
			pos[0]+=bm.width+sep[0];
		}
		// paste to output
		copy_bitmap_to(out.rng(), {xx,yy}, gly_pos, bm);
		// rc file
		print(&rc_file, "sprite {} ", I4(charcode));
		if (charcode == '\\' or charcode == '\"'){
			print(&rc_file, "name \"\\{}\" ", charcode);
		}
		else {
			print(&rc_file, "name \"{}\" ", charcode);
		}

		print(&rc_file, "box {} {} {} {} ",
			gly_pos[0], yy-gly_pos[1]-I4(bm.rows), I4(bm.width), I4(bm.rows)
		);
		print(&rc_file, "rbox {} {} {} {} ",
			I4(gly->bitmap_left), I4(-gly->bitmap_top + f_asc),
			I4(bm.width), I4(bm.rows)
		);
		print(&rc_file, "adv {}\n", I4(gly->advance.x >> 6));

		// show debug info
		if (verbose >= 2) {
			pr1("charcode ")(I4(charcode))("\n");
			pr1(" width ")(met->width)("\n");
			pr1(" height ")(met->height)("\n");
			pr1(" bearing_x ")(met->horiBearingX)("\n");
			pr1(" bearing_y ")(met->horiBearingY)("\n");
			pr1(" advance ")(met->horiAdvance)("\n");

			pr1(" bitmap_left ")(gly->bitmap_left)("\n");
			pr1(" bitmap_top ")(gly->bitmap_top)("\n");
			pr1(" advance[0] ")(gly->advance.x >> 6)("\n");
			pr1(" advance[1] ")(gly->advance.y >> 6)("\n");

			pr1(" bitmap\n");
			pr1("  rows ")(bm.rows)("\n");
			pr1("  width ")(bm.width)("\n");
			pr1("  pitch ")(bm.pitch)("\n");
			pr1("  num_grays ")(bm.num_grays)("\n");
			pr1("  pixel_mode ")(I4(bm.pixel_mode))("\n");
			pr1("  palette_mode ")(I4(bm.palette_mode))("\n");}
	}
	rc_file.close();
	// save png
	auto name_png=format(tmp.rng(), "res/{}.png", fontname);
	pr1("I write output image:")(name_png)("\n");
	png_save_grayscale(out.rng(), {xx,yy}, name_png);
	return 0;
}
