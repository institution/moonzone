#pragma once
#include <GL/glew.h>
#include "ext/ext.hpp"
#define AGL() A(glok());
B glok();
HC* get_gl_err_msg(U4 x);
V myShowGLInfo();
V myAttachShader(U4 prog, U4 shader_type, HC* shader_src);
V myLoadShader(U4 shad, HC* shader_src);
V myShowShader(U4 shad);
V myLinkProgram(U4 prog);
U4 myAttLoc(U4 prog, HC* name);
U4 myUniLoc(U4 prog, HC* name);
