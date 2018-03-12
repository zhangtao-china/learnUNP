CC = gcc
CFLAGS = -pipe -O2
VPATH = ../base/:.
BASEOBJS = error.o wrapio.o readline.o wrapsocket.o wrapunix.o
SOURCES := $(wildcard ./*.c)
OBJS := $(patsubst %.c,%.o,$(SOURCES))
DESTDIR = ./bin
OBJSDIR = ./objs
RM = rm -rf

TARGETS = tcpsend01 tcprecv01 tcprecv02 tcprecv03

COMDEP = $(BASEOBJS) $(DESTDIR)

.PHONY : all
all : $(TARGETS)

tcpsend01: %:%.o $(COMDEP)
	$(CC) $(CFLAGS) $(patsubst %.o, $(OBJSDIR)/%.o, $(filter %.o, $^)) -o $(DESTDIR)/$@

tcprecv01: %:%.o $(COMDEP)
	$(CC) $(CFLAGS) $(patsubst %.o, $(OBJSDIR)/%.o, $(filter %.o, $^)) -o $(DESTDIR)/$@

tcprecv02: %:%.o $(COMDEP)
	$(CC) $(CFLAGS) $(patsubst %.o, $(OBJSDIR)/%.o, $(filter %.o, $^)) -o $(DESTDIR)/$@

tcprecv03: %:%.o $(COMDEP)
	$(CC) $(CFLAGS) $(patsubst %.o, $(OBJSDIR)/%.o, $(filter %.o, $^)) -o $(DESTDIR)/$@

$(BASEOBJS) $(OBJS) : %.o:%.c $(OBJSDIR)
	$(CC) -c $(CFLAGS) $< -o $(OBJSDIR)/$@

$(DESTDIR):
	test -d $@ || mkdir -p $@

$(OBJSDIR):
	test -d $@ || mkdir -p $@

clean:
	-$(RM) $(DESTDIR)
	-$(RM) $(OBJSDIR) 