echo Running MakeMakeMake by Eskil Steenberg Hald
gcc makemake.c -o makemake

set -e
cmd=`basename $0`
echo "Running $cmd" 1>&2

if [ $# -gt 3 ]; then
    echo "[$cmd] takes up to three args" 1>&2
    exit 1
fi

./makemake $*  1>&2
make