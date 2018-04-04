CC = g++
CC_FLAGS = -std=c++11 -O2 -static -Wall

LIBS = -lpthread -lz
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:%.cpp=%.o)

EXEC = WebServer

$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC) $(LIBS)
%.o: %.c
	    gcc -c $(CC_FLAGS) $< -o $@


clean:
	rm -f $(EXEC) $(OBJECTS)
