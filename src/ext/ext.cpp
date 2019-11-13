//#include<unistd.h>
//#include<errno.h>
#include"ext.hpp"
namespace ext{
	// set working dir to executable dir
	V chdir(){
		// PATH_MAX~4096
		H buf[PATH_MAX]{0};
		H nwd[PATH_MAX];
		// get exe path
		auto ret=readlink("/proc/self/exe", buf, PATH_MAX); A(ret!=-1);
		// remove filename
		H * p=buf+ret;
		while(buf<=p and *p!='/') --p;
		*p='\0';
		// normalize and set
		A(realpath(buf,nwd)!=0);
		::chdir(nwd);
		pr1("I:workdir:")(nwd)("\n");}
}//ext
