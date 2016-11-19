#--------------------------
# Macro definitions
	CXXFLAGS = -O1 -g -Wall -std=c++14
	OBJ = main.o mongoose.o graph.o
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
main.o: main.cpp mongoose.h graph.h
mongoose.o: mongoose.c mongoose.h
graph.o: graph.cpp graph.h
