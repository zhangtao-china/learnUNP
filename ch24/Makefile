CC = gcc
CFLAGS = -pipe -O2
VPATH = ../base/:.
BASEOBJS = error.o wrapio.o readline.o wrapsocket.o wrapunix.o heartbeatcli.o heartbeatserv.o
SOURCES := $(wildcard ./*.c)
OBJS := $(patsubst %.c,%.o,$(SOURCES))
DESTDIR = ./bin
OBJSDIR = ./objs
RM = rm -rf

TARGETS = tcpsend01 tcprecv01 tcprecv02 tcprecv03 tcpsend04 tcprecv04 tcpsend05 tcprecv05 tcpsend06 echoclient echoserver

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

tcpsend04: %:%.o $(COMDEP)
	$(CC) $(CFLAGS) $(patsubst %.o, $(OBJSDIR)/%.o, $(filter %.o, $^)) -o $(DESTDIR)/$@

tcprecv04: %:%.o $(COMDEP)
	$(CC) $(CFLAGS) $(patsubst %.o, $(OBJSDIR)/%.o, $(filter %.o, $^)) -o $(DESTDIR)/$@

tcpsend05: %:%.o $(COMDEP)
	$(CC) $(CFLAGS) $(patsubst %.o, $(OBJSDIR)/%.o, $(filter %.o, $^)) -o $(DESTDIR)/$@

tcprecv05: %:%.o $(COMDEP)
	$(CC) $(CFLAGS) $(patsubst %.o, $(OBJSDIR)/%.o, $(filter %.o, $^)) -o $(DESTDIR)/$@

tcpsend06: %:%.o $(COMDEP)
	$(CC) $(CFLAGS) $(patsubst %.o, $(OBJSDIR)/%.o, $(filter %.o, $^)) -o $(DESTDIR)/$@

echoclient: %:%.o $(COMDEP)
	$(CC) $(CFLAGS) $(patsubst %.o, $(OBJSDIR)/%.o, $(filter %.o, $^)) -o $(DESTDIR)/$@

echoserver: %:%.o $(COMDEP)
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