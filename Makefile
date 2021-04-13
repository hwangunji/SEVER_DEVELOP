all: ser

ser.o: ser.c
        gcc -c ser.c

ser: ser.o
        gcc -o ser ser.o
clean:
        rm*.o
        rm ser




