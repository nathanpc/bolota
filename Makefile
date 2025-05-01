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
debug: clean all

memcheck: CFLAGS += -g3 -DDEBUG -DMEMCHECK
memcheck: clean all
	valgrind --tool=memcheck --leak-check=yes --show-leak-kinds=all \
		--track-origins=yes --log-file=$(BUILDDIR)/valgrind.log ./build/bin/gl s
	cat $(BUILDDIR)/valgrind.log

gtk2: $(BUILDDIR)/stamp
	cd linux/gtk2/ && $(MAKE) $(MAKECMDGOALS)

run:
	cd linux/gtk2/ && $(MAKE) $(MAKECMDGOALS)

clean:
	$(RM) -r $(BUILDDIR)
