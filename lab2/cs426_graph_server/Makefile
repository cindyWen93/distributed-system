#--------------------------
# Macro definitions
	CXXFLAGS = -O1 -g -Wall -std=c++11
	OBJ = main.o mongoose.o graph.o disk.o
	TARGET = cs426_graph_server
#--------------------------
# Rules
all:$(TARGET)
$(TARGET):$(OBJ)
	$(CXX) -o $@ $(OBJ)
clean:
	rm -f $(OBJ) $(TARGET)
#---------------------------
#Dependencies
main.o: main.cpp mongoose.h graph.h struct.hpp disk.hpp
mongoose.o: mongoose.c mongoose.h
graph.o: graph.cpp graph.h
disk.o: disk.cpp disk.hpp struct.hpp
