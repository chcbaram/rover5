# Source: http://stackoverflow.com/questions/9870297/makefile-to-compile-opencv-code-in-c-on-ubuntu-linux
# usage: make filename



TARGET = main


SRC = 	main.cpp	\
		Lib_Vision.cpp \
		./Main_Lib/Thread_Lib.c 	\
		./Main_Lib/KeyBrd_Lib.c



CFLAGS = `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv`


ALLSRC     =  $(SRC)
# List of all source files without directory and file-extension.
ALLSRCBASE = $(notdir $(basename $(ALLSRC)))

# Define all object files.
ALLOBJ = $(addsuffix .o, $(ALLSRCBASE))



MACHINE := $(shell uname -m)
ifeq ($(MACHINE), armv6l)
#USERCFLAGS = -O2 -pipe -mcpu=arm1176jzf-s -mfpu=vfp -mfloat-abi=hard
USERCFLAGS = -O2
endif

LDFLAGS = -lc  -lm 


all: build
#	g++ $(CFLAGS) $(USERCFLAGS) $(ALLOBJ) $< -o $@ $(LIBS)


build: $(ALLOBJ)
	g++ $(CFLAGS) $(USERCFLAGS) $(ALLOBJ) --output $(TARGET) $(LDFLAGS) $(LIBS)



# Compile: create object files from C source files.
define COMPILE_C_TEMPLATE
$(notdir $(basename $(1))).o : $(1)
	@echo $(MSG_COMPILING) $$< to $$@
	g++ -c $$(CFLAGS) $(USERCFLAGS) $$< -o $$@ $$(LIBS)
endef
$(foreach src, $(SRC), $(eval $(call COMPILE_C_TEMPLATE, $(src)))) 




clean: clean_list

clean_list :
	@echo $(MSG_CLEANING)
	rm -f $(TARGET)
	rm -f $(TARGET).sym
	rm -f $(TARGET).lss
	rm -f $(ALLOBJ)
	rm -f $(LSTFILES)
	rm -f $(DEPFILES)
