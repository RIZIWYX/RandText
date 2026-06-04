MALLOCCP = 0
CC = gcc
CFLAGS = -std=c2x -Wall -Wconversion -Werror -Wextra -Wpedantic \
         -Wwrite-strings -O2 -DMALLOCCP=$(MALLOCCP) -DHOLDALL_PUT_TAIL

OPTION    = option
HASHTABLE = ../hashtable
HOLDALL   = ../holdall
MALLOCCP_DIR = ../malloccp

INCLUDES = -I. \
           -I$(OPTION) \
           -I$(HASHTABLE) \
           -I$(HOLDALL) \
           -I$(MALLOCCP_DIR)

DEFINE_H    = $(OPTION)/define.h
HELP_H      = $(OPTION)/help.h
OPTION_H    = $(OPTION)/option.h
OPTION_IP_H = $(OPTION)/option_ip.h
RANDTEXT_H  = randtext.h
HASHTABLE_H = $(HASHTABLE)/hashtable.h
HOLDALL_H   = $(HOLDALL)/holdall.h
MALLOCCP_H  = $(MALLOCCP_DIR)/malloccp.h

objects    = main.o randtext.o hashtable.o holdall.o malloccp.o option.o help.o
executable = randtext

.PHONY: all clean

all: $(executable)

$(executable): $(objects)
	$(CC) $(objects) -o $(executable)

main.o: main.c $(RANDTEXT_H) $(HASHTABLE_H) $(HOLDALL_H) $(MALLOCCP_H)
	$(CC) $(CFLAGS) -c main.c $(INCLUDES) -o $@

randtext.o: randtext.c $(RANDTEXT_H) $(HASHTABLE_H) $(HOLDALL_H) $(MALLOCCP_H)
	$(CC) $(CFLAGS) -c $< $(INCLUDES) -o $@

hashtable.o: $(HASHTABLE)/hashtable.c $(HASHTABLE_H) $(MALLOCCP_H)
	$(CC) $(CFLAGS) -c $< $(INCLUDES) -o $@

holdall.o: $(HOLDALL)/holdall.c $(HOLDALL_H) $(MALLOCCP_H)
	$(CC) $(CFLAGS) -c $< $(INCLUDES) -o $@

malloccp.o: $(MALLOCCP_DIR)/malloccp.c $(MALLOCCP_H)
	$(CC) $(CFLAGS) -c $< $(INCLUDES) -o $@

option.o: $(OPTION)/option.c $(DEFINE_H) $(OPTION_H) $(OPTION_IP_H) $(MALLOCCP_H)
	$(CC) $(CFLAGS) -c $< $(INCLUDES) -o $@

help.o: $(OPTION)/help.c $(DEFINE_H) $(HELP_H) $(OPTION_H) $(MALLOCCP_H)
	$(CC) $(CFLAGS) -c $< $(INCLUDES) -o $@

clean:
	$(RM) -f $(objects) $(executable)
