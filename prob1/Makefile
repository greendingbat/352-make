mymake2: mymake2.o handleinput2.o mymake2.h
	gcc -Wall mymake2.o handleinput2.o -o mymake2
	
mymake2.o: mymake2.c
	gcc -Wall -c mymake2.c -o mymake2.o
	
handleinput2.o: handleinput2.c
	gcc -Wall -c handleinput2.c -o handleinput2.o

.PHONY: clean
clean:
	rm *.o
	rm mymake2