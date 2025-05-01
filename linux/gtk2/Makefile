include ../../variables.mk

# Source file names.
SRCNAMES = main.cpp

# Sources and Objects
PROJECT     = gBolota
TARGET      = $(BUILDDIR)/bin/$(PROJECT)
SOURCES    += $(SRCNAMES)
OBJECTS    := $(patsubst %.cpp, $(BUILDDIR)/$(PROJECT)/%.o, $(SOURCES))
STATICLIBS := $(BUILDDIR)/libbolota/libbolota.a

# Flags
#CFLAGS += $(shell pkg-config --cflags gtk+-2.0 glib-2.0)
#LIBS   += $(shell pkg-config --libs gtk+-2.0 glib-2.0)

.PHONY: all compile test debug memcheck clean
all: compile

compile: $(BUILDDIR)/$(PROJECT)/stamp $(TARGET)

$(TARGET): $(OBJECTS) $(STATICLIBS)
	$(CXX) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

$(BUILDDIR)/$(PROJECT)/%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/libbolota/libbolota.a:
	cd $(SRCDIR) && $(MAKE) $(MAKECMDGOALS)

$(BUILDDIR)/$(PROJECT)/stamp:
	$(MKDIR) $(@D)
	$(TOUCH) $@

run: test

debug: CFLAGS += -g3 -DDEBUG
debug: clean all

memcheck: CFLAGS += -g3 -DDEBUG -DMEMCHECK
memcheck: compile

test:
	$(TARGET)

clean:
	$(RM) -r $(BUILDDIR)/$(PROJECT)
