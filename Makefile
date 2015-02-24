TARGET = timing
OBJ = $(TARGET).o
SRC = $(TARGET).cc
CXX := g++
CFLAGS = -lrt
LDFLAGS =

$(OBJ): $(SRC)
	$(CXX) $(CFLAGS) -c $^

clean:
	rm -f *.o *.~ $(TARGET)
