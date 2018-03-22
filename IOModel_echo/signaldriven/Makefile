CC = gcc
CFLAGS = -O2 -pipe
LFLAGS = -pthread
VPATH = ../../base/:.
BASEOBJS = error.o readline.o wrapio.o wrapsocket.o wrapunix.o 
SOURCES := $(wildcard ./*.c)
OBJS := $(patsubst %.c,%.o,$(SOURCES))
DESDIR = ./bin
OBJSDIR = ./objs
TARGET = dgecho
RM = rm -rf

COMDEP = $(BASEOBJS) $(DESDIR)


$(TARGET) : $(OBJS) $(COMDEP)
	$(CC) $(LFLAGS) $(CFLAGS) $(patsubst %.o, $(OBJSDIR)/%.o, $(filter %.o, $^)) -o $(DESDIR)/$@

$(BASEOBJS) $(OBJS) : %.o:%.c $(OBJSDIR)
	$(CC) -c $(CFLAGS) $< -o $(OBJSDIR)/$@

$(DESDIR):
	test -d $@ || mkdir -p $@

$(OBJSDIR):
	test -d $@ || mkdir -p $@

clean:
	-$(RM) $(OBJSDIR)
	-$(RM) $(DESDIR)