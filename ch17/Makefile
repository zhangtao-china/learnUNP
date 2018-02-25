CC = gcc
CFLAGS = -pipe -O2 -g
VPATH = ../base/:.
BASEOBJS =error.o wrapsocket.o wrapunix.o get_ifi_info.o 
OBJS = prifinfo.o prmac.o 
DISDIR = ./bin
OBJSDIR = ./objs
TARGET = prifinfo prmac

all: $(TARGET)
.PHONY : all

prifinfo : %:%.o $(BASEOBJS) $(DISDIR)
	$(CC) $(CFLAGS) $(patsubst %.o, $(OBJSDIR)/%.o, $(filter %.o, $^)) -o $(DISDIR)/$@

prmac : %:%.o $(BASEOBJS) $(DISDIR)
	$(CC) $(CFLAGS) $(patsubst %.o, $(OBJSDIR)/%.o, $(filter %.o, $^)) -o $(DISDIR)/$@

$(OBJS) : %.o:%.c $(OBJSDIR)
	$(CC) -c $(CFLAGS) $< -o $(OBJSDIR)/$@

$(BASEOBJS) : %.o:%.c $(OBJSDIR)
	$(CC) -c $(CFLAGS) $< -o $(OBJSDIR)/$@

$(OBJSDIR):
	test -d $(OBJSDIR) || mkdir -p $@

$(DISDIR):
	test -d $(DISDIR) || mkdir -p $@

clean:
	-$(RM) -rf $(DISDIR)
	-$(RM) -rf $(OBJSDIR)