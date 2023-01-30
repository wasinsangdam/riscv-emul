# Compiler
CC = g++
CXXLFAGS = -Wall -g -O2

# Target
TARGET = main

# Directories (sources, includes, objects)
MKDIR   := @mkdir -p
SRCDIR 	:= ./src
INCDIR	:= ./inc
OBJDIR	:= ./obj

SRCEXT	:= cc
INCEXT 	:= h
OBJEXT	:= o
DEPEXT	:= d

# Files
SOURCES 	:= $(notdir $(wildcard $(SRCDIR)/*.$(SRCEXT)))
HEADERS		:= $(notdir $(wildcard $(INCDIR)/*.$(INCEXT)))

OBJS		:= $(SOURCES:.$(SRCEXT)=.$(OBJEXT))
DEPS		:= $(SOURCES:.$(SRCEXT)=.$(DEPEXT))
OBJECTS		:= $(patsubst %.$(OBJEXT),$(OBJDIR)/%.$(OBJEXT),$(OBJS))
DEPS		:= $(OBJECTS:.$(OBJEXT)=.$(DEPEXT))

# Compile
#$(OBJDIR):
#	$(MKDIR) $(OBJDIR) 

$(OBJDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
		$(MKDIR) $(OBJDIR)
		$(CC) $(CXXLFAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
		$(CC) $(CXXLFAGS) $(OBJECTS) -o $(TARGET)
clean:
		rm -rf $(OBJDIR) $(TARGET)

