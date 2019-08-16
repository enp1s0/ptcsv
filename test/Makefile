CXX=g++
CXXFLAGS=-std=c++11 -I../
TARGET=test_ptcsv

$(TARGET) : main.cpp ../ptcsv.hpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)

