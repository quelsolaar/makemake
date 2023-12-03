
/* this is a demo program for showing how makemake works */

/*
MakeMake is aprogram that parses .c and .h files and autimaticly creates a makefile.

Because make make itself is a program, that needs to be built, a shellscript is provided, 
called "makemakemake", it creats the source code for makemake, compiles it, and then runs
makemake. If you are either using makemake, or makemakemake, they use the exact same 
arguments.

To build this test program using using the shell script call:
chmod -x makemakemake.sh
./makemakemake.sh ./makemake_test_one.c


using the command:
./makemake ./makemake_test_one.c

To see further options, call either makemake or makemakemake witout any options.

This demo consists of 3 files, a main file (makemake_test_one.c), a header file 
(makemake_test_two.h) that declares a function, and another .c file (makemake_test_three.c)
that defines the function. Makemake, will parse the first file, then include the header, 
parse that, assume the declaration of "my_test_function" in the header file is used, and 
therfor add the finale .c file after finding the function defined in it.

*/


#include "makemake_test_two.h" 


int main(int argc, char **argv)
{
	my_test_function();
	return 1;
}