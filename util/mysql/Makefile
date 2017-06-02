DIR_NAME=$(shell /bin/pwd | /usr/bin/awk -F / '{print $$NF}')

MODNAME=libutil_$(DIR_NAME)

DEV_PATH=../../
LIB_PATH=$(DEV_PATH)/libs
INCLUDE_PATH=$(DEV_PATH)/include/util

BUILD_INC = 
BUILD_LIB = 
STATIC_LIB = 

CXX=gcc -std=c++0x#g++-4.8

CP=/bin/cp -f
RM=/bin/rm -f
MV=/bin/mv -f
MKDIR=/bin/mkdir -p

SRC:= $(wildcard ./*.cpp)
CSRC:=$(wildcard ./*.c)
CCSRC:=$(wildcard ./*.cc)
SRC_H:=$(wildcard ./*.h)

OBJS := $(patsubst ./%.cpp, ./%.o, $(SRC))
COBJS:= $(patsubst ./%.c, ./%.o, $(CSRC))
CCOBJS:= $(patsubst ./%.cc, ./%.o, $(CCSRC))
OBJS_H:= $(patsubst ./%.h, $(INCLUDE_PATH)/${DIR_NAME}/%.h, $(SRC_H))

DEP:=$(OBJS:%.o=%.d)
CDEP:=$(COBJS:%.o=%d)
CCDEP:=$(CCOBJS:%.o=%d)

INCLUDES:= -I ${INCLUDE_PATH} -I ../../include
CFLAGS:= $(BUILD_INC) -Wall -ggdb -fPIC
LD_LIB= $(STATIC_LIB) $(BUILD_LIB)

TAGT=$(LIB_PATH)/$(MODNAME).a
	
all: begin $(TAGT)

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
$(OBJS_H) : $(INCLUDE_PATH)/${DIR_NAME}/%.h : ./%.h
	$(CP) $< $(INCLUDE_PATH)/${DIR_NAME}

$(TAGT):$(OBJS) $(COBJS) $(CCOBJS) $(OBJS_H)
	ar r $(TAGT) $(OBJS) $(COBJS) $(CCOBJS)
	ranlib $(TAGT)

.PHONY: begin
begin:
	$(MKDIR) $(INCLUDE_PATH)/${DIR_NAME}
	$(MKDIR) $(LIB_PATH)

.PHONY: clean
clean:
	$(RM) $(OBJS)
	$(RM) *.d
	$(RM) *.o
	$(RM) $(TAGT)
