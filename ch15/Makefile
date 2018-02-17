CC = gcc
CFLAGS = -pipe -O2
VPATH = ../base/:.
BASEOBJS = error.o readline.o wrapio.o wrapsocket.o wrapunix.o
OBJS = unixstrcli01.o unixstrserv01.o unixdgcli01.o unixdgserv01.o
OBJSDIR = objs
DISDIR = ./bin


all:unixstrcli01 unixstrserv01 unixdgcli01 unixdgserv01

.PHONY : all


unixstrcli01 : $(OBJSDIR) $(DISDIR) $(BASEOBJS) unixstrcli01.o
	$(CC) $(CFLAGS) $(BASEOBJS:%.o=$(OBJSDIR)/%.o) $(OBJSDIR)/unixstrcli01.o -o $(DISDIR)/unixstrcli01


unixstrserv01 : $(OBJSDIR) $(DISDIR) $(BASEOBJS)  unixstrserv01.o
	$(CC) $(CFLAGS)  $(patsubst %.o, $(OBJSDIR)/%.o, $(BASEOBJS)) $(OBJSDIR)/unixstrserv01.o -o $(DISDIR)/unixstrserv01

unixdgcli01 : %:%.o $(OBJSDIR) $(DISDIR) $(BASEOBJS) 
	$(CC) $(CFLAGS)  $(patsubst %.o, $(OBJSDIR)/%.o, $(BASEOBJS)) $(OBJSDIR)/$@.o -o $(DISDIR)/$@

unixdgserv01 : %:%.o $(OBJSDIR) $(DISDIR) $(BASEOBJS) 
	$(CC) $(CFLAGS)  $(patsubst %.o, $(OBJSDIR)/%.o, $(BASEOBJS)) $(OBJSDIR)/$@.o -o $(DISDIR)/$@

$(BASEOBJS) : %.o:%.c
	$(CC) -c $(CFLAGS) $< -o $(OBJSDIR)/$@

$(OBJS) : %.o:%.c 
	$(CC) -c $(CFLAGS) $< -o $(OBJSDIR)/$@

$(OBJSDIR):
	test -d $(OBJSDIR) || mkdir -p ./$@

$(DISDIR):
	test -d $(DISDIR) || mkdir -p $@

clean:
	-$(RM) -rf $(DISDIR)
	-$(RM) -rf $(OBJSDIR)