
include target.mak

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
					 lib_postproc lib_util lib_hmgen
LIBHMGEN_OBJS = $(addsuffix $(OBJSUF), $(LIBHMGEN_BASENAMES))
LIBHMGEN_SRCS = $(addsuffix .c, $(LIBHMGEN_BASENAMES))

SILENT ?= yes
SILENT_EXEC = @test "$(SILENT)" = "yes" && echo "$(2)" || echo $(1); $(1)

all: $(GUI) $(CLI)

verbose: SILENT=no
verbose: all

$(CLI)-verbose: SILENT=no
$(CLI)-verbose: $(CLI)
$(CLI): $(CLIg)
	$(call SILENT_EXEC,cp $(CLIg) $@,CP $(CLIg) $@)
	$(call SILENT_EXEC,$(STRIP) $@,STRIP $@)

$(CLIg): CFLAGS = $(DEF_CFLAGS)
$(CLIg): LDFLAGS = $(DEF_LDFLAGS)
$(CLIg): $(LIBHMGEN) $(CLI_OBJS)
	$(call SILENT_EXEC,$(CC) -o $@ $(CLI_OBJS) $(LIBHMGEN) $(LDFLAGS),LD $@)

$(GUI)-verbose: SILENT=no
$(GUI)-verbose: $(GUI)
$(GUI): $(GUIg)
	$(call SILENT_EXEC,cp $(GUIg) $@,CP $(GUIg) $@)
	$(call SILENT_EXEC,$(STRIP) $@,STRIP $@)

$(GUIg): CFLAGS = $(GTK_CFLAGS) $(DEF_CFLAGS)
$(GUIg): LDFLAGS = $(GTK_LDFLAGS) $(DEF_LDFLAGS)
$(GUIg): $(LIBHMGEN) $(GUI_OBJS)
	$(call SILENT_EXEC,$(CC) -o $@ $(GUI_OBJS) $(LIBHMGEN) $(LDFLAGS),LD $@)

$(LIBHMGEN)-verbose: SILENT=no
$(LIBHMGEN)-verbose: $(LIBHMGEN)
$(LIBHMGEN): CFLAGS = $(DEF_CFLAGS)
$(LIBHMGEN): LDFLAGS = $(DEF_LDFLAGS)
$(LIBHMGEN): $(LIBHMGEN_OBJS)
	$(call SILENT_EXEC,$(AR) $(AR_FLAGS) $@ $^,AR $@)
	$(call SILENT_EXEC,$(RANLIB) $@,RANLIB $@)

.c.o:
	$(call SILENT_EXEC,$(CC) -c -o $@ $< $(CFLAGS),CC $<)

clean:
	rm -f $(CLI) $(CLIg) $(CLI_OBJS)
	rm -f $(LIBHMGEN) $(LIBHMGEN_OBJS)
	rm -f $(GUI) $(GUIg) $(GUI_OBJS)

mrproper distclean: clean
	rm -f *~ .depend *.pgm *.ppm

depend-verbose dep-verbose: SILENT=no
depend-verbose dep-verbose: dep
depend dep: $(CLI_SRCS) $(LIBHMGEN_SRCS) $(GUI_SRCS)
	$(call SILENT_EXEC,$(CC_DEP) $(DEP_FLAGS) $(DEF_CFLAGS) $(GTK_CFLAGS) $^ 1>.depend,CCDEP >.depend)

size:
	@wc -l *.[ch] Makefile *.mak | tail -1

size-long:
	@wc -l *.[ch] Makefile *.mak

.PHONY:	all dep depend clean distclean mrproper verbose $(CLI)-verbose size
.PHONY: $(GUI)-verbose $(LIBHMGEN)-verbose depend-verbose dep-verbose size-long

-include .depend
