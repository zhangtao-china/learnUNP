CC = gcc
CFLAGS = -pipe -O2
VPATH = ../../base/:.
BASEOBJS = error.o wrapsocket.o wrapunix.o
OBJS = client_v5.o
DISDIR = ./bin
OBJSDIR = ${shell pwd}/objs

all:client_v5
.PHONY : all


client_v5 : %:%.o $(OBJSDIR) $(DISDIR) $(BASEOBJS) 
	$(CC) $(CFLAGS) $(BASEOBJS:%.o=$(OBJSDIR)/%.o) $(OBJSDIR)/$@.o -o $(DISDIR)/$@

$(BASEOBJS) : %.o:%.c
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