#***************************************************************************
#*       Copyright (c) 2010 Aces Technology Co., Ltd.
#*	 Lee Sung Ho
#***************************************************************************


EXE = aviparser


ALL_OBJS = \
	avidec.o	\
	aviutil.o		\
	file.o \
	main-test.o	\
	util.o	


all: makeall


COMMON_INCLUDE = ./ 
		
makeall: $(EXE)

$(EXE):	$(ALL_OBJS)
	gcc -g -o aviparser $(ALL_OBJS) -lpthread

avidec.o:	avidec.c \
	$(COMMON_INCLUDE)
	gcc -c -I../inc/	avidec.c

aviutil.o:	aviutil.c \
	$(COMMON_INCLUDE)
	gcc -c -g -I../inc/	aviutil.c

file.o:	file.c \
	$(COMMON_INCLUDE)
	gcc -c -g -I../inc/	file.c

main-test.o:	main-test.c \
	$(COMMON_INCLUDE)
	gcc -c -g -I../inc/	main-test.c

util.o:	util.c \
	$(COMMON_INCLUDE)
	gcc -c -g -I../inc/	util.c

clean:
	rm -f $(ALL_OBJS)
	rm -f $(EXE)
