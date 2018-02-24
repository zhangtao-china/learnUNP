CC = gcc
CFLAGS = -pipe -O2
VPATH = ../base/:.
BASEOBJS = error.o wrapio.o wrapsocket.o wrapunix.o readline.o
OBJS = web.o
DISDIR = ./bin
OBJSDIR = ./objs
TARGET = web


$(TARGET) : %:%.o $(BASEOBJS) $(DISDIR)
	$(CC) $(CFLAGS) $(patsubst %.o, $(OBJSDIR)/%.o, $(filter %.o, $^)) -o $(DISDIR)/$@

$(BASEOBJS) : %.o:%.c $(OBJSDIR)
	$(CC) -c $(CFLAGS) $< -o $(OBJSDIR)/$@

$(OBJS) : %.o:%.c $(OBJSDIR)
	$(CC) -c $(CFLAGS) $< -o $(OBJSDIR)/$@

$(OBJSDIR):
	test -d $(OBJSDIR) || mkdir -p $@

$(DISDIR):
	test -d $(DISDIR) || mkdir -p $@

clean:
	-$(RM) -rf $(DISDIR)
	-$(RM) -rf $(OBJSDIR)