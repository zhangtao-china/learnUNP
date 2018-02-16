CC = gcc
CFLAGS = 
VPATH = ../base
OBJS = error.o wrapsocket.o 
APPOBJS = hostent.o namesearch.o
OBJSDIR = objs
TARGETS = hostent namesearch
TARGETDIR = bin
RM = rm -rf

.PHONY : all
all : $(TARGETS)

hostent : %:%.o $(OBJS) $(TARGETDIR)
	$(CC) $(CFLAGS) $(OBJSDIR)/$@.o $(patsubst %.o, $(OBJSDIR)/%.o, $(filter %.o, $(OBJS))) -o $(TARGETDIR)/$@

namesearch : %:%.o $(OBJS) $(TARGETDIR)
	$(CC) $(CFLAGS) $(OBJSDIR)/$@.o $(patsubst %.o, $(OBJSDIR)/%.o, $(filter %.o, $(OBJS))) -o $(TARGETDIR)/$@

$(OBJS) : %.o:%.c $(OBJSDIR)
	$(CC) -c $(CFLAGS) $< -o $(OBJSDIR)/$@

$(APPOBJS) : %.o:%.c $(OBJSDIR)
	$(CC) -c $(CFLAGS) $< -o $(OBJSDIR)/$@

$(OBJSDIR):
	test -d $@ || mkdir -p $@

$(TARGETDIR):
	test -d $@ || mkdir -p $@

clean:
	-$(RM) $(OBJSDIR)
	-$(RM) $(TARGETDIR)