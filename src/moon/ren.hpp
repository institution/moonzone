#pragma once
#include<unistd.h> //usleep
#include<SDL2/SDL.h>
#include"my.hpp"
#include"shader.hpp"
#include"ext/png.hpp"
#include"aff.hpp"
// 1 window and gl create with distance field rendering
// 2 have sprite concept and text rendering
// 3 can load sprites from ss files
// texture == spritesheet
// window is kindof widget?
// or all functions?
// real window dim for ex. 1920 1200
// outside dim: [pix]
// inside dim: [unit]
// idim
// odim
// window odim: 1920 1200
// window idim: 320 200
//after resize
// window odim: 1920 600
// window idim: 320 200
// only scaling change
// logical units
//  game dim 320 200
//  sprite
//   real 160 100
//   game 320 200
//   uvab 0 0 1 1
//   sheet 1
// ss_format {
//  U32{1} -- byte order mark
//  Sheet
//  Sprite[sheet.max_sprites]
//  U8[sheet.twh[0]*sheet.twh[1]] -- tex with distance field
#define ASDL(x) {if(!x) pr2("E SDL:")(SDL_GetError())('\n'),SDL_Quit(); A(x);}
namespace ext{

	V init_glew();
	// timer
	struct Timer{
		I4 t1{0};
		V  set(I4 delay){t1=(I4)SDL_GetTicks()+delay;}
		I4 left()C      {R t1-(I4)SDL_GetTicks();}
		V  wait()       {if(auto l=left(); l>0) A(usleep(l*1000)==0);}
	};
	// ren
	I4 C N_TEX=16;
	struct Ren{
		// logical and physical dimensions
		// logical dim is const while resizing (stretch)
		// handle window resizeing
		// rendering distance fields
		// store textures and sprite geometry on gpu
		// transformations: move rotate colorize

		// window & context
		SDL_Window *win{0};
		SDL_GLContext ctx;
		V2<F4> win_dim{0,0};  // physical dim
		V2<F4> ren_dim{0,0};  // render to dim
		V2<F4> ctx_dim{0,0};  // logical dim
		// opengl stuff
		Arr<F4,16> proj; // projection matrix
		Arr<U4,N_TEX> tex{}; //indexed by texu
		Arr<U4,N_TEX> vao{};
		Arr<U4,N_TEX> vbo{};
		Arr<U4,1> prog{};
		//
		Arr<Aff,4> affs{};
		I4 affi=0;
		Aff& aff(){R affs[affi];}
		//
		Ren()=default;
		Ren(Ren C&)=delete;
		//
		Ren& create(HC* title, V2<F4> win_dim, V2<F4> ctx_dim);
		V destroy();
		Ren& clear();
		Ren& flip(){SDL_GL_SwapWindow(win);R *this;}
		V load_projection_matrix(F4 w, F4 h);

		V load_tdf(U4 texu, Rng<U1> data, V2<I4> dim);
		V load_spg(U4 texu, Rng<F4> data, I4 len);

		B handle(SDL_Event & o);

		Ren& render(U4 texu, I4 texi, I4 mode=1);

		//Ren& col(Arr<U1,3> c){aff().col(c);R *this;}
		Ren& col(Arr<U1,4> c){aff().col(c);R *this;}
		Ren& pos(Vf x){aff().pos(x);R *this;}
		Ren& rot(Vf x){aff().rot(x);R *this;}
		Ren& scl(Vf x){aff().scl(x);R *this;}
		Ren& posl(Vf x){aff().posl(x);R *this;}
		Ren& rotl(Vf x){aff().rotl(x);R *this;}
		Ren& scll(Vf x){aff().scll(x);R *this;}
		Ren& push(){affs[affi+1]=affs[affi]; ++affi; R *this;}
		Ren& pop(){A(affi>0); --affi; R *this;}
		Ren& rst(){aff().rst();R *this;}

		Ren& set_window_size(V2<F4> wd);

		//V render_circle(U4 texu, I4 texi, V2<F4> p, V2<F4> r, V2<F4> s, Arr<U1,3> c);
	};




}//ext
