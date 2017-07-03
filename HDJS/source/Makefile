
#********define executable file name**********
#定义可执行程序的名称，需要根据实际情况进行更改
EXECUTABLE := hdjs

#********define shared libs used by the lib,separated by space*********
#使用的共享库列表，以空格分开, 需要根据实际情况更改
LIBS :=stdc++

#define c++ compiler,such as ppc_8xx-g++,g++
#定义使用的C++编译器,例如ppc_8xx-g++,g++等
CC=arm-uclibc-gcc

#define path of the shared lib to export
#输出的共享库所在路径
ifeq ($(EXPORTBASEPATH),)
EXPORTBASEPATH=/mnt/hgfs/HDJS
endif

EXPORTPATH:=$(EXPORTBASEPATH)
LIBPATH:=$(EXPORTPATH)/lib

#define include dir
#定义头文件包含
INCLUDEDIR := -I../include -I./applib -I./base -I./cfgfile -I./cfgfile/confuse -I./server -I./connect -I./sgui -I./msrule -I./sms

# Now alter any implicit rules' variables if you like, e.g.:
# 现在来改变任何你想改动的隐含规则中的变量，例如
CFLAGS := -Wall -O2 -D ARCH_ARM -D ARCH_ARM7 
CFLAGS += $(INCLUDEDIR)
CPPFLAGS += -MMD

RM-F := rm -f

# You shouldn't need to change anything below this point.
# 从这里开始，你应该不需要改动任何东西。

#define model rules for c and cpp files
#定义模式规则

%.o:%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@
%.o:%.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

SOURCE_DIRS = applib base cfgfile cfgfile/confuse server sgui connect msrule sms

SOURCE := $(wildcard *.c) $(wildcard *.cpp) 
SOURCE += $(foreach DIR,$(SOURCE_DIRS),$(wildcard $(DIR)/*.c $(DIR)/*.cpp))

ALLOBJS := $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCE)))
EXISTDEPS := $(wildcard *.d)
EXISTDEPS += $(foreach DIR,$(SOURCE_DIRS),$(wildcard $(DIR)/*.d))

.PHONY : everything objs clean veryclean rebuild debug
everything : $(EXECUTABLE)
objs : $(ALLOBJS)
clean :
	$(RM-F) $(ALLOBJS)
	$(RM-F) $(EXISTDEPS)
veryclean : clean
	$(RM-F) $(EXPORTPATH)/$(EXECUTABLE)
rebuild : veryclean everything
debug :
	@echo EXISTDEPS=$(EXISTDEPS)
	@echo ALLOBJS=$(ALLOBJS)

include $(EXISTDEPS)

$(EXECUTABLE) : $(ALLOBJS)
	$(CC) -L$(LIBPATH)/ -lm -s -o $(EXPORTPATH)/$(EXECUTABLE) $(ALLOBJS) $(addprefix -l,$(LIBS))

