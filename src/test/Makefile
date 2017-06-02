SRCDIR = .
SRCINC = -I. -I../ -I../include -I../../include -I../../include/util  -I ../../include/libmongoc-1.0 -I ../../include/libbson-1.0
OBJDIR = .
LIBDIR = -L../../libs -L../libs -L../protocol/libs
BUILDDIR = ../../build

DB_DIR=./dbc
COMMON_DIR=./gen-cpp
HANDLERS_DIR=./handlers
CXX=gcc -lstdc++#g++-4.8
CFLAGS:= -Wall -g -Werror -Wno-deprecated -I.. -fPIC -static
LDFLAGS:= -lutil_json -lutil_glog -lpthread

NAME:=$(shell /bin/pwd | /usr/bin/awk -F / '{print $$NF}')
TARGET:=$(BUILDDIR)/$(NAME)/$(NAME)

SRC := $(wildcard $(SRCDIR)/*.cpp)
DB_SRC = $(wildcard $(DB_DIR)/*.cpp)
COMMON_SRC = $(wildcard $(COMMON_DIR)/*.cpp)
HANDLERS_SRC = $(wildcard $(HANDLERS_DIR)/*.cpp)
SRC_LIB := $(wildcard ../../libs/*.a)

OBJS := $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC))
DB_OBJS = $(patsubst $(DB_DIR)/%.cpp, $(OBJDIR)/%.o, $(DB_SRC))
COMMON_OBJS = $(patsubst $(COMMON_DIR)/%.cpp, $(OBJDIR)/%.o, $(COMMON_SRC))
HANDLERS_OBJS = $(patsubst $(HANDLERS_DIR)/%.cpp, $(OBJDIR)/%.o, $(HANDLERS_SRC))

DEP:=$(OBJS:%.o=%.d)
DDEP:=$(DB_OBJS:%.o=%.d)
CDEP:=$(COMMON_OBJS:%.o=%.d)
HDEP:=$(HANDLERS_OBJS:%.o=%.d)

.PHONY : all deps clean veryclean install objs cinc

all : $(TARGET)

objs : $(OBJS) $(DB_OBJS) $(COMMON_OBJS) $(HANDLERS_OBJS)

-include $(DEP) $(DDEP) $(CDEP) $(HDEP)

$(OBJS) : $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	$(CXX) $(CFLAGS) $(SRCINC) -MM -MT $@ -MF $(patsubst %.o, %.d, $@) $<
	$(CXX) -c $(CFLAGS) $(SRCINC) $< -o $@

$(DB_OBJS) : $(OBJDIR)/%.o : $(DB_DIR)/%.cpp
	$(CXX) $(CFLAGS) $(SRCINC) -MM -MT $@ -MF $(patsubst %.o, %.d, $@) $<
	$(CXX) -c $(CFLAGS) $(SRCINC) $< -o $@

$(COMMON_OBJS) : $(OBJDIR)/%.o : $(COMMON_DIR)/%.cpp
	$(CXX) $(CFLAGS) $(SRCINC) -MM -MT $@ -MF $(patsubst %.o, %.d, $@) $<
	$(CXX) -c $(CFLAGS) $(SRCINC) $< -o $@

$(HANDLERS_OBJS) : $(OBJDIR)/%.o : $(HANDLERS_DIR)/%.cpp
	$(CXX) $(CFLAGS) $(SRCINC) -MM -MT $@ -MF $(patsubst %.o, %.d, $@) $<
	$(CXX) -c $(CFLAGS) $(SRCINC) $< -o $@

clean:
	rm -f $(OBJDIR)/*.o 
	rm -f *.d
	rm -f $(TARGET)

$(TARGET) : $(OBJS) $(DB_OBJS) $(COMMON_OBJS) $(HANDLERS_OBJS) $(SRC_LIB)
	/bin/mkdir -p $(BUILDDIR)/$(NAME)
	$(CXX) $(OBJS) $(DB_OBJS) $(COMMON_OBJS) $(HANDLERS_OBJS) -o $(TARGET) $(LIBDIR) $(LDFLAGS) 
