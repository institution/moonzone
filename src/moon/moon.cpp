#include"ren.hpp"
#include"ext/ss.hpp"
#include"ext/png.hpp"
#include"aff.hpp"

B nan(Vf x){R isnan(x[0]) or isnan(x[1]);}

// graphics vertical resolution
// -1:equal to window h; 0<:set to fixed value, ex. 128,1080
F4 C VRes=-1;  // not implemented
B  C ScrollPastEdge=0;

I4 C TexuUnit{2}, TexuTerr{1};

// strategy(AI)+roguelike(?)+action(physsim) ... battlezone, Moonzone Commander

// colors
Arr<U8,3> C
	CBlack  {  0,   0,   0},
	CWhite  {255, 255, 255};
Arr<U8,3> C
	CText   {255, 255, 255},
	CGround {  0, 128,   0},
	CWater  {  0,   0, 128},
	CFriend {  0,   0, 255},
	CEnemy  {255,   0,   0},
	CNeutral{128, 128, 128};


using Type=U8;
Type C
	TypeNone{0},
	TypeChap{1},
	TypeHelo{2},
	TypeDHelo{3},   // double helo -- transport heli or gunship
	TypeSam{4},
	TypeTank{5},
	TypeCT{6};      // Control Tower

// default factions: None{0}, Blue{1} (player side), Red{2} (enemy)


V render_line(Ren &ren, Vf p1, Vf p0){
	Vf v=p1-p0;
	F4 l=mag(v);
	Vf pos=v/2.0f;
	ren.push().scl({l/2.0f,0.2f}).rotl({v[0]/l,v[1]/l}).pos(pos).render(10,0,0).pop();
}

F4 dd=0;
Vf dn={0.0f,0.0f};
Vf Dimvel={0.0f,0.0f};

struct Obj{
	Vf ab;  //aabb

};


// game object
struct Vtol {
	I4 texu{0}, texi{0}; // texture unit&index

	Vf pos{0,0}, vel{0,0}, acc{0,0}, frc{0,0};
	Vf engine_rot{1,0};
	F4 mss{1.0f};

	F4 thr{0.0};
	//v,rv, //dynamic prop



	U1 fac{0}; // faction id
	U1 ply{0}; // player(can be AI) id
	U1 alive{0};
	U1 _pad1;

	F4 fuel{0};
	F4 hit{0};


	I2 up{0};     // parent, vehicle
	I2 down{0};   // child,  pilot
	//I16 right{0};  // linked list
	//I16 left{0};

	F4 dir{0.0};

	V thrust(F4 x){thr=x;}
	V steer(F4 dir){th dir=dir;}

	V update(F4 dt){
		// steer engine
		engine_rot=rotapp(engine_rot, make_rot(dir*dt));
		// apply engine force
		frc=frc+rotvec(engine_rot, Vf{0.0f, thr*40.0f});

		sim(dt);
		clamp_pos();
	}

	V sim(F4 dt){
		D(A(!nan(frc)));
		// sim
		acc=frc/mss;
		vel=vel+acc*dt;
		pos=pos+vel*dt;

		//pr1("I displ:")(mag(vel)*dt)("\n");
	}

	V clamp_pos(){
		// clamp position
		// 0.5+eps : 127.5-eps  is safe
		F4 C e=0.01;
		if(pos[1]<0.5f+e){pos[1]=0.5f+e; vel[1]=0;}
		if(pos[1]>127.5f-e){pos[1]=127.5f-e; vel[1]=0;}
		if(pos[0]<0.5f+e){pos[0]=0.5f+e; vel[0]=0;}
		if(pos[0]>2047.5f-e){pos[0]=2047.5f-e; vel[0]=0;}
	}

	V render(Ren &ren){
		F4 sx=vel[0]>0?-1:1;

		//if(thr>0.0) ren.push().posl({0,-2}).col({255,255,0,255}).render(TexuUnit,8).pop();

		ren.push();
		ren.col({0x93,0xA1,0xAC,255}).scll({sx,1.0}).pos(pos).render(TexuUnit,0).scll({sx,1.0});

		ren.push();
		ren.rotl(engine_rot).col({0x42,0x50,0x58,255}).render(TexuUnit,1);

		if(thr>0.0) ren.posl({0,-2}).col({255,255,0,255}).render(TexuUnit,8);
		//if(thr>0.0) ren.push().posl({0,-2}).col({255,255,0,255}).render(8,0).pop();
		ren.pop();




		/*{
		Vf p{0,0},q{10,5};
		Vf v=q-p;
		F4 l=dist(v);
		Vf pos=v/2.0f;
		ren.push().scl({l/2.0f,0.2f}).rotl(make_rot(v)).pos(pos).col({255,0,0,255}).render(10,0,0).pop();
		}*/


		ren.col({255,0,0,255});
		render_line(ren, vel, {0,0});

		ren.col({255,255,0,255});
		render_line(ren, dn*10.0f, {0,0});

		ren.col({0,0,255,255});
		render_line(ren, Dimvel, {0,0});



		ren.pop();
	}

};


// convert 0-255 -> pix -> unit
Inl F4 conv(F4 x, F4 upp){R (x*(8.0f/255.0f)-4.0f)*upp;}

// value and derivate of df at z in [pix]
V sample_unit(F4 &v, Vf &dv, Tsl<U1,2> tex, Vf z, F4 upp){
	F4 x0;
	F4 y0;
	F4 x=modf(z[0], &x0);
	F4 y=modf(z[1], &y0);

	F4 x1=x0+1;
	F4 y1=y0+1;

	F4 v00=conv(tex[I4(y0-0.5f)][I4(x0-0.5f)], upp);
	F4 v10=conv(tex[I4(y0-0.5f)][I4(x1-0.5f)], upp);
	F4 v11=conv(tex[I4(y1-0.5f)][I4(x1-0.5f)], upp);
	F4 v01=conv(tex[I4(y1-0.5f)][I4(x0-0.5f)], upp);

	F4 vb=(v10-v00)*x+v00;
	F4 vl=(v01-v00)*y+v00;
	F4 vt=(v11-v01)*x+v01;
	F4 vr=(v11-v10)*y+v10;

	v=(vr-vl)*x+vl;
	//D(A(v==(vt-vb)*y+vb);
	dv={(vr-vl),(vt-vb)};

	//v=v00;
	//dv=Vf{v10-v00, v01-v00};

	//pr1("I ")("x0:")(x0)(" x:")(x)(" y0:")(y0)(" y:")(y)("\n");
	//pr1("I ")("v00:")(v00)(" v10:")(v10)(" v11:")(v11)(" v01:")(v01)("\n");
	//pr1("I ")("z:")(z.rng())(" d:")(v)(" dv:")(dv.rng())("\n");
	}



struct App{
	Ren ren;

	Sheet unit{TexuUnit};
	Sheet terr{TexuTerr};

	Vtol vtol;

	Arr<F4,2> mouse_pos; // mouse pos in game units
	Vf cam_pos{0,64};       // 0(max_left), 1(max_right)


	B stop{false};     // app is running

	App()=default;
	~App(){ren.destroy();}

	V init();
	V update(F4 dt);
	V render();
	B handle(SDL_Event &e);

	V load_sheet(Sheet &s, Rng<H> path_ss, Rng<H> path_png);
	V load_sheet(Sheet &s);



	// ret distance to terr surface in units clamped to (4*upp), negative outside, positive inside
	F4 collision_terr(Vf &dv, Vf p){
		// TODO: linear approx of field
		F4 v;
		Vf z={p[0],128.0f-p[1]};
		sample_unit(v, dv, terr.tex.tsl(), z, terr.spr[0].upp);
		dv[0]=-dv[0];
		R v;}

};



V App::load_sheet(Sheet &s){
	A(s.texu!=-1);

	F4 tX=s.tex.dim()[0];
	F4 tY=s.tex.dim()[1];

	auto n=s.spr.len();
	MRng<F4> buf(n*16);
	DO(i,n){
		auto &e=s.spr[i];
		F4 u0=e.t0;
		F4 v0=e.t1;
		F4 u1=(e.t0+e.t2);
		F4 v1=(e.t1+e.t3);

		F4 x0=e.r0;
		F4 y0=-(e.r1);        // flip to match screen coords???
		F4 x1=(e.r0+e.r2);
		F4 y1=-(e.r1+e.r3); // flip to match screen coords???

		Arr<F4,16> tmp{x0,y0,u0,v0, x1,y0,u1,v0, x1,y1,u1,v1, x0,y1,u0,v1};
		//Arr<F4,16> tmp{x0,y1,u0,v1, x1,y1,u1,v1, x1,y0,u1,v0, x0,y0,u0,v0};

		cp(buf.rng(i*16,16), tmp.rng());
		//pr1("I sprgeo ")(i)(":")(tmp.rng())("\n");
		// if tex attached and sprite texbox nonzero
		if(tX!=0 and tY!=0 and e.t2!=0 and e.t3!=0 and e.upp){
			// calc units per pixel
			F4 uppX=e.r2/F4(e.t2*tX);
			F4 uppY=e.r3/F4(e.t3*tY);
			A(uppX==uppY);
			A(e.upp==uppX);
			// all sprites on one tex should be in the same scale
			A(s.spr[0].upp=e.upp);}}
	ren.load_spg(s.texu, buf.rng(), n);
	ren.load_tdf(s.texu, s.tex.rng(), s.tex.dim());}

V App::load_sheet(Sheet &s, Rng<H> path_ss, Rng<H> path_png){
	pr1("I load ")(path_ss)("\n");
	load(MFd(path_ss).fd())(s);


	/*DO(i,s.spr.len()){
		pr1(s.spr[i])("\n");
	}*/

	pr1("I load ")(path_png)("\n");
	png_load_gray8b(s.tex, path_png);
	load_sheet(s);}


V App::init(){
	pr1("I init\n");

	// keep virtual vres:256 [u]
	ren.create("Moonzone Commando", {196*4,128*4}, {196,128}).clear().flip();

	load_sheet(unit, strz("res/unit.ss"), strz("res/unit.df.png"));
	load_sheet(terr, strz("res/terr.ss"), strz("res/terr.df.png"));

	// line?
	Sheet s{10}; s.max_sprites(1).sprite(0).box(0,0,1,1).bea(-1,-1).dim(2,2);
	load_sheet(s);

	{
		Sheet s{8}; s.max_sprites(1).sprite(0).box(0, 0.125, 0.125, 0.125).bea(-15.5,-15.5).dim(32,32);
		png_load_gray8b(s.tex, strz("res/unit.df.png"));
		pr1("I sheet 8:")(s.spr[0],0)("\n");
		load_sheet(s);
	}

	Sheet unit1{7};
	load_sheet(unit1, strz("res/unit1.ss"), strz("res/unit1.df.png"));


	//Sheet terr{1}; terr.max_sprites(1).sprite(0).box(0,0,2048,128).bea(0,0).dim(2048,128);

	// init units
	vtol.pos={64,64};
	vtol.alive=1;
}

V body_update(App &world, Vtol &body){


}

V App::update(F4 dt){
	//pr1("I dt:")(dt)("\n");
	U1 C* key=SDL_GetKeyboardState(0);


	cam_pos[0]=vtol.pos[0];
	if(!ScrollPastEdge){
		Vf cam_lim{ren.ctx_dim[0]/2.0f, 2048.0f-ren.ctx_dim[0]/2.0f};
		cam_pos[0]=clamp(cam_pos[0], cam_lim[0], cam_lim[1]);}


	D(A(!nan(vtol.frc)));


	// gravity
	vtol.frc=Vf{0.0f,-10.0f*vtol.mss};

	// terrain collision response
	{
		Vf nm;
		Dimvel={0.0f,0.0f};
		F4 d=collision_terr(nm, vtol.pos);
		dd=d; dn=nm;
		F4 m=mag(nm);
		if(0<=d and m){
			Vf n=nm/m;
			Vf s=n*d;

			F4 c=sum(n*vtol.vel);
			if(c<0){
				//impact velocity
				Vf imvel=c*n;
				Dimvel=imvel;
				// strong response
				F4 k=1000.0f;
				//critically damped system
				vtol.frc=n*k*d-2.0f*sqrtf(vtol.mss*k)*imvel;
			}

			/*if(0){
				// force to neurtalize other forces
				vtol.frc={0.0f, 0.0f};
				// cancel impact velocity
				vtol.frc=vtol.frc-imvel*vtol.mss/dt;
				// force to move out of terr
				vtol.frc=vtol.frc+(s*vtol.mss)/dt*dt;
			}*/

			// limit position
			//vtol.pos=vtol.pos+n*d;

			D(A(!nan(vtol.frc)));
		}
	}

	D(A(!nan(vtol.frc)));

	vtol.steer(0.0);
	if(key[SDL_SCANCODE_LEFT])  vtol.steer(+2.0);
	el(key[SDL_SCANCODE_RIGHT]) vtol.steer(-2.0);

	vtol.thrust(0.0);
	if(key[SDL_SCANCODE_UP])   vtol.thrust(1.0);
	//el(key[SDL_SCANCODE_DOWN]) vtol.thrust(0.1);

	//F4 sf=dt*0.1f;
	//if(key[SDL_SCANCODE_A])   u.kin.scl(Vf{1.0f+sf, 1.0f+sf});
	//el(key[SDL_SCANCODE_Z])   u.kin.scl(Vf{1.0f-sf, 1.0f-sf});


	D(A(!nan(vtol.frc)));

	vtol.update(dt);

}



V App::render(){
	//pr1("I cam ")(cam_pos.rng())("\n");
	//pr1("I vtol ")(vtol.pos.rng())("\n");

	Timer t; t.set(0);


	I4 x,y;
	SDL_GetMouseState(&x, &y);
	//mouse_pos = get_ctx_pos({x,y});

	// sky
	ren.col({0x87,0xCE,0xEB,255}); //SkyBlue
	ren.clear();
	ren.rst();

	ren.pos({-cam_pos[0],-cam_pos[1]});



	// terrain
	ren.push().col({0x60,0x80,0x38,255});
	ren.pos({0,128}).render(TexuTerr,0);
	ren.pop();


	vtol.render(ren);

	//ren.push().col({200,0,0,255});
	//ren.pos({20,80}).render(8,0);
	//ren.pop();


	DO(i,2){
		ren.push().pos({i*32,32}).col({200,0,0,255}).render(7,i).pop();
	}

	//auto r=font.vboi(37); DOi(16, print("{} ", r[i])); print("\n");
	//ren.render(font.texu, 38);
	//pr1("I render_time[ms]:")(-t.left())("\n");
	ren.flip();}

B App::handle(SDL_Event & o){
	if(o.type==SDL_QUIT){
		stop=1;
		R 1;}
	el(o.type==SDL_KEYDOWN){
		/*if(o.key.keysym.sym==SDLK_LEFT){
			R 1;
		}
		el(o.key.keysym.sym==SDLK_RIGHT){
			R 1;
		}*/
	}
	if(ren.handle(o)) R 1;
	R 0;}



struct Rand{
	I8 seed{1};
	I8 m{2147483647};
	I8 a{48271};
	I8 c{0};
	I4 operator()(){
		seed=(a*seed+c)%m;
		R seed;}
};


I4 main(int argc, char ** argv){
	chdir();
	App app; app.init();
	Timer rt,ut;
	SDL_Event o;
	Rand rand;
	while(!app.stop){
		auto utl=ut.left();

		if(utl<=0){
			//pr1("I el[ms]:")(10-utl)("\n");
			Timer t; t.set(0);
			app.update(0.010f);
			//pr1("I update_time[ms]:")(-t.left())("\n");
			//pr1("I rand:")(rand())("\n");
			ut.set(10);}

		if(rt.left()<=0){


			app.render();


			rt.set(17);}

		{
		//pr1("I minto:")(min(rt.left(),ut.left()))("\n");
		SDL_WaitEventTimeout(nullptr, min(rt.left(),ut.left()));
		Timer t; t.set(0);
		while(SDL_PollEvent(&o)) app.handle(o);
		//pr1("I handle_time[ms]:")(-t.left())("\n");
		}

		}
	R 0;}


