CC = gcc
CFLAGS = -O2 -pipe
LFLAGS = -pthread
VPATH = ../base:.
BASEOBJS = wrapsocket.o wrapio.o readline.o error.o wrappthread.o wrapunix.o
OBJS = $(patsubst %.c,%.o, $(wildcard ./*.c))
DESDIR = ./bin
TARGET = ping
OBJSDIR = ./objs
RM = rm -rf




$(TARGET) : %:%.o $(BASEOBJS) $(DESDIR)
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