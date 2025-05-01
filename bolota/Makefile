include ../variables.mk

# Source file names.
SRCNAMES = Document.cpp UString.cpp Field.cpp FieldTypes.cpp DateField.cpp \
	IconField.cpp Errors/Error.cpp Errors/ConsistencyError.cpp \
	Errors/SystemError.cpp Utilities/FileUtils.cpp

# Sources and Objects
PROJECT  = libbolota
TARGET   = $(BUILDDIR)/$(PROJECT)/$(PROJECT).a
SOURCES += $(addprefix $(SRCDIR)/, $(SRCNAMES))
OBJECTS := $(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/$(PROJECT)/%.o, $(SOURCES))

# Unicode conversion shim.
SOURCES += $(SRCDIR)/../shims/cvtutf/ConvertUTF.cpp \
	$(SRCDIR)/../shims/cvtutf/Unicode.cpp
OBJECTS += $(BUILDDIR)/$(PROJECT)/Unicode/ConvertUTF.o \
	$(BUILDDIR)/$(PROJECT)/Unicode/Unicode.o

.PHONY: all compile debug memcheck clean
all: compile

compile: $(BUILDDIR)/$(PROJECT)/stamp $(TARGET)

$(TARGET): $(OBJECTS)
	$(AR) rcs $@ $^

$(BUILDDIR)/$(PROJECT)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/$(PROJECT)/Unicode/%.o: $(SRCDIR)/../shims/cvtutf/%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/$(PROJECT)/stamp:
	$(MKDIR) $(@D)
	$(MKDIR) $(@D)/Unicode
	$(MKDIR) $(@D)/Errors
	$(MKDIR) $(@D)/Utilities
	$(TOUCH) $@

run: test

debug: CFLAGS += -g3 -DDEBUG
debug: clean all

memcheck: CFLAGS += -g3 -DDEBUG -DMEMCHECK
memcheck: compile

clean:
	$(RM) -r $(BUILDDIR)/$(PROJECT)
