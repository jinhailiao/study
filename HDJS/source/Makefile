
#********define executable file name**********
#�����ִ�г�������ƣ���Ҫ����ʵ��������и���
EXECUTABLE := hdjs

#********define shared libs used by the lib,separated by space*********
#ʹ�õĹ������б����Կո�ֿ�, ��Ҫ����ʵ���������
LIBS :=stdc++

#define c++ compiler,such as ppc_8xx-g++,g++
#����ʹ�õ�C++������,����ppc_8xx-g++,g++��
CC=arm-uclibc-gcc

#define path of the shared lib to export
#����Ĺ���������·��
ifeq ($(EXPORTBASEPATH),)
EXPORTBASEPATH=/mnt/hgfs/HDJS
endif

EXPORTPATH:=$(EXPORTBASEPATH)
LIBPATH:=$(EXPORTPATH)/lib

#define include dir
#����ͷ�ļ�����
INCLUDEDIR := -I../include -I./applib -I./base -I./cfgfile -I./cfgfile/confuse -I./server -I./connect -I./sgui -I./msrule -I./sms

# Now alter any implicit rules' variables if you like, e.g.:
# �������ı��κ�����Ķ������������еı���������
CFLAGS := -Wall -O2 -D ARCH_ARM -D ARCH_ARM7 
CFLAGS += $(INCLUDEDIR)
CPPFLAGS += -MMD

RM-F := rm -f

# You shouldn't need to change anything below this point.
# �����￪ʼ����Ӧ�ò���Ҫ�Ķ��κζ�����

#define model rules for c and cpp files
#����ģʽ����

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
