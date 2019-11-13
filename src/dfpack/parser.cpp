#include "parser.hpp"
HC* Parser::tok_find(HC* s0){
	H *t=ts.ptr(), *te=ts.ptr()+tk;
	while(1){
		// end of entries?
		if (t==te) R 0;
		// cmp current
		char const* t0=t, *s=s0;
		while (*s && *t && *s==*t) ++s, ++t;
		if (*s=='\0' && *t=='\0') R t0;
		// skip to next
		while (++t, *t!='\0');}
	A(0);}
HC* Parser::tok_push(HC* s){
	HC* t_=&ts[tk];
	while(A(tk<ts.len()), ts[tk]=*s, *s) ++tk, ++s;
	R t_;}
HC* Parser::tok_add(HC* s){
	HC* t=tok_find(s);
	if(!t) t=tok_push(s);
	R t;}
Token Parser::sym_add(HC* s){R Token('y', tok_add(s));}
V Parser::p_skip(){
	while (1){
		if(c==' '||c=='\t'||c=='\n'){get();}
		el(c=='#'){
			while (get(),(' '<=c&&c<='~')||c=='\t');}
		else{break;}}}
V Parser::p_sym(){
	bk=0;
	if('!'<=c&&c<='~'){
		while (put(c),get(),'!'<=c&&c<='~');
		put('\0');
		p_skip();}
	else pr2("E [")(line)(",")(col)("]invalid symbol character:'")(c)("'\n"),fail(0);}
V Parser::p_str(){
	bk=0;
	if(c=='"'){
		get();
		while(1){
			if(c=='\\'){
				switch(get(),c){
					case 't' :put('\t'); break;
					case 'n' :put('\n'); break;
					case 'r' :put('\r'); break;
					case '"' :put('"');  break;
					case '\\':put('\\'); break;
					default:
						pr2("E [")(line)(",")(col)("]invalid escaped character:'")(c)("'\n"),fail();}
				get();}
			el(c=='"')        {put('\0'); get(); break;}
			el(' '<=c&&c<='~'){put(c);    get();}
			else pr2("E [")(line)(",")(col)("]invalid char inside string:")(c)("'\n"),fail();}
		p_skip();}
	else pr2("E [")(line)(",")(col)("]invalid string quote:")(c)("'\n"),fail();}
Token Parser::parse(){
	// return next token
	if(eof){
		R Token('y', tok_add(""));}
	el(c=='"'){
		R p_str(), Token('s', tok_add(bs.ptr()));}
	el('!'<=c&&c<='~'){
		R p_sym(), Token('y', tok_add(bs.ptr()));}
	else pr2("E [")(line)(",")(col)("]unexpected char:'")(c)("'\n"),fail();}


/*TEST(
	char fname[]="/tmp/testparserXXXXXX";
	File rc(mkstemp(fname));
	print(rc, "sheet _ max_sprites 1\n");
	print(rc, "sprite 0\n");
	print(rc, "param2 \"Ala ma kota\"\n");

	Parser<> p(fname);
	A(eq(p.parse().sym(),"sheet"));
	A(eq(p.parse().sym(),"_"));
	A(eq(p.parse().sym(),"param1"));
	A(p.parse().num() == 123);
	A(eq(p.parse().sym(),"param2"));
	A(eq(p.parse().str(),"Ala ma kota"));

)*/
