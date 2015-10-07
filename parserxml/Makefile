
include ../../../MakeFile_Public

ARFLAGS = cr
CPPFLAGS= -ggdb3 -Wall -Werror $(INC) -fPIC 

TARGET	= $(TARGET_DIR)/librapidxml.a

OBJ_DIR	= .objs/

TARGET_DIR = ../../../Lib/

INC	= -I. -I.. -I../../include -I../../../include \
      -I../../../../JYGF/src
      
VPATH	= .:$(SERVER_DIR)

SRC = $(wildcard *.cpp)
OBJS = $(addprefix $(OBJ_DIR)/,$(subst .cpp,.o,$(SRC)))


all : $(TARGET)

$(TARGET) : $(OBJS)
	@if [ ! -e $(TARGET_DIR) ]; then \
		mkdir -p $(TARGET_DIR); \
	fi
	$(AR) $(ARFLAGS) $@ $?

$(OBJ_DIR)/%.o : %.cpp
	@if [ ! -e $(OBJ_DIR) ]; then \
		mkdir $(OBJ_DIR); \
	fi
	$(CXX) $(CPPFLAGS) -c $< -o $@

clean: 
	rm -f $(OBJS) $(TARGET)
