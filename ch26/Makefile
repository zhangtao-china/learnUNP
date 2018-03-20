CC = gcc
CFLAGS = -O2 -pipe
LFLAGS = -pthread
VPATH = ../base:.
BASEOBJS = wrapsocket.o wrapio.o readline.o error.o wrappthread.o wrapunix.o
OBJS = $(patsubst %.c,%.o, $(wildcard ./*.c))
DESDIR = ./bin
OBJSDIR = ./objs
RM = rm -rf

.PHONY : all
all : clientpthread servpthread

clientpthread : %:%.o $(BASEOBJS) $(DESDIR)
	$(CC) $(LFLAGS) $(patsubst %.o, $(OBJSDIR)/%.o, $(filter %.o, $^)) -o $(DESDIR)/$@

servpthread : %:%.o $(BASEOBJS) $(DESDIR)
	$(CC) $(LFLAGS) $(patsubst %.o, $(OBJSDIR)/%.o, $(filter %.o, $^)) -o $(DESDIR)/$@

$(BASEOBJS) $(OBJS) : %.o:%.c $(OBJSDIR)
	$(CC) -c $(CFLAGS) $< -o $(OBJSDIR)/$@

$(DESDIR):
	test -d $@ || mkdir -p $@

$(OBJSDIR):
	test -d $@ || mkdir -p $@

clean:
	-$(RM) $(OBJSDIR)
	-$(RM) $(DESDIR)