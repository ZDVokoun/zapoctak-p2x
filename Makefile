CC := gcc

CFLAGS := -Wall -Wextra -g -std=c23 -fPIC -MMD -MP

LDFLAGS := 
LDLIBS := -lm

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
SHARED_LIBRARY := $(BUILDDIR)/libzapoctak.so

# Example files
EXAMPLE_SOURCES := $(wildcard $(EXDIR)/*.c)
EXAMPLES := $(basename $(notdir $(EXAMPLE_SOURCES)))
EXAMPLE_BINS := $(addprefix $(BINDIR)/,$(EXAMPLES))

# Docs related
DOCSBUILD := docs/build

.PHONY: all examples clean cleandocs docs library sharedlibrary

# Default target now builds both libraries
all: library sharedlibrary examples docs
	@echo "Libraries built: $(LIBRARY) and $(SHARED_LIBRARY)"
	@echo "Examples built: $(EXAMPLE_BINS)"
	@echo "Main docs built: $(DOCSBUILD)/docs.pdf"
	@echo "Doxygen docs built: $(DOCSBUILD)/html/index.html"

library: $(LIBRARY)

sharedlibrary: $(SHARED_LIBRARY)

examples: $(EXAMPLE_BINS)
	@echo "All examples built"

docs: $(DOCSBUILD)/docs.pdf $(DOCSBUILD)/.doxygen_done
	@echo "Docs generated"

$(DOCSBUILD)/.doxygen_done: $(LIB_HEADERS) | $(DOCSBUILD)
	@doxygen
	@touch $@

$(DOCSBUILD)/docs.pdf: docs/docs.tex | $(DOCSBUILD)
	@xelatex -output-directory=$(DOCSBUILD) -shell-escape docs/docs.tex
	@xelatex -output-directory=$(DOCSBUILD) -shell-escape docs/docs.tex

$(LIBRARY): $(LIB_OBJECTS) | $(BUILDDIR)
	ar rcs $@ $(LIB_OBJECTS)
	@echo "Static library created: $@"

$(SHARED_LIBRARY): $(LIB_OBJECTS) | $(BUILDDIR)
	$(CC) -shared -o $@ $(LIB_OBJECTS) $(LDFLAGS) $(LDLIBS)
	@echo "Shared library created: $@"

# Compile library objects
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@
	@echo "Compiled: $<"

# Compile examples
$(BINDIR)/%: $(EXDIR)/%.c $(LIBRARY) | $(BINDIR)
	$(CC) $(CFLAGS) -I$(INCDIR) $< $(LIBRARY) $(LDFLAGS) $(LDLIBS) -o $@
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

# Include the generated dependency files
-include $(LIB_OBJECTS:.o=.d)
