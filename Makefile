
# make clean/all CONFIG=Debug/Release ARCH=linux64/raspi ECHO=0/1

# the parrent dir of "arch" dir
ROOT_DIR = /home/daniel/git

#PROJ_SRCS = $(wildcard *.c)
PROJ_SRCS = iot_preliminary.cpp

# list of directories containing header files
#PROJ_INC_DIR = $(ROOT_DIR)/include
PROJ_INC_DIR = ../../include ../../../framework/include

# the executable file
PROJ_OUT = iot_preliminary.out 

# list of ADDITIONAL compiler flags (prepended with "-" sign)
#PROJ_CFLAGS = -Wunused-but-set-variable
PROJ_CFLAGS =
PROJ_CPPFLAGS = -std=c++11

#PROJ_MACROS = _BSD_SOURCE
PROJ_MACROS =

# additional object files (created by other projects)
PROJ_ADD_OBJS = select.obj function.obj nbd_comma.o

# additional libraries
PROJ_LIBS = 

# linker flags
PROJ_LDFLAGS =

# list of projects (makefiles) this project depends on
PROJ_DEPENDS  = ../../../framework/src/reactor/Makefile ../buse/Makefile ../../../framework/src/function/Makefile

include $(ROOT_DIR)/make/Project.mak

soft_link:
	ln -rs $(wildcard *.hpp) /home/daniel/git/projects/final_project/concrete/include
.PHONY : soft_link 
