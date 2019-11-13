#pragma once
#include"lodepng/lodepng.h"
#include"ext/mat.hpp"
namespace ext{
	// show metainfo
	V png_display_info(LodePNGInfo const& info);
	// 8bit grayscale png -> [0,1] F4 2D array
	V png_load_gray8b(MTen<U1,2> &ret, Rng<H> path);
	// [0,1] F4 2D array -> 8bit grayscale png
	V png_save_gray8b(Tsl<U1,2> img2, Rng<H> path);
	V png_save_rgb8b(Tsl<Arr<U1,3>,2> img2, Rng<H> path);
}
