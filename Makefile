# Makefile for zapoctak project
# Build: make
# Build examples: make examples
# Clean: make clean

CC := gcc
CFLAGS := -Wall -Wextra -g -std=c23 -fPIC
LDFLAGS := -lm

# Directories
SRCDIR := src
INCDIR := include
EXDIR := examples
BUILDDIR := build
BINDIR := $(BUILDDIR)/bin
OBJDIR := $(BUILDDIR)/obj

# Library files
LIB_SOURCES := $(wildcard $(SRCDIR)/*.c)
LIB_HEADERS := $(wildcard $(INCDIR)/*.h)
LIB_OBJECTS := $(addprefix $(OBJDIR)/,$(notdir $(LIB_SOURCES:.c=.o)))
LIBRARY := $(BUILDDIR)/libzapoctak.a

# Example files
EXAMPLE_SOURCES := $(wildcard $(EXDIR)/*.c)
EXAMPLES := $(basename $(notdir $(EXAMPLE_SOURCES)))
EXAMPLE_BINS := $(addprefix $(BINDIR)/,$(EXAMPLES))

# Docs related
DOCSBUILD := docs/build

.PHONY: all examples clean distclean help docs

# Default target
all: $(LIBRARY)
	@echo "Static library built: $(LIBRARY)"

examples: $(EXAMPLE_BINS)
	@echo "All examples built"

docs: $(DOCSBUILD)/docs.pdf $(DOCSBUILD)/html
	@echo "Docs generated"

$(DOCSBUILD)/html: $(LIB_HEADERS) $(DOCSBUILD)
	@doxygen

$(DOCSBUILD)/docs.pdf: docs/docs.tex $(DOCSBUILD)
	@xelatex -output-directory=$(DOCSBUILD) -shell-escape docs/docs.tex
	@xelatex -output-directory=$(DOCSBUILD) -shell-escape docs/docs.tex

# Build library
$(LIBRARY): $(LIB_OBJECTS) | $(BUILDDIR)
	@mkdir -p $(BUILDDIR)
	ar rcs $@ $(LIB_OBJECTS)
	@echo "Library created: $@"

# Compile library objects
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@
	@echo "Compiled: $<"

# Compile examples
$(BINDIR)/%: $(EXDIR)/%.c $(LIBRARY) | $(BINDIR)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) -I$(INCDIR) $< $(LIBRARY) $(LDFLAGS) -o $@
	@echo "Built example: $@"

# Create directories
$(BUILDDIR) $(OBJDIR) $(BINDIR) $(DOCSBUILD):
	@mkdir -p $@

# Clean build artifacts
clean: cleandocs
	rm -rf $(BUILDDIR)
	@echo "Cleaned build artifacts"

cleandocs:
	rm -rf $(DOCSBUILD)
