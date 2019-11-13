#pragma once
namespace shader{
inline char const* vert0 = R"(
	#version 300 es
	precision mediump float;

	uniform mat4 u_proj;
	uniform lowp int u_mode;

	uniform vec2 p;
	uniform vec2 r;
	uniform vec2 s;

	in  vec2 a_xy;
	in  vec2 a_uv;

	out vec2 b_xy;
	out vec2 b_uv;

	void main(){
		gl_Position=u_proj * vec4(
			r[0]*s[0]*a_xy[0] - r[1]*s[1]*a_xy[1] + p[0],
			r[1]*s[0]*a_xy[0] + r[0]*s[1]*a_xy[1] + p[1],
			0.0,
			1.0
		);
		//gl_Position=u_proj * vec4(a_xy.x, a_xy.y, 0.0, 1.0);
		b_uv=a_uv;
		b_xy=a_xy;
	}
)";

inline char const* frag0 = R"(
	#version 300 es
	precision mediump float;

	uniform lowp sampler2D u_base;
	uniform lowp int u_mode;
	uniform vec4 u_rgba;

	in vec2 b_uv;
	in vec2 b_xy;
	in vec2 b_pq;
	out vec4 outcolor;

	void main(){
		//float cba= mod(b_xy[1], 2.0)<1.0 ? 0.9 : 1.0;
		float cba=1.0f;
		float v=0.0f;

		switch(u_mode){
			case 0:
				v=1.0f-abs(b_xy.y);
				break;
			case 1:
				v=float(texture(u_base, b_uv).r);
				break;
			default:
				v=0.0f;
				break;
		}

		float aa=fwidth(v)*0.5;
		//float aa=0.0f;
		float w=smoothstep(0.5-aa,0.5+aa,v);
		outcolor=vec4(u_rgba.xyz,u_rgba.a*w*cba);


	}
)";
}
