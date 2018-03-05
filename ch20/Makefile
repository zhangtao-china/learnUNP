CC = gcc
CFLAGS = -pipe -O2
VPATH = ../base/:.
BASEOBJS = error.o readline.o wrapio.o wrapsocket.o wrapunix.o
SOURCES := $(wildcard ./*.c)
OBJS := $(patsubst %.c,%.o,$(SOURCES))
DISDIR = ./bin
OBJSDIR = ./objs
TARGETS = $(patsubst %.c,%,$(SOURCES))
RM = rm -rf

COMDEP = $(BASEOBJS) $(DISDIR)

.PHONY : all
all : $(TARGETS)



dgclibcast1:%:%.o $(COMDEP)
	$(CC) $(CFLAGS) $(patsubst %.o, $(OBJSDIR)/%.o, $(filter %.o, $^)) -o $(DISDIR)/$@

dgclibcast2:%:%.o $(COMDEP)
	$(CC) $(CFLAGS) $(patsubst %.o, $(OBJSDIR)/%.o, $(filter %.o, $^)) -o $(DISDIR)/$@

dgclibcast3:%:%.o $(COMDEP)
	$(CC) $(CFLAGS) $(patsubst %.o, $(OBJSDIR)/%.o, $(filter %.o, $^)) -o $(DISDIR)/$@
	
dgclibcast4:%:%.o $(COMDEP)
	$(CC) $(CFLAGS) $(patsubst %.o, $(OBJSDIR)/%.o, $(filter %.o, $^)) -o $(DISDIR)/$@

dgclibcast5:%:%.o $(COMDEP)
	$(CC) $(CFLAGS) $(patsubst %.o, $(OBJSDIR)/%.o, $(filter %.o, $^)) -o $(DISDIR)/$@



$(BASEOBJS) $(OBJS):%.o:%.c $(OBJSDIR)
	$(CC) -c $(CFLAGS) $< -o $(OBJSDIR)/$@

$(DISDIR):
	test -d $(DISDIR) || mkdir -p $@

$(OBJSDIR):
	test -d $(OBJSDIR) || mkdir -p $@

clean:
	-$(RM) $(DISDIR)
	-$(RM) $(OBJSDIR)
