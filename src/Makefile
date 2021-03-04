GCC = gcc
CFLAG = -o
RM = rm -f

default: clean car_rental

car_rental:
	$(GCC) $(CFLAG) ../bin/car_rental car_rental.c simlib.c

clean:
	$(RM) ../bin/car_rental
