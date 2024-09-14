include common.mk

# Sources and Objects
SRCNAMES  = main.c bolota.c
SOURCES  += $(addprefix $(SRCDIR)/, $(SRCNAMES))
OBJECTS  := $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SOURCES))

.PHONY: all compile compiledb run debug memcheck clean
all: compile

compile: $(BUILDDIR)/stamp $(TARGET)

compiledb: clean
	bear --output .vscode/compile_commands.json -- make compile

run: $(TARGET)
	./$(TARGET)

debug: CFLAGS += -g3 -DDEBUG
debug: clean all

memcheck: CFLAGS += -g3 -DDEBUG -DMEMCHECK
memcheck: compile
	valgrind --tool=memcheck --leak-check=yes --show-leak-kinds=all \
		--track-origins=yes --log-file=valgrind.log ./$(TARGET)
	cat valgrind.log

clean:
	$(RM) valgrind.log
	$(RM) $(OBJECTS)
	$(RM) $(TARGET)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

$(BUILDDIR)/stamp:
	$(MKDIR) $(@D)
	$(TOUCH) $@
