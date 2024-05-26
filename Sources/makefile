#
# Makefile 
#

.c.o:
	@Echo "*e[33;1m  Lc $(CFLAGS) TempCFile.c*e[0m"
#	lc $(CFLAGS) TempCFile.c

OBJS = df1:Objects/AssRoutine.o TempCFile.o

TempCFile: $(OBJS)
	Blink LIB:c.o $(OBJS) TO TempCFile LIB OBJ:Puzzland.lib LIB:lc.lib LIB:lcm.lib LIB:amiga.lib
