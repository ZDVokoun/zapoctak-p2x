# Makefile for zapoctak project
# Build: make
# Build examples: make examples
# Clean: make clean

CC := gcc
CFLAGS := -Wall -Wextra -O2 -std=c23 -fPIC
LDFLAGS := -lm

# Directories
SRCDIR := src
INCDIR := include
EXDIR := examples
BUILDDIR := build
BINDIR := $(BUILDDIR)/bin
OBJDIR := $(BUILDDIR)/obj

# Library files
LIB_SOURCES := $(SRCDIR)/config.c $(SRCDIR)/mp_number.c $(SRCDIR)/residue.c $(SRCDIR)/conversion.c
LIB_OBJECTS := $(addprefix $(OBJDIR)/,$(notdir $(LIB_SOURCES:.c=.o)))
LIBRARY := $(BUILDDIR)/libzapoctak.a

# Example files
EXAMPLES := example_b64_conversion example_residue_conversion example_multiply example_add example_subtract example_square
EXAMPLE_SOURCES := $(addprefix $(EXDIR)/,$(addsuffix .c,$(EXAMPLES)))
EXAMPLE_BINS := $(addprefix $(BINDIR)/,$(EXAMPLES))

.PHONY: all examples clean distclean help

# Default target
all: $(LIBRARY)
	@echo "Static library built: $(LIBRARY)"

examples: $(EXAMPLE_BINS)
	@echo "All examples built"

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
$(BUILDDIR) $(OBJDIR) $(BINDIR):
	@mkdir -p $@

# Clean build artifacts
clean:
	rm -rf $(BUILDDIR)
	@echo "Cleaned build artifacts"

# Full distclean
distclean: clean
	@echo "Full clean complete"

# Help target
help:
	@echo "zapoctak - Multi-precision arithmetic with residue number system"
	@echo ""
	@echo "Available targets:"
	@echo "  make              - Build the static library"
	@echo "  make examples     - Build example programs"
	@echo "  make clean        - Remove build artifacts"
	@echo "  make distclean    - Full clean"
	@echo "  make help         - Show this help message"
	@echo ""
	@echo "Project structure:"
	@echo "  zapoctak/include/  - Public header files"
	@echo "  zapoctak/src/      - Library source files"
	@echo "  zapoctak/examples/ - Example programs"
	@echo "  build/             - Build output directory"
