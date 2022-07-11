MINGW1_PREFIX=i586-mingw32msvc-
MINGW2_PREFIX=i686-w64-mingw32-
MINGW1=$(shell which $(MINGW1_PREFIX)cc 2> /dev/null)
MINGW2=$(shell which $(MINGW2_PREFIX)gcc 2> /dev/null)
ifneq ("$(MINGW1)","")
	CC=$(MINGW1_PREFIX)cc
else ifneq ("$(MINGW2)","")
	CC=$(MINGW2_PREFIX)gcc
else
$(error MinGW not found)
endif
OUT_EXE			= mem_track_test.exe
OBJS			= main.o
RM			= rm -f

.PHONY : all
all : $(OUT_EXE)

.PHONY : clean
clean :
	$(RM) $(OBJS) $(OUT_EXE)

$(OUT_EXE) : $(OBJS) ../common/call_vmm.c ../common/call_vmm.h
	$(CC) -o $(OUT_EXE) $(OBJS)  ../common/call_vmm.c

main.o : main.c ../common/call_vmm.h
	$(CC) -Wall -c -o main.o main.c

