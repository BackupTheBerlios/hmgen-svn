#! /bin/sh

# Experimental configure/make script
# Copyright (C) 2007 Ivo van Poorten
# See LICENSE for licensing details
# Tested with: bash 3.1.17, zsh 4.3.4, pdksh 5.2.14, posh 0.5.7

# ----------------------------------( USAGE )----------------------------------

help() {
    cat << __EOF__

usage: ./build.sh [option(s)] [action(s)] [option(s)] [action(s)] ...

options:

--cc=CC             specify C compiler
--prefix=PREFIX     installation prefix [default: $PREFIX]
--destdir=DESTDIR   installation destination directory [default: $DESTDIR]
--verbose           verbose compilation
--nocolor           disable colors for terminals w/o ANSI capabilities
--configfile=FILE   specify alternative config file
--depsfile=FILE     specify alternative dependencies file
--ar=AR             specify archiver
--ranlib=RANLIB     specify ranlib
--strip=STRIP       specify strip
--awk=AWK           specify awk
--help              print this message

actions:

all                 build all
cli                 build cli version only
gui                 build gui version only
lib                 build (static) library only
install             install all programs
install_cli         install cli version only
install_gui         install gui version only
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

not() { eval "$@" ; test $? -ne 0 ; }

case `echo -n` in
    -n)     _echo_n=        _echo_c='\c'    _echo_e=        ;; # SysV
    *)      _echo_n='-n '   _echo_c=        _echo_e='-e '   ;; # BSD
esac

test -z "$V" && V=0
test -z "$C" && C=1

init_colors() {
    if test "$C" -ne 1 ; then Bon= ; Boff= ; Red= ; Green=
    else Bon="[1m" ; Boff="[0m" ; Red="[31m" ; Green="[32m"
    fi
}
init_colors

# Defaults

PREFIX=/usr/local
DESTDIR=
CC=cc
action=
depsfile=./build.deps
configfile=./build.config

# --------------------------------( CONFIGURE )--------------------------------

die() { echo "$1" >&2 ; exit 1 ; }

question() {
    case "$1" in
        ?) echo $_echo_n"$1                : $_echo_c" ;;
        ??) echo $_echo_n"$1               : $_echo_c" ;;
        ???) echo $_echo_n"$1              : $_echo_c" ;;
        ????) echo $_echo_n"$1             : $_echo_c" ;;
        ?????) echo $_echo_n"$1            : $_echo_c" ;;
        ??????) echo $_echo_n"$1           : $_echo_c" ;;
        ???????) echo $_echo_n"$1          : $_echo_c" ;;
        ????????) echo $_echo_n"$1         : $_echo_c" ;;
        ?????????) echo $_echo_n"$1        : $_echo_c" ;;
        ??????????) echo $_echo_n"$1       : $_echo_c" ;;
        ???????????) echo $_echo_n"$1      : $_echo_c" ;;
        ????????????) echo $_echo_n"$1     : $_echo_c" ;;
        ?????????????) echo $_echo_n"$1    : $_echo_c" ;;
        ??????????????) echo $_echo_n"$1   : $_echo_c" ;;
        ???????????????) echo $_echo_n"$1  : $_echo_c" ;;
        ???????????????*) echo $_echo_n"$1 : $_echo_c" ;;
    esac
}

answer() {
    if   test "$1" = "no" ;  then   col="$Bon$Red"
    elif test "$1" = "yes" ; then   col="$Bon$Green"
    else                            col="$Bon"
    fi
    test -n "$1" && echo "$col$1$Boff" || echo "<none>"
}

result() { test -n "$2" && question "$1" && answer "$2" ; }

results() { while test -n "$1" ; do result "$1" "$2" ; shift 2 ; done ; }

my_pkg_config() {
    question $1
    RES=`pkg-config --atleast-version=$4 $1 2>/dev/null && echo yes || echo no`
    answer $RES
    test "$RES" != yes && die "at least version $4 not found"
    eval $2=\"`pkg-config --cflags $1 2>/dev/null`\"
    eval $3=\"`pkg-config --static --libs $1 2>/dev/null`\"
}

find_program() {
    prog=$1; shift
    var=$1; shift
    mand=$1; shift
    question $prog
    for i in $@ NONE ; do
        if which $i 2>/dev/null 1>&2 ; then break ; fi
    done
    eval $var=$i
    answer $i
    test "$mand" = "mandatory" && test "$i" = "NONE" && die "$prog is $mand"
}

pad_with_zeroes() {
    while true; do
        read A
        case "$A" in
            ?)      echo 0000$A ;;
            ??)     echo 000$A  ;;
            ???)    echo 00$A   ;;
            ????)   echo 0$A    ;;
            ?????)  echo $A     ;;
            *)      break       ;;
        esac
    done
}

get_svn_revision() {
    SVN_REV=`svn info * 2>/dev/null | grep '^Revision:' \
        | cut -d ' ' -f 2 | pad_with_zeroes \
        | sort -r | _head 1 | sed 's/^0//; s/^0//; s/^0//;'`
    test $SVN_REV || SVN_REV=`grep revision .svn/entries 2>/dev/null \
        | cut -d '"' -f 2` | pad_with_zeroes \
        | sort -r | _head 1 | sed 's/^0//; s/^0//; s/^0//;'
    test $SVN_REV || SVN_REV=`sed -n -e '/^dir$/{n;p;q;}' \
        .svn/entries 2>/dev/null`
    test $SVN_REV && echo $SVN_REV || echo 0
}

cc_conf() {
    WARN_FLAGS="$1"
    OPT_FLAGS="$2"
    STD_FLAGS="$3"
    DEBUG_FLAGS="$4"
    DEP_FLAGS="$5"
    OBJ_OUT_FLAG="$6"
    DONT_LINK_FLAG="$7"
}

configure() {
    question tools
    for i in which sed grep tr sort uniq cat test cut cp rm chmod ; do
        (which $i) 2>/dev/null 1>&2 || die "$i is mandatory"
        echo $_echo_n"$i $_echo_c"
    done
    echo

    find_program compiler CC mandatory $CC cc gcc suncc icc tcc
    CC_DEP=$CC

    question vendor
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
    answer $CC_VENDOR
    result version $CC_VERSION

    if test "$CC_VENDOR" = gnu ; then
        if test "$CC_VERSION_MAJOR" -le 2 ; then
            echo "gcc version < 3.x is not supported"
            exit 1
        fi
    fi

    case $CC_VENDOR in
        gnu)     cc_conf "-Wall -W" "-O3" "-std=c99" "-g" "-MM" "-o" "-c" ;;
        sun)     cc_conf "" "-xO5" "-Xc" "-g" "-xM" "-o" "-c" ;;
        unknown) cc_conf "" "-O" "" "-g" "" "-o" "-c" ;;
    esac
    results warnings "$WARN_FLAGS" optimize "$OPT_FLAGS" standards "$STD_FLAGS"
    results debug "$DEBUG_FLAGS" dependencies "$DEP_FLAGS"
    results "object out" "$OBJ_OUT_FLAG" "do not link" "$DONT_LINK_FLAG"

    find_program pkg-config PKGCONFIG mandatory $PKGCONFIG pkg-config

    my_pkg_config gtk+-2.0 GTK_CFLAGS GTK_LDFLAGS 2.0
    my_pkg_config gthread-2.0 GTHREAD_CFLAGS GTHREAD_LDFLAGS 2.0

    if test "$CC_VENDOR" = "sun" ; then
        GTHREAD_CFLAGS=`echo xXx $GTHREAD_CFLAGS | sed 's/-pthread//; s/^xXx//'`
        GTHREAD_LDFLAGS=`echo xXx $GTHREAD_LDFLAGS | sed 's/-pthread//; s/^xXx//'`
    fi

    question "hmgen version"
    DEFINES=
    X="#define HMG_VERSION_MAJOR"
    VERMAJ=`grep "$X" version.h | sed "s/$X//"`
    X="#define HMG_VERSION_MINOR"
    VERMIN=`grep "$X" version.h | sed "s/$X//"`
    SVN_REV=`get_svn_revision`
    DEFINES="$DEFINES -DSVN_REVISION=$SVN_REV"
    answer `echo $VERMAJ`.`echo $VERMIN`.$SVN_REV

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

    find_program ar AR mandatory $AR ar gar

    AR_FLAGS=cru
    result "ar flags" $AR_FLAGS

    find_program ranlib RANLIB  optional $RANLIB    ranlib true
    find_program strip  STRIP   optional $STRIP     strip true
    find_program awk    AWK     optional $AWK       mawk gawk nawk awk true

    results "install prefix" "$PREFIX" "install destdir" "$DESTDIR"
}

output_build_config() {
    echo writing $configfile
    CONFIGURE_DONE=yes
    for i in CC CC_DEP CC_VENDOR CC_VERSION CC_VERSION_MAJOR CC_VERSION_MINOR \
        CC_VERSION_SUBMINOR WARN_FLAGS OPT_FLAGS STD_FLAGS DEBUG_FLAGS \
        DEP_FLAGS OBJ_OUT_FLAG DONT_LINK_FLAG GTK_CFLAGS GTK_LDFLAGS \
        GTHREAD_CFLAGS GTHREAD_LDFLAGS DEFINES SYSTEM SYS_LDFLAGS EXESUF AR \
        AR_FLAGS RANLIB STRIP OBJSUF AWK PREFIX DESTDIR CONFIGURE_DONE ; do
        eval echo "$i=\\\"\$$i\\\"" >> $configfile
    done
}

# ----------------------------------( UTIL )-----------------------------------

# unwrap lines --> sort --> uniq --> merge equal target lines and change syntax
ccdeps_to_shdeps() {
    tr -d '\015' |
    $AWK '/[\\]$/ { $NF="" ; ORS="" ; print ; next} { ORS="\n" ; print }' |
    sort |
    uniq |
    $AWK '
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
    if test "$V" = "0" ; then   echo "$Bon$2$Boff $3"
    else                        echo "$1"
    fi
    eval "$1" || die "build failed"
}

make_exec2() {
    make_exec "$1" "$2" "$1"
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
    for i in $@ ; do echo $i$suf ; done
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
    if grep -q CONFIGURE_DONE=\"yes\" $configfile 2>/dev/null ; then
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

make_conf_init_deps() {
    make_configure
    make_init_project
    make_deps
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
    make_cli
    make_gui
}

make_clean() {
    rm -f $libhmgen_objs $cli_objs $gui_objs $libhmgen
    rm -f $cli_g_exe $cli_exe $gui_g_exe $gui_exe
    rm -f *~
}

make_distclean() {
    make_clean
    rm -f $configfile $depsfile
}

make_install_cli() {
    make_cli
    make_exec2 "mkdir -p \"$DESTDIR/$PREFIX/bin\"" "install:"
    make_exec2 "cp $cli_exe \"$DESTDIR/$PREFIX/bin\"" "install:"
    make_exec2 "chmod 755 \"$DESTDIR/$PREFIX/bin/$cli_exe\"" "install:"
}

make_install_gui() {
    make_gui
    make_exec2 "mkdir -p \"$DESTDIR/$PREFIX/bin\"" "install:"
    make_exec2 "cp $gui_exe \"$DESTDIR/$PREFIX/bin\"" "install:"
    make_exec2 "chmod 755 \"$DESTDIR/$PREFIX/bin/$gui_exe\"" "install:"
    make_exec2 "mkdir -p \"$DESTDIR/$PREFIX/share/hmgen\"" "install:"
    make_exec2 "cp -a share/hmgen/* \"$DESTDIR/$PREFIX/share/hmgen\"" "install:"
}

make_install() {
    make_install_cli
    make_install_gui
}

# ----------------------------------( MAIN )-----------------------------------

optarg() {
    echo $1 | cut -d '=' -f 2
}

for i in $@ ; do
    case "$i" in --help|-help|-h|-?) help ; exit ;; esac
done

for arg in $@ ; do
    case "$arg" in
        --cc=*)         CC=`optarg $arg`      ;;
        --ar=*)         AR=`optarg $arg`      ;;
        --ranlib=*)     RANLIB=`optarg $arg`  ;;
        --strip=*)      STRIP=`optarg $arg`   ;;
        --awk=*)        AWK=`optarg $arg`     ;;
        --prefix=*)     PREFIX=`optarg $arg`  ;;
        --destdir=*)    DESTDIR=`optarg $arg` ;;
        --verbose)      V=1                 ;;
        --nocolor)      C=0 ; init_colors   ;;
        --configfile=*) configfile=`optarg $arg`  ;;
        --depsfile=*)   depsfile=`optarg $arg`    ;;
        configure)
            action=1
            >$configfile
            make_configure
            ;;
        deps)
            action=1
            >$depsfile
            make_conf_init_deps
            ;;
        lib)
            action=1
            make_conf_init_deps
            make_lib $libhmgen
            ;;
        cli|gui|all|clean|distclean|install_cli|install_gui|install)
            action=1
            make_conf_init_deps
            make_$arg
            ;;
        -*)
            echo unknown option ; exit 1 ;;
        *)
            echo unknown action ; exit 1 ;;
    esac
done

if test -z "$action" ; then make_conf_init_deps ; make_all ; fi

echo "done"
