CC = gcc
CFLAGS = -pipe -O2
VPATH = ../../base/:.
BASEOBJS = error.o readline.o wrapio.o wrapsocket.o wrapunix.o
OBJS = client_v3.o client_v4.o server_v3.o server_v4.o server_v5.o
DISDIR = ./bin

export OBJSDIR = ${shell pwd}/objs

all:client_v3 client_v4 server_v3 server_v4 server_v5
.PHONY : all


client_v3 : $(OBJSDIR) $(DISDIR) $(BASEOBJS) client_v3.o
	$(CC) $(CFLAGS) $(BASEOBJS:%.o=$(OBJSDIR)/%.o) $(OBJSDIR)/client_v3.o -o $(DISDIR)/client_v3

client_v4 : $(OBJSDIR) $(DISDIR) $(BASEOBJS)  client_v4.o
	$(CC) $(CFLAGS)  $(patsubst %.o, $(OBJSDIR)/%.o, $(BASEOBJS)) $(OBJSDIR)/client_v4.o -o $(DISDIR)/client_v4

server_v3 : $(OBJSDIR) $(DISDIR) $(BASEOBJS)  server_v3.o
	$(CC) $(CFLAGS)  $(BASEOBJS:%.o=$(OBJSDIR)/%.o) $(OBJSDIR)/server_v3.o -o $(DISDIR)/server_v3

server_v4 : $(OBJSDIR) $(DISDIR) $(BASEOBJS)  server_v4.o
	$(CC) $(CFLAGS)  $(BASEOBJS:%.o=$(OBJSDIR)/%.o) $(OBJSDIR)/server_v4.o -o $(DISDIR)/server_v4

server_v5 : $(OBJSDIR) $(DISDIR) $(BASEOBJS)  server_v5.o
	$(CC) $(CFLAGS)  $(BASEOBJS:%.o=$(OBJSDIR)/%.o) $(OBJSDIR)/server_v5.o -o $(DISDIR)/server_v5

$(BASEOBJS) : %.o:%.c
	$(CC) -c $(CFLAGS) $< -o $(OBJSDIR)/$@

$(OBJS) : %.o:%.c 
	$(CC) -c $(CFLAGS) $< -o $(OBJSDIR)/$@

$(OBJSDIR):
	test -d $(OBJSDIR) || mkdir -p $@

$(DISDIR):
	test -d $(DISDIR) || mkdir -p $@

clean:
	-$(RM) -rf $(DISDIR)
	-$(RM) -rf $(OBJSDIR)