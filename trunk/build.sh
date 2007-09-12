#! /bin/sh

# Experiment #2
#
# Instead of putting configure inside the Makefile, which is basically calling
# sh all the time, put 'make' functionality inside configure :) The previous
# experiment needed a POSIX environment anyway and on top of that, it depended
# heavily on GNU make (which is bad) and recursive make (which is worse).
# This experiment just needs a POSIX environment and nothing more.
# Or perhaps not even POSIX ;)

# ----------------------------------( USAGE )----------------------------------

help() {
    cat << __EOF__

usage: ./build.sh [options] [actions]

options:

--cc=CC             specify C compiler
--prefix=PREFIX     installation prefix [default: $prefix]
--help              print this message

actions:

all                 build all
cli                 build cli version only
gui                 build gui version only
lib                 build (static) library only
install             install all programs
install-cli         install cli version only
install-gui         install gui version only
configure           perform configure only
deps                determine dependencies only
clean               clean up
distclean           cleaner up

__EOF__
}

# ----------------------------------( INIT )-----------------------------------

# Be a nice shell

LC_ALL=C ; export LC_ALL
if test -n "$ZSH_VERSION" && (emulate sh) 2>/dev/null 1>&2 ; then
    emulate sh
    NULLCMD=:
else
    case `(set -o) 2>/dev/null` in *posix*) set -o posix ;; esac
fi
BIN_SH=xpg4 ; export BIN_SH
DUALCASE=1 ; export DUALCASE
unset ENV MAIL MAILPATH
PS1='$ ' ; PS2='> ' ; PS4='+ '
nl='
'
IFS=" 	$nl"

# Portable functions

if test "`(echo first ; echo last) | head -1 2>/dev/null`" = "first" ; then
    _head() { head -$1 2>/dev/null ; }
else
    _head() { head -n $1 2>/dev/null ; }
fi
if test "`(echo first ; echo last) | tail -1 2>/dev/null`" = "last" ; then
    _tail() { tail -$1 2>/dev/null ; }
else
    _tail() { tail -n $1 2>/dev/null ; }
fi

not() {
    eval "$@"
    test $? -ne 0
}

case `echo -n` in
    -n)     _echo_n=        _echo_c='\c'    ;; # SysV
    *)      _echo_n='-n '   _echo_c=        ;; # BSD
esac

test -z "$V" && V=0
test -z "$C" && C=1
if test "$C" -ne 1 ; then Bon= ; Boff= ; Red= ; Green=
else Bon="[1m" ; Boff="[0m" ; Red="[31m" ; Green="[32m"
fi

# Defaults

prefix=/usr/local
CC=cc
action=
depsfile=build.deps
configfile=build.config

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

    DEFINES=
    SVN_REV=`svn info * 2>/dev/null | grep ^Revision: \
        | cut -d ' ' -f 2 | xargs -n 1 -iX printf "%05i\n" X \
        | sort -r | _head 1 | sed 's/^0//' | sed 's/^0//' \
        | sed 's/^0//'`
    test $SVN_REV || SVN_REV=`grep revision .svn/entries 2>/dev/null \
        | cut -d '"' -f 2` | xargs -n 1 -iX printf "%05i\n" X \
        | sort -r | _head 1 | sed 's/^0//' | sed 's/^0//' \
        | sed 's/^0//'
    test $SVN_REV || SVN_REV=`sed -n -e '/^dir$/{n;p;q;}' \
        .svn/entries 2>/dev/null`
    test $SVN_REV || SVN_REV=0
    DEFINES="$DEFINES -DSVN_REVISION=$SVN_REV"

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

    OBJSUF=.o
    result "obj suffix" $OBJSUF

    AR=ar
    result "ar" $AR

    AR_FLAGS=cru
    result "ar flags" $AR_FLAGS

    RANLIB=ranlib
    result "ranlib" $RANLIB

    STRIP=strip
    result "strip" $STRIP
}

output_build_config() {
   echo writing $configfile
    cat >> $configfile << __EOF__
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
DEFINES="-DSVN_REVISION=$SVN_REV"
SYSTEM="$SYSTEM"
SYS_LDFLAGS="$SYS_LDFLAGS"
EXESUF="$EXESUF"
AR="$AR"
AR_FLAGS="$AR_FLAGS"
RANLIB="$RANLIB"
STRIP="$STRIP"
OBJSUF="$OBJSUF"
CONFIGURE_DONE=yes
__EOF__
}

# ----------------------------------( UTIL )-----------------------------------

# unwrap lines --> sort --> uniq --> merge equal target lines and change syntax
ccdeps_to_shdeps() {
    tr -d '\015' |
    awk '/[\\]$/ { $NF="" ; printf("%s",$0) ; next } { print }' |
    sort |
    uniq |
    awk '
        BEGIN { save="" }
        {   if (save==$1) {
                ORS="" ;
                for (i=2; i <= NF; i++) {
                    if ((substr($i,1,1) != "/") && (substr($i,2,2) != ":/")) {
                        print " " $i
                    }
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
                    if ((substr($i,1,1) != "/") && (substr($i,2,2) != ":/")) {
                        print " " $i
                    }
                }
            }
        }
        END { if (save != "") { ORS="\"\n" ; print " " } }'
}

# ----------------------------------( MAKE )-----------------------------------

make_init_default() {
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
    exe=$1 ; shift 1
    make_exec "$CC $OBJ_OUT_FLAG $exe `echo $@` $LDFLAGS" "link" "$exe"
    sleep 1
    touch -c $exe
}

make_o_to_a() {
    a=$1 ; shift 1
    make_exec "$AR $AR_FLAGS $a `echo $@`" "archive" "$a"
    make_exec "$RANLIB $a" "ranlib" "$a"
    sleep 1
    touch -c $a
}

addsuffix() {
    suf=$1 ; shift 1
    for i in $@ ; do
        echo $i$suf
    done
}

up_to_date() {
    r=`ls -t1 $@ 2>/dev/null | _head 1`
    test "$r" = "$1"
}

mangle() {
    echo $1 | sed 's/\./_/g; s/\//__/g;' # change . and / into _ and __
}

make_set_default_object_deps() {
    for i in $@ ; do
        j=`mangle $i`
        defdep=`basename $i $OBJSUF`.c
        eval deps_$j=$defdep
    done
}

make_set_deps() {
    x=$1; shift 1
    x=`mangle $x`
    eval deps_$x=\"$@\"
}

make_objects() {
    for i in $@ ; do
        j=`mangle $i`
        eval deps=\"\$deps_$j\"
        if not up_to_date $i $deps ; then
            make_c_to_o $i
        fi
    done
}

make_lib() {
    lib=$1; shift
    libm=`mangle $lib`
    eval libdeps=\"\$deps_$libm\"
    make_objects $libdeps
    if not up_to_date $lib $libdeps ; then
        make_o_to_a $lib $libdeps
    fi
}

make_exe() {
    t=$1; shift
    tm=`mangle $t`
    eval exedeps=\"\$deps_$tm\"
    for i in $exedeps ; do
        case $i in
            *$OBJSUF)   make_objects $i ;;
            *.a)        make_lib $i ;;
            *)          echo unknown dep $i ; exit 2 ;;
        esac
    done
    if not up_to_date $t $exedeps ; then
        make_link $t $exedeps
    fi
}

_make_deps() {
    make_exec "$CC_DEP $DEP_FLAGS $DEF_CFLAGS $GTK_CFLAGS $GTHREAD_CFLAGS `echo $@` 2>/dev/null | ccdeps_to_shdeps >> $depsfile" "ccdep" "$depsfile"
}

make_deps() {
    if not grep -q deps_done=yes $depsfile 2>/dev/null ; then
        > $depsfile
        _make_deps $libhmgen_srcs $cli_srcs $gui_srcs
        echo "deps_done=yes" >> $depsfile
    fi

    echo reading $depsfile
    . $depsfile
}

make_configure() {
    if grep -q CONFIGURE_DONE=yes $configfile 2>/dev/null ; then
        echo reading $configfile
        . $configfile
    else
        > $configfile
        configure
        output_build_config
    fi
}

# ---------------------------------( PROJECT )---------------------------------

make_init_project() {
    make_init_default

    libhmgen_basenames="lib_algo_ff lib_algo_mpd lib_algo_forge lib_postproc \
                        lib_util lib_hmgen lib_export"
    libhmgen_objs=`addsuffix $OBJSUF $libhmgen_basenames`
    libhmgen_srcs=`addsuffix .c $libhmgen_basenames`
    libhmgen=libhmgen.a

    cli_basenames="cli_main"
    cli_objs=`addsuffix $OBJSUF $cli_basenames`
    cli_srcs=`addsuffix .c $cli_basenames`
    cli_g_exe=hmgen_g$EXESUF
    cli_exe=hmgen$EXESUF

    gui_basenames="gui_main gui_callbacks gui_interface gui_support"
    gui_objs=`addsuffix $OBJSUF $gui_basenames`
    gui_srcs=`addsuffix .c $gui_basenames`
    gui_g_exe=hmgengui_g$EXESUF
    gui_exe=hmgengui$EXESUF

    make_set_default_object_deps $libhmgen_objs $cli_objs $gui_objs
    make_set_deps $libhmgen $libhmgen_objs
    make_set_deps $cli_g_exe $cli_objs $libhmgen
    make_set_deps $gui_g_exe $gui_objs $libhmgen
}

make_cli() {
    make_exe $cli_g_exe
    if not up_to_date $cli_exe $cli_g_exe ; then
        make_exec "cp $cli_g_exe $cli_exe" "copy" "$cli_g_exe $cli_exe"
        make_exec "$STRIP $cli_exe" "strip" "$cli_exe"
    fi
}

make_gui() {
    CFLAGS="$DEF_CFLAGS $GTK_CFLAGS $GTHREAD_CFLAGS"
    LDFLAGS="$DEF_LDFLAGS $GTK_LDFLAGS $GTHREAD_LDFLAGS"
    make_exe $gui_g_exe
    if not up_to_date $gui_exe $gui_g_exe ; then
        make_exec "cp $gui_g_exe $gui_exe" "copy" "$gui_g_exe $gui_exe"
        make_exec "$STRIP $gui_exe" "strip" "$gui_exe"
    fi
}

make_all() {
    make_configure
    make_init_project
    make_deps
    make_cli
    make_gui
}

# ----------------------------------( MAIN )-----------------------------------

optarg() {
    echo $1 | cut -d '=' -f 2
}

for i in $@ ; do
    case "$i" in --help|-help|-h|-?) help ; exit ;; esac
done

for i in $@ ; do
    case "$i" in
        --cc=*)         CC=`optarg $i`      ;;
        --prefix=*)     prefix=`optarg $i`  ;;
        configure)
            action=1
            >$configfile
            make_configure
            ;;
        deps)
            action=1
            >$depsfile
            make_configure
            make_init_project
            make_deps
            ;;
        cli)
            action=1
            make_configure
            make_init_project
            make_deps
            make_cli
            ;;
        gui)
            action=1
            make_configure
            make_init_project
            make_deps
            make_gui
            ;;
        lib)
            action=1
            make_configure
            make_init_project
            make_deps
            make_lib $libhmgen
            ;;
        all)
            action=1
            make_all
            ;;
        --*)
            echo unknown option ; exit 1 ;;
        *)
            echo unknown action ; exit 1 ;;
    esac
done

if test -z "$action" ; then make_all ; fi

echo "done"
