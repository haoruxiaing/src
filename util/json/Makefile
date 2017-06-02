DIR_NAME=$(shell /bin/pwd | /usr/bin/awk -F / '{print $$NF}')

MODNAME=lib_$(DIR_NAME)

DEV_PATH=../..
LIB_PATH=$(DEV_PATH)/libs
INCLUDE_PATH=$(DEV_PATH)/include

BUILD_INC = 
BUILD_LIB = 
STATIC_LIB = 

CP=/bin/cp -f
RM=/bin/rm -f
MV=/bin/mv -f
MKDIR=/bin/mkdir -p

CXX=gcc #g++-4.8

SRC:= $(wildcard ./*.cpp)
CSRC:=$(wildcard ./*.c)
CCSRC:=$(wildcard ./*.cc)
OBJS := $(patsubst ./%.cpp, ./%.o, $(SRC))
COBJS:= $(patsubst ./%.c, ./%.o, $(CSRC))
CCOBJS:= $(patsubst ./%.cc, ./%.o, $(CCSRC))
DEP:=$(OBJS:%.o=%.d)
CDEP:=$(COBJS:%.o=%d)
CCDEP:=$(CCOBJS:%.o=%d)

INCLUDES:= -I ${INCLUDE_PATH} -I ../../includea -I../
CFLAGS:= $(BUILD_INC) -Wall -ggdb -fPIC -std=c++0x
LD_LIB= $(STATIC_LIB) $(BUILD_LIB)

TAGT=$(LIB_PATH)/$(MODNAME).a
	
all: $(TAGT)

ok: clean all

-include $(DEP) $(CDEP) $(CCDEP)

$(OBJS) : ./%.o : ./%.cpp
	$(CXX) $(CFLAGS) $(INCLUDES) -MM -MT $@ -MF $(patsubst %.o, %.d, $@) $<
	$(CXX) $(CFLAGS) -c $< -o $@ $(INCLUDES)
$(COBJS) : ./%.o : ./%.c
	$(CXX) $(CFLAGS) $(INCLUDES) -MM -MT $@ -MF $(patsubst %.o, %.d, $@) $<
	$(CXX) $(CFLAGS) -c $< -o $@ $(INCLUDES)
$(CCOBJS) : ./%.o : ./%.cc
	$(CXX) $(CFLAGS) $(INCLUDES) -MM -MT $@ -MF $(patsubst %.o, %.d, $@) $<
	$(CXX) $(CFLAGS) -c $< -o $@ $(INCLUDES)

$(TAGT):$(OBJS) $(COBJS) $(CCOBJS) 
	$(MKDIR) $(LIB_PATH)
	ar r $(TAGT) $(OBJS) $(COBJS) $(CCOBJS)
	ranlib $(TAGT)

.PHONY: clean
clean:
	$(RM) $(OBJS)
	$(RM) *.d
	$(RM) $(TAGT)
