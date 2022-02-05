sysiproject4:	main.o symbols.o directive.o headers.h 
	gcc -o project4 -Wall -O0 main.o symbols.o directive.o

main.o:	main.c headers.h 
	gcc -c -Wall -O0 -g main.c

symbols.o:	symbols.c headers.h
	gcc -c -Wall -O0 -g symbols.c

directive.o:	directive.c headers.h
	gcc -c -Wall -O0 -g directive.c




clean:	
	rm *.o -f
	touch *.c
	rm project4 -f
