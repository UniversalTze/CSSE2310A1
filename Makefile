all: uqwordiply 
CC=gcc 
CFLAGS= -Wall -pedantic -std=gnu99 -Werror -I/local/courses/csse2310/include


uqwordiply: uqwordiply.c dictionary.c dictionary.h 
	$(CC) $(CFLAGS) -o uqwordiply uqwordiply.c dictionary.c
	# $@ = name of target, $< = name of first prereq
	# line above same as
	# gcc -Wall -pedantic -std=gnu99 -o $@ $< 
	# Headers don't need to be apart of compilations lmao
 

clean: 
	rm uqwordiply
