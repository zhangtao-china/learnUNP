CC = gcc
CFLAGS = 
VPATH = ../base
OBJS = error.o wrapsocket.o hostent.o
OBJSDIR = objs
TARGET = hostent
TARGETDIR = bin
RM = rm -rf

$(TARGET) : $(OBJS) $(TARGETDIR)
	$(CC) $(CFLAGS) $(patsubst %.o, $(OBJSDIR)/%.o, $(filter %.o, $^)) -o $(TARGETDIR)/$@

$(OBJS) : %.o:%.c $(OBJSDIR)
	$(CC) -c $(CFLAGS) $< -o $(OBJSDIR)/$@

$(OBJSDIR):
	test -d $@ || mkdir -p $@

$(TARGETDIR):
	test -d $@ || mkdir -p $@

clean:
	-$(RM) $(OBJSDIR)
	-$(RM) $(TARGETDIR)