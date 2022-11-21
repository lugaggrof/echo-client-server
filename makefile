CC=gcc
TARGET=echo-server echo-client

all: $(TARGET)

echo-server:
	$(CC) -pthread echo-server.c -o echo-server

echo-client:
	$(CC) -pthread echo-client.c -o echo-client

clean:
	rm -f $(TARGET) *.o
