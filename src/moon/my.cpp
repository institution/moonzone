#include "my.hpp"
B glok(){
	auto x=glGetError();
	if(x!=GL_NO_ERROR){
		do{
			pr2("E:GL:errno:")(x)(":")(get_gl_err_msg(x))('\n');
			x=glGetError();
		}while(x!=GL_NO_ERROR);
		R 0;}
	R 1;}
HC *get_gl_err_msg(U4 x) {
	#define CASE_ERR(X) case X: return #X
	switch(x){
		CASE_ERR(GL_NO_ERROR);
		CASE_ERR(GL_INVALID_ENUM);
		CASE_ERR(GL_INVALID_VALUE);
		CASE_ERR(GL_INVALID_OPERATION);
		CASE_ERR(GL_INVALID_FRAMEBUFFER_OPERATION);
		CASE_ERR(GL_OUT_OF_MEMORY);
		CASE_ERR(GL_STACK_UNDERFLOW);
		CASE_ERR(GL_STACK_OVERFLOW);}
	#undef CASE_ERR
	R "unknown_opengl_error_code";}
V myAttachShader(U4 prog, U4 shader_type, HC* shader_src){
	auto s=glCreateShader(shader_type); AGL();
	glAttachShader(prog, s);
	myLoadShader(s, shader_src);
	// will remain attached to program after delete
	glDeleteShader(s);}
V pr_shader(Rng<H> buf){
	I4 i=0,j=0,l=0;
	while(j<buf.len()){
		while(j<buf.len() and buf[j]!='\n') ++j;
		++j;
		pr1(++l)(buf.sli(i,j)); i=j;}}
V myLoadShader(U4 shad, HC* shader_src){
	glShaderSource(shad, 1, &shader_src, 0), AGL();
	glCompileShader(shad), AGL();
	I4 compiled; glGetShaderiv(shad, GL_COMPILE_STATUS, &compiled), AGL();
	if(!compiled){
		I4 len{0};
		glGetShaderiv(shad, GL_INFO_LOG_LENGTH, &len), AGL();
		MRng<H> log(len);
		glGetShaderInfoLog(shad, len, 0, log.ptr()), AGL();
		pr_shader(str(shader_src));
		pr2("E GL compile shader:")(log.rng())('\n');}
	A(compiled);}
V myShowShader(U4 shad){
	I4 len{0};
	glGetShaderiv(shad, GL_SHADER_SOURCE_LENGTH, &len);
	MRng<H> buf(len);
	glGetShaderSource(shad, len, 0, buf.ptr());
	I4 i=0,j=0,l=0;
	while(j<len and buf[j]!='\n') pr1(++l)(buf.rng(i,j)),i=j;}
V myShowGLInfo(){
	I4 v{0};
	glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &v);
	pr1("GL_MAX_VERTEX_UNIFORM_VECTORS   ")(v)("\n");
	glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &v);
	pr1("GL_MAX_FRAGMENT_UNIFORM_VECTORS ")(v)("\n");
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &v);
	pr1("GL_MAX_VERTEX_ATTRIBS           ")(v)("\n");
	glGetIntegerv(GL_MAX_VERTEX_OUTPUT_COMPONENTS, &v);
	pr1("GL_MAX_VERTEX_OUTPUT_COMPONENTS ")(v)("\n");
	AGL();}
V myLinkProgram(U4 prog){
	glLinkProgram(prog);
	I4 linked; glGetProgramiv(prog, GL_LINK_STATUS, &linked);
	if(!linked){
		I4 len{0};
		glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
		MRng<H> log(len);
		glGetProgramInfoLog(prog, len, 0, log.ptr());
		pr2("E GL link program:")(log.rng())('\n');}
	A(linked);}
U4 myAttLoc(U4 prog, HC* name){
	auto loc=glGetAttribLocation(prog, name);
	if(loc==-1) pr2("W myAttrLoc:attribute not found:")(name)("\n");
	A(loc!=-1);
	R loc;}
U4 myUniLoc(U4 prog, HC* name){
	auto loc=glGetUniformLocation(prog, name);
	if(loc==-1) pr2("W myUniLoc:uniform not found:")(name)("\n");
	A(loc!=-1);
	R loc;}
