
exec:
	cd ../base; pwd   # print (Makefile'sDir/../base)

#make会一条一条的执行其后的命令。需要注意的是，如果你要让上一条命令的结果应用在下一条命令时，你应该使用分号分隔这两条命令
#exec:
#	cd ../base
#	pwd                # print (Makefile'sDir)

#每当命令运行完后，make会检测每个命令的返回码，如果命令返回成功，那么make会执行下一条命令，
#当规则中所有的命令成功返回后，这个规则就算是成功完成了。如果一个规则中的某个命令出错了（命令退出码非零），那么make就会终止执行当前规则，这将有可能终止所有规则的执行
#有些时候，命令的出错并不表示就是错误的
#为了忽略命令的出错，我们可以在Makefile的命令行前加一个减号“-”（在Tab键之后），标记为不管命令出不出错都认为是成功的
# .IGNORE -k
createdir:
	-mkdir test


#-----------------------------------------------嵌套执行make----------------------------------
#假设有一个子目录subdir
subsystem:
	cd subdir && $(MAKE)
#其等价于：
subsystem:
	$(MAKE) -C subdir

#总控Makefile的变量可以传递到下级的Makefile中（如果你显示的声明），但是不会覆盖下层的Makefile中所定义的变量，除非指定了“-e”参数

#传递变量到下级Makefile中
export <variable ...>
#不想让某些变量传递到下级Makefile中
unexport <variable ...>

#示例一：
export variable = value
#其等价于：
variable = value
export variable
#其等价于：
export variable := value
#其等价于：
variable := value
export variable

#示例二：
export variable += value
#其等价于：
variable += value
export variable

#如果要传递所有的变量，那么，只要一个export就行了。后面什么也不用跟，表示传递所有的变量
export

#有两个变量，一个是SHELL，一个是MAKEFLAGS，这两个变量不管你是否export，其总是要传递到下层Makefile中，特别是MAKEFILES变量，其中包含了make的参数信息，
#如果我们执行“总控Makefile”时有make参数或是在上层Makefile中定义了这个变量，那么MAKEFILES变量将会是这些参数，并会传递到下层Makefile中，这是一个系统级的环境变量
# MAKEFILES
# SHELL

#定义命令包 define endef
define run-yacc
yacc $(firstword $^)
mv y.tab.c $@
endef
#“run-yacc”是这个命令包的名字，其不要和Makefile中的变量重名

#usage example
foo.c : foo.y
$(run-yacc)

#-----------------------------------------------使用变量----------------------------------
#一、变量的基础
#1.在Makefile中的定义的变量，就像是C/C++语言中的宏一样，他代表了一个文本字串，在Makefile中执行的时候其会自动原模原样地展开在所使用的地方。
#  其与C/C++所不同的是，你可以在Makefile中改变其值。在Makefile中，变量可以使用在“目标”，“依赖目标”，“命令”或是Makefile的其它部分中
#2.变量的命名字可以包含字符、数字，下划线（可以是数字开头），但不应该含有“:”、“#”、“=”或是空字符（空格、回车等）。
#  变量是大小写敏感的，“foo”、“Foo”和“FOO”是三个不同的变量名
#3.变量在声明时需要给予初值，而在使用时，需要给在变量名前加上“$”符号，但最好用小括号“（）”或是大括号“{}”把变量给包括起来。如果要使用真实的“$”字符，那么需要用“$$”来表示
#4.变量会在使用它的地方精确地展开

#二、变量中的变量
#在定义变量的值时，我们可以使用其它变量来构造变量的值，在Makefile中有两种方式来在用变量定义变量的值
#1.简单的使用“=”号，在“=”左侧是变量，右侧是变量的值，右侧变量的值可以定义在文件的任何一处，也就是说，右侧中的变量不一定非要是已定义好的值，其也可以使用后面定义的值
#ex:
foo = $(bar)
bar = $(ugh)
ugh = Huh?
all:
echo $(foo)
#如果在变量中使用函数，那么，这种方式会让我们的make运行时非常慢，更糟糕的是，他会使用得两个make的函数“wildcard”和“shell”发生不可预知的错误
#为了避免上面的这种方法，我们可以使用make中的另一种用变量来定义变量的方法。这种方法使用的是“:=”操作符

x := foo
y := $(x) bar
x := later
#其等价于：
y := foo bar
x := later

#这种方法，前面的变量不能使用后面的变量，只能使用前面已定义好了的变量。如果是这样：
y := $(x) bar
x := foo
#y的值是“bar”，而不是“foo bar”

#三、变量高级用法
#1. 变量值的替换。我们可以替换变量中的共有的部分，其格式是“$(var:a=b)”或是“${var:a=b}”，
#   其意思是，把变量“var”中所有以“a”字串“结尾”的“a”替换成“b”字串。这里的“结尾”意思是“空格”或是“结束符”
foo := a.o b.o c.o
bar := $(foo:.o=.c)
# “$(bar)”的值是“a.c b.c c.c”。
foo := a.o b.o c.o
bar := $(foo:%.o=%.c)
#这依赖于被替换字串中的有相同的模式，模式中必须包含一个“%”字符，这个例子同样让$(bar)变量的值为“a.c b.c c.c”

#2. 把变量的值再当成变量
x = y
y = z
z = u
a := $($($(x)))
#$(a)的值是“u”

first_second = Hello
a = first
b = second
all = $($a_$b)
#“$a_$b”组成了“first_second”，于是，$(all)的值就是“Hello”。

#综合例子
#(1)
ifdef do_sort
func := sort
else
func := strip
endif
bar := a d b g q c
foo := $($(func) $(bar))

#(2)
dir = foo
$(dir)_sources := $(wildcard $(dir)/*.c)
define $(dir)_print
lpr $($(dir)_sources)
endef


#四、追加变量值
#如果变量之前没有定义过，那么，“+=”会自动变成“=”，如果前面有变量定义，那么“+=”会继承于前次操作的赋值符。如果前一次的是“:=”，那么“+=”会以“:=”作为其赋值符
objects = main.o foo.o bar.o utils.o
objects += another.o
#ex:
variable := value
variable += more
#等价于：
variable := value
variable := $(variable) more

#五、override 指示符
#如果有变量是通常make的命令行参数设置的，那么Makefile中对这个变量的赋值会被忽略。
#如果你想在Makefile中设置这类参数的值，那么，你可以使用“override”指示符。其语法是：
override <variable> = <value>
override <variable> := <value>
#当然，你还可以追加：
override <variable> += <more text>
#对于多行的变量定义，我们用define指示符，在define指示符前，也同样可以使用ovveride指示符，如：
override define foo
bar
endef

#六、多行变量
#还有一种设置变量值的方法是使用define关键字。使用define关键字设置变量的值可以有换行，这有利于定义一系列的命令（前面我们讲过“命令包”的技术就是利用这个关键字）。
#define指示符后面跟的是变量的名字，而重起一行定义变量的值，定义是以endef关键字结束。其工作方式和“=”操作符一样。
#变量的值可以包含函数、命令、文字，或是其它变量。因为命令需要以[Tab]键开头，所以如果你用define定义的命令变量中没有以[Tab]键开头，那么make就不会把其认为是命令。
#ex:
define two-lines
echo foo
echo $(bar)
endef

#七、环境变量

#八、目标变量
<target ...> : <variable-assignment>
<target ...> : overide <variable-assignment>
#当我们设置了这样一个变量，这个变量会作用到由这个目标所引发的所有的规则中去
#ex:
prog : CFLAGS = -g
prog : prog.o foo.o bar.o
$(CC) $(CFLAGS) prog.o foo.o bar.o

prog.o : prog.c
$(CC) $(CFLAGS) prog.c
foo.o : foo.c
$(CC) $(CFLAGS) foo.c
bar.o : bar.c
$(CC) $(CFLAGS) bar.c

#九、模式变量

#-----------------------------------------------使用条件判断----------------------------------
#使用条件判断，可以让make根据运行时的不同情况选择不同的执行分支。条件表达式可以是比较变量的值，或是比较变量和常量的值
#条件表达式的语法为：
#<conditional-directive>
#<text-if-true>
#endif
#以及：
#<conditional-directive>
#<text-if-true>
#else
#<text-if-false>
#endif
#其中<conditional-directive>表示条件关键字，如“ifeq”。这个关键字有四个。
#第一个是“ifeq”
ifeq (<arg1>, <arg2>)
ifeq '<arg1>' '<arg2>'
ifeq "<arg1>" "<arg2>"
ifeq "<arg1>" '<arg2>'
ifeq '<arg1>' "<arg2>"
#比较参数“arg1”和“arg2”的值是否相同。当然，参数中我们还可以使用make的函数
#其比较参数“arg1”和“arg2”的值是否相同，如果不同，则为真

#第二个条件关键字是“ifneq”。语法是：
ifneq (<arg1>, <arg2>)
ifneq '<arg1>' '<arg2>'
ifneq "<arg1>" "<arg2>"
ifneq "<arg1>" '<arg2>'
ifneq '<arg1>' "<arg2>"
#其比较参数“arg1”和“arg2”的值是否相同，如果不同，则为真

#第三个条件关键字是“ifdef”。语法是：
ifdef <variable-name>
#如果变量<variable-name>的值非空，那到表达式为真。否则，表达式为假.<variable-name>同样可以是一个函数的返回值。
#注意，ifdef只是测试一个变量是否有值，其并不会把变量扩展到当前位置

#示例一：
bar =
foo = $(bar)
ifdef foo
frobozz = yes
else
frobozz = no
endif
#示例二：
foo =
ifdef foo
frobozz = yes
else
frobozz = no
endif
#第一个例子中，“$(frobozz)”值是“yes”，第二个则是“no”。 ★★★★★

#第四个条件关键字是“ifndef”。其语法是：
ifndef <variable-name>
#和“ifdef”是相反的意思

#在<conditional-directive>这一行上，多余的空格是被允许的，但是不能以[Tab]键做为开始（不然就被认为是命令）。
#而注释符“#”同样也是安全的。“else”和“endif”也一样，只要不是以[Tab]键开始就行了

#-----------------------------------------------使用函数----------------------------------
#函数调用，很像变量的使用，也是以“$”来标识的，其语法如下：
$(<function> <arguments>)
#或是
${<function> <arguments>}
#这里，<function>就是函数名，make支持的函数不多。<arguments>是函数的参数，参数间以逗号“,”分隔，而函数名和参数之间以“空格”分隔。
#函数调用以“$”开头，以圆括号或花括号把函数名和参数括起。
#ex:
comma:= ,
empty:=
space:= $(empty) $(empty)
foo:= a b c
bar:= $(subst $(space),$(comma),$(foo))
# $(comma)的值是一个逗号。$(space)使用了$(empty)定义了一个空格，$(foo)的值是“a b c”
# 这个函数也就是把$(foo)中的空格替换成逗号，所以$(bar)的值是“a,b,c”