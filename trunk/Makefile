
# Makefile with built-in configure experiment
# Needs GNU Make to run properly

ifeq ($(V),1)
SILENT=no
endif

# ---------- COLORS ----------

ifneq ($(colors),no)
Bon = "[1m"
Boff = "[0m"
Red = "[31m"
Green = "[32m"
else
Bon =
Boff =
Red =
Green =
endif

# ---------- CONFIGURE PREAMBLE ----------

-include config.mak

MAKEFLAGS += --no-print-directory

ifneq ($(CONFIGURE_DONE),yes)
DEFGOAL=$(filter-out all clean distclean configure, $(MAKECMDGOALS))
$(DEFGOAL) all:
	@make configure
	@make $(MAKECMDGOALS)

clean:

else

# ---------- PROJECT ----------

.SUFFIXES:
.SUFFIXES: $(OBJSUF) .c

DEF_CFLAGS = $(STD_FLAGS) $(WARN_FLAGS) $(OPT_FLAGS) $(DEBUG_FLAGS) $(DEFINES)
DEF_LDFLAGS = -lm $(DEBUG_FLAGS)

CLI = hmgen$(EXESUF)
CLIg = hmgen_g$(EXESUF)
CLI_BASENAMES = cli_main
CLI_OBJS = $(addsuffix $(OBJSUF), $(CLI_BASENAMES))
CLI_SRCS = $(addsuffix .c, $(CLI_BASENAMES))

GUI = hmgengui$(EXESUF)
GUIg = hmgengui_g$(EXESUF)
GUI_BASENAMES = gui_main gui_callbacks gui_interface gui_support
GUI_OBJS = $(addsuffix $(OBJSUF), $(GUI_BASENAMES))
GUI_SRCS = $(addsuffix .c, $(GUI_BASENAMES))

LIBHMGEN = libhmgen.a
LIBHMGEN_BASENAMES = lib_algo_ff lib_algo_mpd lib_algo_forge \
                     lib_postproc lib_util lib_hmgen lib_export
LIBHMGEN_OBJS = $(addsuffix $(OBJSUF), $(LIBHMGEN_BASENAMES))
LIBHMGEN_SRCS = $(addsuffix .c, $(LIBHMGEN_BASENAMES))

build_pretty = printf "$(Bon)%s$(Boff) %s\n" "$(1)" "$(2)"
SILENT ?= yes
SILENT_EXEC = @test "$(SILENT)" = "yes" && $(call build_pretty,$(2),$(3)) \
                                        || echo "$(1)"; $(1)

all: $(GUI) $(CLI)

verbose: SILENT=no
verbose: all

$(CLI)-verbose: SILENT=no
$(CLI)-verbose: $(CLI)
$(CLI): $(CLIg)
	$(call SILENT_EXEC,cp $(CLIg) $@,copy,$(CLIg) $@)
	$(call SILENT_EXEC,$(STRIP) $@,strip,$@)

$(CLIg): CFLAGS = $(DEF_CFLAGS)
$(CLIg): LDFLAGS = $(DEF_LDFLAGS)
$(CLIg): $(LIBHMGEN) $(CLI_OBJS)
	$(call SILENT_EXEC,$(CC) $(OBJ_OUT_FLAG) $@ $(CLI_OBJS) $(LIBHMGEN) \
		$(LDFLAGS),link,$@)

$(GUI)-verbose: SILENT=no
$(GUI)-verbose: $(GUI)
$(GUI): $(GUIg)
	$(call SILENT_EXEC,cp $(GUIg) $@,copy,$(GUIg) $@)
	$(call SILENT_EXEC,$(STRIP) $@,strip,$@)

$(GUIg): CFLAGS = $(GTK_CFLAGS) $(GTHREAD_CFLAGS) $(DEF_CFLAGS)
$(GUIg): LDFLAGS = $(GTK_LDFLAGS) $(GTHREAD_LDFLAGS) $(DEF_LDFLAGS)
$(GUIg): $(LIBHMGEN) $(GUI_OBJS)
	$(call SILENT_EXEC,$(CC) $(OBJ_OUT_FLAG) $@ $(GUI_OBJS) $(LIBHMGEN) \
		$(LDFLAGS),link,$@)

$(LIBHMGEN)-verbose: SILENT=no
$(LIBHMGEN)-verbose: $(LIBHMGEN)
$(LIBHMGEN): CFLAGS = $(DEF_CFLAGS)
$(LIBHMGEN): LDFLAGS = $(DEF_LDFLAGS)
$(LIBHMGEN): $(LIBHMGEN_OBJS)
	$(call SILENT_EXEC,$(AR) $(AR_FLAGS) $@ $^,archive,$@)
	$(call SILENT_EXEC,$(RANLIB) $@,ranlib,$@)

.c.o:
	$(call SILENT_EXEC,$(CC) $(DONT_LINK_FLAG) $(OBJ_OUT_FLAG) $@ $< $(CFLAGS)\
		,compile,$<)

depend-verbose dep-verbose: SILENT=no
depend-verbose dep-verbose: dep
depend dep: $(CLI_SRCS) $(LIBHMGEN_SRCS) $(GUI_SRCS)
	$(call SILENT_EXEC,$(CC_DEP) $(DEP_FLAGS) $(DEF_CFLAGS) $(GTK_CFLAGS) $(GTHREAD_CFLAGS) $^ 1>.depend,ccdep,>.depend)

size:
	@wc -l *.[ch] Makefile *.mak | tail -1

size-long:
	@wc -l *.[ch] Makefile *.mak

clean:
	rm -f $(CLI) $(CLIg) $(CLI_OBJS)
	rm -f $(LIBHMGEN) $(LIBHMGEN_OBJS)
	rm -f $(GUI) $(GUIg) $(GUI_OBJS)

.PHONY: all dep depend clean verbose $(CLI)-verbose size \
        $(GUI)-verbose $(LIBHMGEN)-verbose depend-verbose dep-verbose size-long

endif	# END CONFIGURE PREAMBLE

# ---------- CONFIGURE ----------

define config-print-result
	if test "$(2)" = "no" ; then \
		col="$(Bon)$(Red)" ; \
	elif test "$(2)" = "yes" ; then \
		col="$(Bon)$(Green)" ; \
	else \
		col="$(Bon)" ; \
	fi ; \
	printf "%-16s : $$col%s$(Boff)\n" "$(1)" "$(2)"
endef

clean-config.mak:
	@> config.mak

config-cc:
	@$(call config-print-result,compiler,$(CC))
	@echo CC=$(CC) >> config.mak
	@echo CC_DEP=$(CC) >> config.mak

config-cc-vendor-and-version:	config-cc
	@CC_VENDOR=`$(CC) --help 2>/dev/null | grep -q gcc && echo gnu` ; \
	if test "$$CC_VENDOR" = "gnu" ; then \
		CC_VERSION=`$(CC) -dumpversion` ; \
	else \
		CC_VENDOR=`$(CC) -flags 2>/dev/null | grep -q sunperf && echo sun` ; \
		if test "$$CC_VENDOR" = "sun" ; then \
			CC_VERSION=`echo | $(CC) -E - | grep ident | cut -d ' ' -f 5` ; \
		else \
			CC_VENDOR=unknown; \
		fi ; \
	fi ; \
	echo CC_VENDOR=$$CC_VENDOR >> config.mak ; \
	echo CC_VERSION=$$CC_VERSION >> config.mak ; \
	echo CC_VERSION_MAJOR=`echo $$CC_VERSION | cut -d '.' -f 1` \
		>>config.mak ;\
	echo CC_VERSION_MINOR=`echo $$CC_VERSION | cut -d '.' -f 2` \
		>>config.mak ;\
	echo CC_VERSION_SUBMINOR=`echo $$CC_VERSION | cut -d '.' -f 3` \
		>>config.mak ;\
	$(call config-print-result,vendor,$$CC_VENDOR) ; \
	$(call config-print-result,version,$$CC_VERSION)

config-cc-vendor-and-version-restrict:	config-cc-vendor-and-version
	@. config.mak ; \
	if test "$$CC_VENDOR" = gnu ; then \
		if test "$$CC_VERSION_MAJOR" -le 2 ; then \
			echo "gcc version < 3.x is not supported" ; \
			exit 1 ; \
		fi ; \
	fi ;

config-cc-flags: 	config-cc-vendor-and-version-restrict
	@. config.mak ; \
	case $$CC_VENDOR in \
		gnu) \
			WARN_FLAGS="-Wall -W" ; \
			OPT_FLAGS="-O3" ; \
			STD_FLAGS="-std=c99" ; \
			DEBUG_FLAGS="-g" ; \
			DEP_FLAGS="-MM" ; \
			OBJ_OUT_FLAG="-o" ; \
			DONT_LINK_FLAG="-c" ; \
			;; \
		sun) \
			WARN_FLAGS= ; \
			OPT_FLAGS="-xO5" ; \
			STD_FLAGS="-Xc" ; \
			DEBUG_FLAGS="-g" ; \
			DEP_FLAGS="-xM" ; \
			OBJ_OUT_FLAG="-o" ; \
			DONT_LINK_FLAG="-c" ; \
			;; \
	esac ; \
	echo WARN_FLAGS=$$WARN_FLAGS >> config.mak ; \
	echo OPT_FLAGS=$$OPT_FLAGS >> config.mak ; \
	echo STD_FLAGS=$$STD_FLAGS >> config.mak ; \
	echo DEBUG_FLAGS=$$DEBUG_FLAGS >> config.mak ; \
	echo DEP_FLAGS=$$DEP_FLAGS >> config.mak ; \
	echo OBJ_OUT_FLAG=$$OBJ_OUT_FLAG >> config.mak ; \
	echo DONT_LINK_FLAG=$$DONT_LINK_FLAG >> config.mak ; \
	$(call config-print-result,warnings,$$WARN_FLAGS) ; \
	$(call config-print-result,optimize,$$OPT_FLAGS) ; \
	$(call config-print-result,standards,$$STD_FLAGS) ; \
	$(call config-print-result,debug,$$DEBUG_FLAGS) ; \
	$(call config-print-result,dependencies,$$DEP_FLAGS) ; \
	$(call config-print-result,object out,$$OBJ_OUT_FLAG) ; \
	$(call config-print-result,do not link,$$DONT_LINK_FLAG) ;

config-pkg-config:
	@TMP=`pkg-config --version 2>/dev/null 1>&2 && echo yes || echo no` ; \
	$(call config-print-result,pkg-config,$$TMP) ; \
	if test "$$TMP" != "yes" ; then \
		echo "pkg-config not found" ; \
		exit 1 ; \
	fi ; \

NOERR=2>/dev/null

define CONFIG_PKG_CONFIG_PKG
	@TMP=`pkg-config --atleast-version=$(4) $(1) $(NOERR) && \
		echo yes || echo no` ;\
	$(call config-print-result,$(1),$$TMP) ; \
	if test "$$TMP" != "yes" ; then \
		echo "$(1), at least version $(4), not found." ; \
		echo "Perhaps you need to install the dev(el) packages?" ; \
		exit 1 ; \
	fi ; \
	TVARC=`pkg-config --cflags $(1) $(NOERR)` ; \
	TVARL=`pkg-config --static --libs $(1) $(NOERR)` ; \
	echo $(2)=$$TVARC >> config.mak ; \
	echo $(3)=$$TVARL >> config.mak ;
endef

config-gtk-flags: config-pkg-config
	$(call CONFIG_PKG_CONFIG_PKG,gtk+-2.0,GTK_CFLAGS,GTK_LDFLAGS,2.0)

config-gthread-flags: config-pkg-config
	$(call CONFIG_PKG_CONFIG_PKG,gthread-2.0,GTHREAD_CFLAGS,GTHREAD_LDFLAGS,2.0)

config-suncc-flags-fix: config-cc-vendor-and-version config-gthread-flags
	@if grep -q CC_VENDOR=sun config.mak ; then \
		GTHREAD_CFLAGS=`grep GTHREAD_CFLAGS= config.mak | sed 's/-pthread//'` ; \
		GTHREAD_LDFLAGS=`grep GTHREAD_LDFLAGS= config.mak | sed 's/-pthread//'` ; \
		echo $$GTHREAD_CFLAGS >> config.mak ; \
		echo $$GTHREAD_LDFLAGS >> config.mak ; \
	fi ;

config-define-none:
	@echo DEFINES= >> config.mak

config-define-svnrev: config-define-none
	@svn_rev=`LC_ALL=C svn info * 2>/dev/null | grep ^Revision: \
		| cut -d ' ' -f 2 | xargs -n 1 -I '{}' printf "%05i\n" '{}' \
		| sort -r | head -n 1 | sed 's/^0//' | sed 's/^0//' \
		| sed 's/^0//'` ; \
	test $$svn_rev || svn_rev=`grep revision .svn/entries 2>/dev/null \
		| cut -d '"' -f 2` | xargs -n 1 -I '{}' printf "%05i\n" '{}' \
		| sort -r | head -n 1 | sed 's/^0//' | sed 's/^0//' \
		| sed 's/^0//' ; \
	test $$svn_rev || svn_rev=`sed -n -e '/^dir$/{n;p;q;}' \
		.svn/entries 2>/dev/null` ; \
	test $$svn_rev || svn_rev=0 ; \
	echo DEFINES=-DSVN_REVISION=$$svn_rev >> config.mak

configure: clean-config.mak \
           config-cc-flags \
           config-gtk-flags \
           config-gthread-flags \
           config-suncc-flags-fix \
           config-define-svnrev
	@echo AR?=ar >> config.mak
	@echo RANLIB?=ranlib >> config.mak
	@echo STRIP?=strip >> config.mak
	@echo EXESUF= >> config.mak
	@echo OBJSUF=.o >> config.mak
	@echo AR_FLAGS=cru >> config.mak
	@echo CONFIGURE_DONE=yes >> config.mak

mrproper distclean: clean
	rm -f *~ .depend *.pgm *.ppm config.mak

.PHONY: mrproper distclean clean-config.mak configure config-cc \
        config-cc-vendor-and-version \
        config-cc-flags config-cc-vendor-and-version-restrict \
        config-pkg-config config-gtk-flags config-gthread-flags

# ---------- DEPENDENCIES ----------

-include .depend
