# Makefile для компиляции и тестирования
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -g
TARGET = a.out
SOURCES = device.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET) *.o *.out *.gcda *.gcno

test: $(TARGET)
	./$(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

rebuild: clean all

.PHONY: all clean test valgrind rebuild