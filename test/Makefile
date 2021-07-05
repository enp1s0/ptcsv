CXX=g++
CXXFLAGS=-std=c++11 -I../include
TARGET=test_ptcsv

$(TARGET) : main.cpp ../include/ptcsv.hpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)

