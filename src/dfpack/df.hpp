#pragma once
#include"ext/mat.hpp"
namespace ext{
	V convf (Rng<F4> rs, Rng<U1> xs);
	V sdt(Tsl<F4,2> f);
	V downscale8x(MTen<F4,2> &img1x, Tsl<F4,2> img8x);
	V cn(Tsl<F4,2> f2, F4 mx);
	V conv8b(Rng<U1> rs, Rng<F4> xs);
}//ext
