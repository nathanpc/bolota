### Makefile
### Automates the build and everything else of the project.
###
### Author: Nathan Campos <nathan@innoveworkshop.com>

include variables.mk

.PHONY: all compiledb gtk2 run debug memcheck clean
all: $(BUILDDIR)/stamp gtk2

$(BUILDDIR)/stamp:
	$(MKDIR) $(@D)
	$(MKDIR) $(BUILDDIR)/bin
	$(TOUCH) $@

compiledb: clean
	bear --output .vscode/compile_commands.json -- make debug

debug: CFLAGS += -g3 -DDEBUG
debug: all

memcheck: CFLAGS += -g3 -DDEBUG -DMEMCHECK
memcheck: all
	G_SLICE=always-malloc G_DEBUG=gc-friendly valgrind --tool=memcheck \
		--leak-check=full --show-leak-kinds=all --track-origins=yes \
		--suppressions=/usr/share/glib-2.0/valgrind/glib.supp \
		--suppressions=/usr/share/gtk-3.0/valgrind/gtk.supp \
		--log-file=$(BUILDDIR)/valgrind.log \
		./build/bin/gBolota
	cat $(BUILDDIR)/valgrind.log

gtk2: $(BUILDDIR)/stamp
	cd linux/gtk2/ && $(MAKE) $(MAKECMDGOALS)

run:
	cd linux/gtk2/ && $(MAKE) $(MAKECMDGOALS)

clean:
	$(RM) -r $(BUILDDIR)
