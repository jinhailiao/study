#define executable file name
EXECUTABLE := dyjc
EXE_OUTDIR := ../../mnt/dyjc/program/
CORE_SO := fnd mproc baseb nwb
APP_SO := share
APP_AR := dbcj httx lcd ppp jlcy fhkz ctcy mpcfg

#define shared libs used by the lib,separated by space 
LIBS := stdc++ m $(CORE_SO) $(APP_SO) $(APP_AR)
LIBPATH := .

#define c++ compiler,such as ppc_8xx-g++,g++
CC=arm-uclibc-gcc
CORE_INCLUDE := -I../../../coreassets/foundation -I../../../coreassets/foundation/confuse -I../../../coreassets/foundation/sgui -I../../../coreassets/mprocess -I../../../coreassets/basebusiness -I../../../coreassets/nwbusiness
HEAD_INCLUDE := $(CORE_INCLUDE) -I. -I.. -I../mpcfg -I../share -I../dbcj -I../httx -I../ppp -I../lcd -I../jlcy -I../fhkz -I../ctcy
CFLAGS := -Wall -Werror -fPIC -O2 -D ARCH_ARM -D ARCH_ARM7 $(HEAD_INCLUDE)
CPPFLAGS += -MMD
RM-F := rm -f

#define model rules for c and cpp files
%.o:%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@
%.o:%.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

SOURCE_DIRS :=
SOURCE := $(wildcard *.c) $(wildcard *.cpp) 
SOURCE += $(foreach DIR,$(SOURCE_DIRS),$(wildcard $(DIR)/*.c $(DIR)/*.cpp))

ALLOBJS := $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCE)))
EXISTDEPS := $(wildcard *.d)
EXISTDEPS += $(foreach DIR,$(SOURCE_DIRS),$(wildcard $(DIR)/*.d))

LIBS_APP_AR := $(addprefix lib,$(addsuffix .a, $(APP_AR)))
LIBS_APP_SO := $(addprefix lib,$(addsuffix .so, $(APP_SO)))
LIBS_CORE_SO := $(addprefix ../../../coreassets/lib,$(addsuffix .so, $(CORE_SO)))

.PHONY : everything objs clean veryclean rebuild debug
everything : $(EXECUTABLE)
objs : $(ALLOBJS)
clean :
	$(RM-F) $(ALLOBJS)
	$(RM-F) $(EXISTDEPS)
veryclean : clean
	$(RM-F) $(EXECUTABLE)
rebuild : veryclean everything
debug :
	@echo EXISTDEPS=$(EXISTDEPS)
	@echo ALLOBJS=$(ALLOBJS)

include $(EXISTDEPS)

$(EXECUTABLE) : $(ALLOBJS) $(LIBS_APP_SO) $(LIBS_APP_AR)
	cp -f $(LIBS_CORE_SO) ./
	$(CC) -L$(LIBPATH)/ -export-dynamic -s -o $(EXECUTABLE) $(ALLOBJS) $(addprefix -l,$(LIBS))
	cp -f $(LIBS_CORE_SO) $(EXE_OUTDIR)
	cp -f $(LIBS_APP_SO) $(EXE_OUTDIR)
	cp -f $(EXECUTABLE) $(EXE_OUTDIR)
