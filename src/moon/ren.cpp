#include "ren.hpp"
namespace ext{
	V init_glew(){
		auto x=glewInit();
		if(x!=GLEW_OK) pr2("E GLEW:")((HC*)glewGetErrorString(x))('\n'), A(x==GLEW_OK);
		AGL();
		// show available extensions
		if(0){
			I4 n; glGetIntegerv(GL_NUM_EXTENSIONS, &n);
			pr1("I GLExtensions:");
			DO(i,n) pr1((HC*)glGetStringi(GL_EXTENSIONS,i))(' ');
			pr1('\n');}
		if(glewIsSupported("GL_KHR_debug")) pr1("I GL_KHR_debug available\n");}
	/*
	// translate, rotate, scale, color
	ren.pos(), ren.rot(), ren.scl(), ren.col()
	render_rect(ren, pos, dim, col)
	render_frame
	render_line(ren, p0, p1, col, th) // arbitrary line
	render_glyph(ren, ss, spi, pos, col)
	render_text(ren, ss, pos, col, text)
	render_sprite(ren, ss, spi, col)
	render_sprite(ren, ss, spi, dim, col)
	*/
	// load texture distance field
	V Ren::load_tdf(U4 texu, Rng<U1> data, V2<I4> dim){
		A(tex[texu]!=0), A(data.len()==dim[0]*dim[1]);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);   AGL();
		glActiveTexture(GL_TEXTURE0+texu);       AGL();
		glBindTexture(GL_TEXTURE_2D, tex[texu]); AGL();
		// target level internal_format width height
		// border(0) format type data
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_R8, dim[0], dim[1],
			0, GL_RED, GL_UNSIGNED_BYTE, data.ptr()
		); AGL();
		// linear approx, no wrap
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		AGL();
	}
	// load sprite geometries
	V Ren::load_spg(U4 texu, Rng<F4> data, I4 len){
		// data:xyuv*4*nsprite
		A(data.len()%16==0), A(data.len()/16==len);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[texu]);
		glBufferData(GL_ARRAY_BUFFER, data.len()*sizeof(F4), data.ptr(), GL_DYNAMIC_DRAW);
		AGL();

		pr1("I load_spg tex:")(texu)("\n");
		DO(i,data.len()/16){
			auto r=rng(data, i*16, 16);
			pr1("I load_spg:")(i)(": ")(r.rng())("\n");
		}
		// verify uv coords in [0,1]
		DO(i,data.len()/4){
			auto r=rng(data, i*4, 4);
			F4 x=r[0], y=r[1], u=r[2], v=r[3]; UNUSED(x); UNUSED(y);
			A(0.0f<=u), A(u<=1.0f), A(0.0f<=v), A(v<=1.0f);}}
	Ren& Ren::set_window_size(V2<F4> wd){
		A(ctx_dim[1]>0);
		// ctx_dim[1] -- hold fixed
		win_dim=wd;
		ctx_dim[0]=(win_dim[0]/win_dim[1])*ctx_dim[1];
		pr1("I ctx_dim:=")(ctx_dim.rng())("\n");
		load_projection_matrix(ctx_dim[0], ctx_dim[1]);
		glUniformMatrix4fv(
			myUniLoc(prog[0], "u_proj"),
			1,
			GL_FALSE,
			this->proj.ptr()
		); AGL();
		glViewport(0, 0, i4(win_dim[0]), i4(win_dim[1]));
		R *this;}
	V Ren::load_projection_matrix(F4 w, F4 h){
		// 2D view
		// bottom-left pixel is (0,0)
		// +x goes right
		// +y goes up
		// orto
		Arr<F4,16> p{
			2.0f/w,  0.0f,    0.0f,     0.0f,
			0.0f,    2.0f/h,  0.0f,     0.0f,
			0.0f,    0.0f,   -2.0f/0.2, 0.0f,
		    0.0f,    0.0f,    0.0f,     1.0f,
		};
		cp(proj.rng(), p.rng());}
	Ren& Ren::create(HC* title, V2<F4> win_dim, V2<F4> ctx_dim){
		// we need this to convert mouse click locations
		th ctx_dim=ctx_dim;
		th win_dim=win_dim;

		// create SDL window with GL context
		auto r=SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER);         ASDL(!r);
		r=SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);    ASDL(!r);
		r=SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3); ASDL(!r);
		r=SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0); ASDL(!r);
		r=SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);          ASDL(!r);
		//r=SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);           ASDL(!r);
		this->win=SDL_CreateWindow(title,
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			I4(win_dim[0]), I4(win_dim[1]),
			SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE
		); ASDL(th win);
		this->ctx=SDL_GL_CreateContext(this->win); ASDL(th ctx);
		// init GL
		init_glew();

		// framebuffer
		glGenFramebuffers(1,fbo);



		// program
		prog[0]=glCreateProgram();
		myAttachShader(prog[0], GL_VERTEX_SHADER, shader::vert0);
		myAttachShader(prog[0], GL_FRAGMENT_SHADER, shader::frag0);
		myLinkProgram(prog[0]); AGL();
		glUseProgram(prog[0]); AGL();
		// setup vertex_arrays[0..N] => x y u v
		glGenVertexArrays(N_TEX, vao.ptr()); AGL();
		glGenBuffers(N_TEX, vbo.ptr());      AGL();
		DO(i,N_TEX){
			glBindBuffer(GL_ARRAY_BUFFER, vbo[i]); AGL();
			glBindVertexArray(vao[i]);             AGL();
			{
				auto loc=myAttLoc(prog[0], "a_xy");
				glEnableVertexAttribArray(loc);
				glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 4*sizeof(F4), (V*)0);
				AGL();
			}
			{
				auto loc=myAttLoc(prog[0], "a_uv");
				glEnableVertexAttribArray(loc);
				glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 4*sizeof(F4), (V*)(2*sizeof(F4)));
				AGL();
			}
		}
		// enable blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,  GL_ONE_MINUS_SRC_ALPHA);
		// create texture ids
		glGenTextures(tex.len(), tex.ptr()); AGL();

		set_window_size(win_dim);
		R *this;}
	V Ren::destroy(){

		// GL cleanup
		glDeleteBuffers(vbo.len(), vbo.ptr());      AGL();
		glDeleteVertexArrays(vao.len(), vao.ptr()); AGL();
		glDeleteProgram(prog[0]);                   AGL();
		glDeleteTextures(tex.len(), tex.ptr());     AGL();
		glDeleteFramebuffers(fbo.len(), fbo.ptr()); AGL();
		// SDL clear context and window
		SDL_GL_DeleteContext(th ctx);
		SDL_DestroyWindow(th win);
		SDL_Quit();}
	B Ren::handle(SDL_Event & o){
		if(o.type==SDL_WINDOWEVENT){
			if(o.window.event==SDL_WINDOWEVENT_SIZE_CHANGED or o.window.event==SDL_WINDOWEVENT_RESIZED){
				auto wd=V2<F4>{f4(o.window.data1), f4(o.window.data2)};
				if(wd!=th win_dim) {
					set_window_size(wd);
					pr1("I window resized to ")(win_dim.rng())('\n');}
				R 1;}}
		el(o.key.type==SDL_KEYDOWN){
			if(o.key.repeat==0 and o.key.keysym.sym==SDLK_F12){
				auto win_dim_i=V2<I4>(win_dim);
				MTen<V3<U1>,2> mt{win_dim_i};
				glPixelStorei(GL_PACK_ALIGNMENT,1); AGL();
				glReadPixels(0,0, win_dim[0], win_dim[1], GL_RGB, GL_UNSIGNED_BYTE, (V*)mt.ptr());
				AGL();
				png_save_rgb8b(rev(mt.tsl()), strz("screen.png"));
				pr1("I screenshoot:screen.png\n");
				R 1;}}
		R 0;}
	//V2<I16> Ren::get_window_dim() const {
	//	I32 w,h; SDL_GetWindowSize(win, &w, &h); R V2<I16>(w,h);
	//}

	/*translate * mat
	render_rect(ren, pos, dim, col)
	render_frame
	render_line(ren, p0, p1, col, th) // arbitrary line
	render_glyph(ren, ss, spi, pos, col)
	render_text(ren, ss, pos, col, text)
	render_sprite(ren, ss, spi, col)
	render_sprite(ren, ss, spi, dim, col)
	*/





	Ren& Ren::render(U4 texu, I4 texi, I4 mode){
		//pr1("render")(texu)(' ')(texi)(' ')(p.rng())(' ')(r.rng())(' ')(s.rng())("\n");

		//glBindBuffer(GL_ARRAY_BUFFER, vbo[texu]);
		//glBufferData(GL_ARRAY_BUFFER, data.len()*sizeof(F4), data.ptr(), GL_DYNAMIC_DRAW);
		//AGL();

		glUniform1i(myUniLoc(prog[0], "u_base"), texu);
		glUniform1i(myUniLoc(prog[0], "u_mode"), mode);
		glUniform2f(myUniLoc(prog[0], "p"), aff().p[0], aff().p[1]);
		glUniform2f(myUniLoc(prog[0], "r"), aff().r[0], aff().r[1]);
		glUniform2f(myUniLoc(prog[0], "s"), aff().s[0], aff().s[1]);
		glUniform4f(myUniLoc(prog[0], "u_rgba"),
			F4(aff().c[0])/F4(255), F4(aff().c[1])/F4(255), F4(aff().c[2])/F4(255), aff().c[3]/F4(255));
		glActiveTexture(GL_TEXTURE0+texu), AGL();
		glBindBuffer(GL_ARRAY_BUFFER, vbo[texu]), AGL();
		glBindVertexArray(vao[texu]), AGL();
		glDrawArrays(GL_TRIANGLE_FAN, texi*4, 4);
		AGL();
		R *this;}



	Ren& Ren::clear(){
		glClearColor(F4(aff().c[0])/F4(255), F4(aff().c[1])/F4(255), F4(aff().c[2])/F4(255), aff().c[3]/F4(255)); AGL();
		glClear(GL_COLOR_BUFFER_BIT); AGL(); R *this;}


	/*V Ren::render_rect(U4 texu, I4 texi, V2<F4> p, V2<F4> r, V2<F4> s, Arr<U1,3> c){
		//pr1("render")(texu)(' ')(texi)(' ')(p.rng())(' ')(r.rng())(' ')(s.rng())("\n");

		//glUniform1i(myUniLoc(prog[0], "u_base"), 0);
		glUniform1i(myUniLoc(prog[0], "u_mode"), 0);

		glUniform4f(myUniLoc(prog[0], "u_rgba"),
			F4(c[0])/255.0f, F4(c[1])/255.0f, F4(c[2])/255.0f, 0.8f);

		glUniform2f(myUniLoc(prog[0], "p"), p[0], p[1]);
		glUniform2f(myUniLoc(prog[0], "r"), r[0], r[1]);
		glUniform2f(myUniLoc(prog[0], "s"), s[0], s[1]);

		glUniform4f(myUniLoc(prog[0], "u_rgba"),
			F4(aff().c[0])/F4(255), F4(aff().c[1])/F4(255), F4(aff().c[2])/F4(255), aff().c[3]/F4(255));

		//glActiveTexture(GL_TEXTURE0+texu), AGL();
		glBindBuffer(GL_ARRAY_BUFFER, vbo[texu]), AGL();
		glBindVertexArray(vao[texu]), AGL();
		glDrawArrays(GL_TRIANGLE_FAN, texi*4, texi*4+4);
		AGL();}*/
}//ext


