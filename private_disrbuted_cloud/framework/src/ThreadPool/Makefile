
# make clean/all CONFIG=Debug/Release ARCH=linux64/raspi ECHO=0/1

# the parrent dir of "arch" dir
ROOT_DIR = /home/daniel/git

#PROJ_SRCS = $(wildcard *.c)
PROJ_SRCS = ThreadPool.cpp ThreadPool_test.cpp

# list of directories containing header files
#PROJ_INC_DIR = $(ROOT_DIR)/include
PROJ_INC_DIR = ../../include ../../../concrete/include

# the executable file
PROJ_OUT = ThreadPool.out 

# list of ADDITIONAL compiler flags (prepended with "-" sign)
#PROJ_CFLAGS = -Wunused-but-set-variable
PROJ_CFLAGS =
PROJ_CPPFLAGS = -std=c++11

#PROJ_MACROS = _BSD_SOURCE
PROJ_MACROS =

# additional object files (created by other projects)
PROJ_ADD_OBJS = semaphore.obj

# additional libraries
PROJ_LIBS = 

# linker flags
PROJ_LDFLAGS =

# list of projects (makefiles) this project depends on
PROJ_DEPENDS  = ../semaphore/Makefile

include $(ROOT_DIR)/make/Project.mak

soft_link:
	ln -rs $(wildcard *.hpp) /home/daniel/git/projects/final_project/framework/include
.PHONY : soft_link 
