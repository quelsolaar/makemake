
## MakeMake

MakeMake is a tool for generateing and running make files from C code. It does so by parsing the .c and .h files in the same directory as a starting file (usualy the C file containging main), and determains recursivly what files are required to build the project. make make does this by looking for any function declaration using extern, and the searches for the corresponding function definition.

### building MakeMake:

To build makemake on unix platform you can use the shellscript MakeMakeMake, it will not only build makemake, it will also run it, and it therefor takes the same parameters as MakeMake. To build makemake:

    chmod -x makemakemake.sh
    ./makemakemake.sh

If make a MakeMakeMake is given options it will run makemake and pass on the options to MakeMake. The MakeMakeMake script there for takes the exact same options as MakeMake. Once MakeMakeMake has built MakeMake you can call it directly.

### Running MakeMake

To run MakeMake the comandline format is
    makemake <c file path> <options ...>

The C file path will be used to identify the starting C file and the path where it resides. This path is the path that will be searched for other .c and .h files for possible inclution in the build.

### Options:

MakeMake can be given opetions in two ways, either using the comand line, or as pragmas in the .c and .h files. Each option consists of key-value pair, the name of the option and what the option is set to. The available options are:

-name <name> Name of the executable.
-makefile <name> Name of the make file (or stdout).
-compilerflag <flag> Custom compiler flags.
-platform <name> Platform define.
-dynamiclib <no option> builds the application as a dynamicly linked library.
-lib <name> Library dependency.
-define <name> Adds a define.

To use the options on the comand line use "-" followed by the option name and option. 

    Example: To build project with a specifird name you may call:
    ./makemake ./example.c -name my_example

To use options from within a .c or .h file, add MakeMake pragma in the following format:

    #pragma makemake <option name> <option value> <optional platform>

If for instanc you have a .c file that depends on PTreads on posix platforms, you may add the pragma:

    #pragma makemake lib lpthread posix

The MakeMake can also generate the MakeMakeMake script by calling:

    ./makemake makemakemakemake

### Platforms

you can define as many platfroms as you want, if you do not define platforms on the command line, MakeMake will assume you are building for the platform you are on. For the following platforms, the pre defined platforms are:

    Linux: linux posix
    Windows: win32
    MacOS: macosx apple posix
    Android: android posix

pragma directives with a given platform will only operate of that platform is defined. You can define your own platforms for setting options this way. 

Make make has been tested on Linux, MacOS, and Windows (Make files have been generated on windows, but not tested on windows)

Thanks to the work of Felix Klinge, MakeMake supports Android and the plan is to add iOS support.

### Test project

MakeMake comes with a simple "hello world" test project, called makemake_test... consisiting of 3 files. To test them call:

    chmod -x makemakemake.sh
    ./makemakemake.sh ./makemake_test_one.c

The file makemake_test_one.c, will include makemake_test_two.h that declares a function defined in makemake_test_three.c

### About the project

MakeMake was designed to solve my problem (Im a C developer that develops protable C programs using VisualStudio, and I do not want to maintain up todate make files for all projects and platforms). There are ways to write C code that makemake will not be able to parse, but for me that is a feature not a bug as it requires me to keep my code clean and simple. 

The main motivation for releasig MakeMake is to serve as a n inspiration for developers that they can wit relativly little effort build their own tooling, especially with a simple language like C. Feel free to be inspired by, use or fork the project.

You can find my work on www.quelsolaar.com and at @quelsolaar @eskilsteenberg, and https://www.youtube.com/@eskilsteenberg you can also reach me at eskil at quelsolaar dot com.

