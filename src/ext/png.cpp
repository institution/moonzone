#include"png.hpp"
namespace ext{
	// show metainfo
	V png_display_info(LodePNGInfo const& info){
		/*LodePNGColorMode const& color = info.color;
		pr1("I Compression method: ")(info.compression_method)('\n');
		pr1("I Filter method: ")(info.filter_method)('\n');
		pr1("I Interlace method: ")(info.interlace_method)('\n');
		pr1("I Color type: ")(color.colortype)('\n');
		pr1("I Bit depth: ")(color.bitdepth)('\n');
		pr1("I Bits per pixel: ")(lodepng_get_bpp(&color))('\n');
		pr1("I Channels per pixel: ")(lodepng_get_channels(&color))('\n');
		pr1("I Is greyscale type: ")(lodepng_is_greyscale_type(&color))('\n');
		pr1("I Can have alpha: ")(lodepng_can_have_alpha(&color))('\n');
		pr1("I Palette size: ")(color.palettesize)('\n');
		pr1("I Has color key: ")(color.key_defined)('\n');
		if(color.key_defined){
			pr1("I Color key: ")(color.key_r)(' ')(color.key_g)(' ')(color.key_b)('\n');}
		else{
			pr1("I Color key: undefined")('\n');*/}
	// general png save
	V png_save(U1 *image, U4 width, U4 height, LodePNGColorType colortype, U4 bitdepth, Rng<H> path){
		unsigned char *png=0;
		unsigned err=0;
		size_t pngsize=0;
		LodePNGState state;	lodepng_state_init(&state);
		state.encoder.auto_convert=0;
		state.info_png.color.colortype=colortype;
		state.info_png.color.bitdepth=bitdepth;
		state.info_raw.colortype=colortype;
		state.info_raw.bitdepth=bitdepth;	
		if(!err) err=lodepng_encode(&png, &pngsize, image, width, height, &state);
		if(err) pr2("E lodepng:")(lodepng_error_text(err))("\n"), A(!err);
		lodepng_save_file(png, pngsize, cstr(path));
		lodepng_state_cleanup(&state);
		free(png);}
	// U1 2D array -> RGB8 png
	V png_save_rgb8b(Tsl<Arr<U1,3>,2> img2, Rng<H> path){
		I4 nbpix=3;	
		U4 width=img2.len(0), height=img2.len(1);
		U1 *image=(U1*)malloc(width*height*nbpix);
		DO(j,img2.len()){
			auto img1=img2[j]; DO(i,img1.len()){
				cp(Rng<U1>{image+((width*j+i)*nbpix), nbpix}, img1[i].rng());}}
		png_save(image, width, height, LCT_RGB, 8, path);
		free(image);}
	// 8bit grayscale png -> U1 2D array
	V png_load_gray8b(MTen<U1,2> &ret, Rng<H> path){
		unsigned char *png=nullptr, *image=nullptr;
		size_t pngsize=0;
		unsigned width=0, height=0, err=0;
		LodePNGState state; lodepng_state_init(&state);
		state.decoder.color_convert=0;
		if(!err) err=lodepng_load_file(&png, &pngsize, cstr(path));
		if(!err) err=lodepng_decode(&image, &width, &height, &state, png, pngsize);
		if(err) pr2("E lodepng:")(lodepng_error_text(err)).n(),A(!err);
		A(state.info_png.color.colortype==LCT_GREY), A(state.info_png.color.bitdepth==8);
		free(png);
		ret.alc({i4(width),i4(height)});
		{auto a=ret.rng(); DO(i,a.len()) a[i]=image[i];}
		lodepng_state_cleanup(&state);
		free(image);}
	// U1 2D array -> 8bit grayscale png
	V png_save_gray8b(Tsl<U1,2> img2, Rng<H> path) {
		unsigned width=img2.len(0), height=img2.len(1), err=0;
		unsigned char *image=(unsigned char*)malloc(width*height), *png=nullptr;
		size_t pngsize=0;
		DO(j,img2.len()){
			auto img1=img2[j]; DO(i,img1.len()){
				image[width*j+i]=img1[i];}}
		LodePNGState state;	lodepng_state_init(&state);
		state.encoder.auto_convert=0;
		state.info_png.color.colortype=LCT_GREY;
		state.info_png.color.bitdepth=8;
		state.info_raw.colortype=LCT_GREY;
		state.info_raw.bitdepth=8;
		if(!err) err=lodepng_encode(&png, &pngsize, image, width, height, &state);
		if(err) pr2("E lodepng:")(lodepng_error_text(err)).n(),A(!err);
		lodepng_save_file(png, pngsize, cstr(path));
		lodepng_state_cleanup(&state);
		free(png);
		free(image);}
}//ext
