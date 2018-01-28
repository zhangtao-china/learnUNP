CC = gcc
CFLAGS = -pipe -O2
VPATH = ../../base/:.
BASEOBJS = error.o readline.o wrapio.o wrapsocket.o wrapunix.o
OBJS = client_v1.o client_v2.o server_v1.o server_v2.o 
OBJSDIR = objs
DISDIR = ./bin

all:client_v1 client_v2 server_v1 server_v2
.PHONY : all

####################################
# 替换方式一:
# 使用文本替换规则, $(BASEOBJS:%.o=$(OBJSDIR)/%.o)表示
# 将BASEOBJS中以空格分隔的并以.o结尾的字符串替换为$(OBJSDIR)/%.o,即在匹配到的字符串前添加$(OBJSDIR)
####################################
client_v1 : $(OBJSDIR) $(DISDIR) $(BASEOBJS) client_v1.o
	$(CC) $(CFLAGS) $(BASEOBJS:%.o=$(OBJSDIR)/%.o) $(OBJSDIR)/client_v1.o -o $(DISDIR)/client_v1

####################################
# 替换方式一:
# 使用文本处理函数patsubst
####################################
client_v2 : $(OBJSDIR) $(DISDIR) $(BASEOBJS)  client_v2.o
	$(CC) $(CFLAGS)  $(patsubst %.o, $(OBJSDIR)/%.o, $(BASEOBJS)) $(OBJSDIR)/client_v2.o -o $(DISDIR)/client_v2

server_v1 : $(OBJSDIR) $(DISDIR) $(BASEOBJS)  server_v1.o
	$(CC) $(CFLAGS)  $(BASEOBJS:%.o=$(OBJSDIR)/%.o) $(OBJSDIR)/server_v1.o -o $(DISDIR)/server_v1

server_v2 : $(OBJSDIR) $(DISDIR) $(BASEOBJS)  server_v2.o
	$(CC) $(CFLAGS)  $(BASEOBJS:%.o=$(OBJSDIR)/%.o) $(OBJSDIR)/server_v2.o -o $(DISDIR)/server_v2

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