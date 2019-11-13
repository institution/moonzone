#pragma once
#include "ext/ext.hpp"
/*
tokenizer
token is string
parse token and put it on stack return char*
user can discard last token (just pop) so it wont use memory
if token already on stack than dont duplicate it, return existing
nice!!!
*/
struct Token{
	H type{'\0'};
	HC* text{nullptr};
	Token(H type, HC* text): type(type), text(text) {}
	HC* sym() {R A(type=='y'), text;}
	I4  num() {R A(type=='y'), atoi(text);}
	F4  flo() {R A(type=='y'), atof(text);}
	HC* str() {R A(type=='s'), text;}
	B operator==(HC* s) {R type=='y' and eq(text,s);} //str cmp
	B operator!=(HC* s) {R !(*this==s);}
	B operator==(Token r){R type==r.type and text==r.text;}   //ptr cmp
	B operator!=(Token r){R !(*this==r);}
};
struct Parser{
	I4 tk{0}, bk{0};
	MRng<H> ts; // token table, all unique tokens must fit into this: tok\0tok\0... no symbol can occur twice
	MRng<H> bs; // token buffer, one token must fit into this
	MFd fp;        // input source file
	H c{'\0'};     // current char
	I4 line{0},col{0}; // location in source file
	B eof{0};

	Parser(){alc();}
	Parser(Rng<H> path){alc(); open(path);};
	~Parser(){ close(); }

	// TL -- total unique token length
	// BL -- max single token length
	V alc(I4 tl=262144, I4 bl=256){ts.alc(tl), bs.alc(bl);}
	V get(){
		if(c=='\n') col=0,++line;
		if (!ext::get(fp,&c)) eof=1,c='\0';
		++col;}
	V put(H c){ A(bk<bs.len()), bs[bk++]=c; }
	V open(Rng<H> path){fp.open(path,"r"), eof=0, line=1; get(); p_skip();}
	V close(){fp.close();}
	V clear(){tk=bk=0; DO(i,ts.len()) ts[i]='\0'; DO(i,bs.len()) bs[i]='\0';}

	HC* tok_find(HC* s0);
	HC* tok_push(HC* s);
	HC* tok_add(HC* s);
	Token sym_add(HC* s);
	V p_skip();
	V p_sym();
	V p_str();
	Token parse();
};

