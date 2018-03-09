CC = gcc
CFLAGS = -pipe -O2
VPATH = ../base/:.
BASEOBJS = error.o wrapio.o readline.o wrapsocket.o wrapunix.o
SOURCES := $(wildcard ./*.c)
OBJS := $(patsubst %.c,%.o,$(SOURCES))
DISDIR = ./bin
OBJSDIR = ./objs
RM = rm -rf

COMDEP = $(BASEOBJS) $(DISDIR)

TARGET = client_udp_v4



# $<表示目标集，如果目标只有一个而依赖有多个，那么命令只被展开一次
client_udp_v4: $(OBJS) $(COMDEP)
#	$(CC) $(CFLAGS) $(patsubst %.o, $(OBJSDIR)/%.o, $<) -o $(DISDIR)/$@
	$(CC) $(CFLAGS) $(patsubst %.o, $(OBJSDIR)/%.o, $(filter %.o, $^)) -o $(DISDIR)/$@


$(BASEOBJS) $(OBJS):%.o:%.c $(OBJSDIR)
	$(CC) -c $(CFLAGS) $< -o $(OBJSDIR)/$@

$(DISDIR):
	test -d $(DISDIR) || mkdir -p $@

$(OBJSDIR):
	test -d $(OBJSDIR) || mkdir -p $@

clean:
	-$(RM) $(DISDIR)
	-$(RM) $(OBJSDIR)
