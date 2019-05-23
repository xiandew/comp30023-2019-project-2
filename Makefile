# Created by Xiande Wen (xiandew, 905003)

CC     = gcc
CFLAGS = -Wall -std=c99
EXE    = crack
OBJDIR = bin
OBJ    = $(addprefix $(OBJDIR)/, sha256.o char_distribution.o main.o)
DEPS   = sha256.h char_distribution.h

# top (default) targets
all: $(EXE)

# how to link executable
$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJ)

$(OBJ): | $(OBJDIR)

$(OBJDIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJDIR):
	mkdir -p $@

# phony targets (these targets do not represent actual files)
.PHONY: clean cleanly all CLEAN

# `make clean` to remove all object files
# `make CLEAN` to remove all object and executable files
clean:
	rm -f -r $(OBJDIR)
CLEAN: clean
	rm -f $(EXE)
