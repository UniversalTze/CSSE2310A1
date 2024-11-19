all: uqwordiply 
CC=gcc 
CFLAGS= -Wall -pedantic -std=gnu99 -Werror -I/local/courses/csse2310/include -L/local/courses/csse2310/lib -lcsse2310a1


uqwordiply: uqwordiply.c dictionary.c dictionary.h 
	$(CC) $(CFLAGS) -o uqwordiply uqwordiply.c dictionary.c dictionary.h
	# $@ = name of target, $< = name of first prereq
	# line above same as
	# gcc -Wall -pedantic -std=gnu99 -o $@ $< 
 

clean: 
	rm uqwordiply
