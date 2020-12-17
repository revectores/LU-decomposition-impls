OBJECT = test/test_lu
# IMPL = lu_seq
IMPL = lu_fine_grain_condvar
LU_PATH = impl/${IMPL}
CC = gcc -I $(LU_PATH) -I util -I timing



test/test_lu: test/test_lu.o $(LU_PATH)/lu.o util/util.o timing/timing.o
	$(CC) -o test/test_lu test/test_lu.o $(LU_PATH)/lu.o util/util.o timing/timing.o

test/test_lu.o: test/test_lu.c
	$(CC) -o test/test_lu.o -c test/test_lu.c

$(LU_PATH)/lu.o: $(LU_PATH)/lu.c
	$(CC) -o $(LU_PATH)/lu.o -c $(LU_PATH)/lu.c

util/util.o: util/util.c
	$(CC) -o util/util.o -c util/util.c

timing/timing.o: timing/timing.c
	$(CC) -o timing/timing.o -c timing/timing.c

clean:
	find . -name "*.o" -type f -delete
	find . -name "*.so" -type f -delete
	rm $(OBJECT)
