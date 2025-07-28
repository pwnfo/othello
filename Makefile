CC = i686-w64-mingw32-gcc
CFLAGS = -Wall -c -s -mwindows -m32
LDFLAGS = -m32 -mwindows -lole32 -loleaut32 -luuid -lwbemuuid

C_SOURCE = $(wildcard *.c)
H_SOURCE = $(wildcard *.h)
OBJ = $(C_SOURCE:.c=.o)

all: othello.exe

othello.exe: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o othello.exe
