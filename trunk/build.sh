#! /bin/sh

# Experiment #2
#
# Instead of putting configure inside the Makefile, which is basically calling
# sh all the time, put 'make' functionality inside configure :) The previous
# experiment needed a POSIX environment anyway and on top of that, it depended
# heavily on GNU make (which is bad) and recursive make (which is worse).
# This experiment just needs a POSIX environment and nothing more.

# FIXME head/tail -1/-n 1 stuff

# ----------------------------------( INIT )-----------------------------------

export LC_ALL=C
if test "$NOCOLOR" ; then Bon= ; Boff= ; Red= ; Green=
else Bon="[1m" ; Boff="[0m" ; Red="[31m" ; Green="[32m"
fi
test -z "$V" && V=0

# --------------------------------( CONFIGURE )--------------------------------

result() {
    if test "$2" = "no" ; then      col="$Bon$Red"
    elif test "$2" = "yes" ; then   col="$Bon$Green"
    else                            col="$Bon"
    fi
    if test -n "$2" ; then
        printf "%-16s : $col%s$Boff\n" "$1" "$2"
    fi
}

my_pkg_config() {
    RES=`pkg-config --atleast-version=$4 $1 2>/dev/null && echo yes || echo no`
    result $1 $RES
    if test "$RES" != "yes" ; then
        echo "$1, at least version $4, not found."
        echo "Perhaps you need to install the dev(el) packages?"
        exit 1
    fi
    eval $2=\"`pkg-config --cflags $1 2>/dev/null`\"
    eval $3=\"`pkg-config --static --libs $1 2>/dev/null`\"
}

configure() {
    for i in $CC cc gcc suncc icc tcc ; do
        if which $i 2>/dev/null 1>&2 ; then
            break
        fi
    done
    CC=$i
    CC_DEP=$i
    result compiler $CC

    CC_VENDOR=`$CC --help 2>/dev/null | grep -q gcc && echo gnu`
    if test "$CC_VENDOR" = "gnu" ; then
        CC_VERSION=`$CC -dumpversion`
    else
        CC_VENDOR=`$CC -flags 2>/dev/null | grep -q sunperf && echo sun`
        if test "$CC_VENDOR" = "sun" ; then
            CC_VERSION=`echo | $CC -E - | grep ident | cut -d ' ' -f 5`
        else
            CC_VENDOR=unknown
            CC_DEP=true
        fi
    fi
    CC_VERSION_MAJOR=`echo $CC_VERSION | cut -d '.' -f 1`
    CC_VERSION_MINOR=`echo $CC_VERSION | cut -d '.' -f 2`
    CC_VERSION_SUBMINOR=`echo $CC_VERSION | cut -d '.' -f 3`
    result vendor $CC_VENDOR
    result version $CC_VERSION

    if test "$CC_VENDOR" = gnu ; then
        if test "$CC_VERSION_MAJOR" -le 2 ; then
            echo "gcc version < 3.x is not supported"
            exit 1
        fi
    fi

    case $CC_VENDOR in
        gnu)
            WARN_FLAGS="-Wall -W"
            OPT_FLAGS="-O3"
            STD_FLAGS="-std=c99"
            DEBUG_FLAGS="-g"
            DEP_FLAGS="-MM"
            OBJ_OUT_FLAG="-o"
            DONT_LINK_FLAG="-c"
            ;;
        sun)
            WARN_FLAGS=
            OPT_FLAGS="-xO5"
            STD_FLAGS="-Xc"
            DEBUG_FLAGS="-g"
            DEP_FLAGS="-xM"
            OBJ_OUT_FLAG="-o"
            DONT_LINK_FLAG="-c"
            ;;
        unknown)
            WARN_FLAGS=
            OPT_FLAGS="-O"
            STD_FLAGS=
            DEBUG_FLAGS="-g"
            DEP_FLAGS=
            OBJ_OUT_FLAG="-o"
            DONT_LINK_FLAG="-c"
            ;;
    esac
    result warnings $WARN_FLAGS
    result optimize $OPT_FLAGS
    result standards $STD_FLAGS
    result debug $DEBUG_FLAGS
    result dependencies $DEP_FLAGS
    result "object out" $OBJ_OUT_FLAG
    result "do not link" $DONT_LINK_FLAG

    RES=`pkg-config --version 2>/dev/null 1>&2 && echo yes || echo no`
    result pkg-config $RES
    if test "$RES" != "yes" ; then
        echo "pkg-config not found"
        exit 1
    fi

    my_pkg_config gtk+-2.0 GTK_CFLAGS GTK_LDFLAGS 2.0
    my_pkg_config gthread-2.0 GTHREAD_CFLAGS GTHREAD_LDFLAGS 2.0

    if test "$CC_VENDOR" = "sun" ; then
        GTHREAD_CFLAGS=`echo xXx $GTHREAD_CFLAGS | sed 's/-pthread//; s/^xXx//'`
        GTHREAD_LDFLAGS=`echo xXx $GTHREAD_LDFLAGS | sed 's/-pthread//; s/^xXx//'`
    fi

    SVN_REV=`svn info * 2>/dev/null | grep ^Revision: \
        | cut -d ' ' -f 2 | xargs -n 1 -iX printf "%05i\n" X \
        | sort -r | head -n 1 | sed 's/^0//' | sed 's/^0//' \
        | sed 's/^0//'`
    test $SVN_REV || SVN_REV=`grep revision .svn/entries 2>/dev/null \
        | cut -d '"' -f 2` | xargs -n 1 -iX printf "%05i\n" X \
        | sort -r | head -n 1 | sed 's/^0//' | sed 's/^0//' \
        | sed 's/^0//'
    test $SVN_REV || SVN_REV=`sed -n -e '/^dir$/{n;p;q;}' \
        .svn/entries 2>/dev/null`
    test $SVN_REV || SVN_REV=0

    SYSTEM=`uname -s 2>/dev/null`
    SYSTEM=`echo $SYSTEM | tr '[A-Z]' '[a-z]'`
    case "$SYSTEM" in
        linux*)     SYSTEM=linux ;;
        mingw*)     SYSTEM=mingw ;;
        *)          SYSTEM="$$SYSTEM_untested" ;;
    esac
    result system $SYSTEM

    SYS_LDFLAGS=
    if test "$SYSTEM" = "mingw" ; then
        if test "$CC_VENDOR" = "gnu" ; then
            SYS_LDFLAGS=-mwindows
        fi
    fi
    result "system ldflags" $SYS_LDFLAGS

    EXESUF=
    if test "$SYSTEM" = "mingw" ; then
        EXESUF=.exe
    fi
    result "exe suffix" $EXESUF
}

output_build_config() {
    cat >> build.config << __EOF__
CC="$CC"
CC_DEP="$CC_DEP"
CC_VENDOR="$CC_VENDOR"
CC_VERSION="$CC_VERSION"
CC_VERSION_MAJOR="$CC_VERSION_MAJOR"
CC_VERSION_MINOR="$CC_VERSION_MINOR"
CC_VERSION_SUBMINOR="$CC_VERSION_SUBMINOR"
WARN_FLAGS="$WARN_FLAGS"
OPT_FLAGS="$OPT_FLAGS"
STD_FLAGS="$STD_FLAGS"
DEBUG_FLAGS="$DEBUG_FLAGS"
DEP_FLAGS="$DEP_FLAGS"
OBJ_OUT_FLAG="$OBJ_OUT_FLAG"
DONT_LINK_FLAG="$DONT_LINK_FLAG"
GTK_CFLAGS="$GTK_CFLAGS"
GTK_LDFLAGS="$GTK_LDFLAGS"
GTHREAD_CFLAGS="$GTHREAD_CFLAGS"
GTHREAD_LDFLAGS="$GTHREAD_LDFLAGS"
DEFINES=-DSVN_REVISION="$SVN_REV"
SYSTEM="$SYSTEM"
SYS_LDFLAGS="$SYS_LDFLAGS"
EXESUF="$EXESUF"
AR=ar
RANLIB=ranlib
STRIP=strip
OBJSUF=.o
AR_FLAGS=cru
CONFIGURE_DONE=yes
__EOF__
}

# ----------------------------------( UTIL )-----------------------------------

# unwrap lines --> sort --> uniq --> merge equal target lines and change syntax
ccdeps_to_shdeps() {
    awk '/[\\]$/ { $NF="" ; printf("%s",$0) ; next } { print }' |
    sort |
    uniq |
    awk '
        BEGIN { save="" }
        {   if (save==$1) {
                ORS="" ;
                for (i=2; i <= NF; i++) {
                    if (substr($i,1,1) != "/") { print " " $i }
                }
            } else {
                if (save != "") { ORS="\"\n" ; print " " }
                ORS="" ; save=$1
                print "deps_"
                for (i=1; i <= length($1) ; i++) {
                    c=substr($1,i,1) ;
                    if (c == ".") { c = "_" }
                    if (c == "/") { c = "__" }
                    if (c == ":") { c = "=\"" }
                    print c
                }
                for (i=2; i <= NF; i++) {
                    if (substr($i,1,1) != "/") { print " " $i }
                }
            }
        }
        END { if (save != "") { ORS="\"\n" ; print " " } }'
}

# ----------------------------------( MAKE )-----------------------------------

# "docs":
#
# make_exec     "command"       "short text"    "short text argument"
# make_c_to_o   filename.o
# make_link     exefilename     filename.o      libraries ...
# make_o_to_a   filename.a      object ...
# addsuffix     suffix          basenames ...
# up_to_date    target          deps ...
#
# make_init_object_deps objects ...

make_init() {
    DEF_CFLAGS="$STD_FLAGS $WARN_FLAGS $OPT_FLAGS $DEBUG_FLAGS $DEFINES"
    DEF_LDFLAGS="-lm $DEBUG_FLAGS $SYS_LDFLAGS"
    CFLAGS="$DEF_CFLAGS"
    LDFLAGS="$DEF_LDFLAGS"
}

make_exec() {
    if test "$V" = "0" ; then   printf "$Bon%s$Boff %s\n" "$2" "$3"
    else                        echo "$1"
    fi
    eval "$1"
}

make_c_to_o() {
    c=`basename $1 $OBJSUF`.c
    make_exec "$CC $DONT_LINK_FLAG $OBJ_OUT_FLAG $1 $c $CFLAGS" "compile" "$c"
}

make_link() {
    exe=$1$EXESUF ; shift 1
    make_exec "$CC $OBJ_OUT_FLAG $exe $@" "link" "$exe"
}

make_o_to_a() {
    a=$1 ; shift 1
    make_exec "$AR $AR_FLAGS $a $@" "archive" "$a"
}

addsuffix() {
    suf=$1 ; shift 1
    for i in $@ ; do
        echo $i$suf
    done
}

up_to_date() {
    r=`ls -t1 $@ 2>/dev/null | head -1`
    case $r in
        $1) true ;;
        *)  false ;;
    esac
}

mangle() {
    echo $1 | sed 's/\./_/g; s/\//__/g;' # change . and / into _ and __
}

make_init_object_deps() {
    for i in $@ ; do
        j=`mangle $i`
        defdep=`basename $i $OBJSUF`.c
        eval deps_$j=$defdep
    done
}

make_check_objects() {
    for i in $@ ; do
        j=`mangle $i`
        eval deps=\"\$deps_$j\"
        if up_to_date $i $deps ; then
            :
        else
            make_c_to_o $i
        fi
    done
}

make_dep() {
    make_exec "$CC_DEP $DEP_FLAGS $DEF_CFLAGS $GTK_CFLAGS $GTHREAD_CFLAGS `echo $@` | ccdeps_to_shdeps >> build.dep" "ccdep" "build.dep"
}

make_init_project() {
    libhmgen_basenames="lib_algo_ff lib_algo_mpd lib_algo_forge lib_postproc \
                        lib_util lib_hmgen lib_export"
    libhmgen_objs=`addsuffix $OBJSUF $libhmgen_basenames`
    libhmgen_srcs=`addsuffix .c $libhmgen_basenames`

    cli_basenames="cli_main"
    cli_objs=`addsuffix $OBJSUF $cli_basenames`
    cli_srcs=`addsuffix .c $cli_basenames`

    gui_basenames="gui_main gui_callbacks gui_interface gui_support"
    gui_objs=`addsuffix $OBJSUF $gui_basenames`
    gui_srcs=`addsuffix .c $gui_basenames`

    make_init_object_deps $libhmgen_objs $cli_objs $gui_objs
}

# ----------------------------------( MAIN )-----------------------------------

if grep -q CONFIGURE_DONE=yes build.config 2>/dev/null ; then
    result "previous config" "build.config"
    . build.config
else
    > build.config
    configure
    output_build_config
fi

make_init
make_init_project

if ! grep -q deps_done=yes build.dep 2>/dev/null ; then
    > build.dep
    make_dep $libhmgen_srcs $cli_srcs $gui_srcs
    echo "deps_done=yes" >> build.dep
fi

. build.dep

make_check_objects $libhmgen_objs
make_check_objects $cli_objs
