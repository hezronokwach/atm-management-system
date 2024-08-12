# Object files
objects = src/main.o src/system.o src/auth.o src/database.o

# Compiler flags
CFLAGS = -I/home/hokwach/sqlite/include
LDFLAGS = -L/home/hokwach/sqlite/lib -lsqlite3 

# Target to build the executable
atm: $(objects)
	cc -o atm $(objects) $(LDFLAGS)

# Compilation rules for object files
src/main.o: src/main.c src/header.h
	cc $(CFLAGS) -c -o src/main.o src/main.c

src/system.o: src/system.c src/header.h
	cc $(CFLAGS) -c -o src/system.o src/system.c

src/auth.o: src/auth.c src/header.h
	cc $(CFLAGS) -c -o src/auth.o src/auth.c

src/database.o: src/database.c src/header.h
	cc $(CFLAGS) -c -o src/database.o src/database.c

# Clean up build artifacts
clean:
	rm -f $(objects) atm
