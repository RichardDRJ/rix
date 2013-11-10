SOURCES	:=	$(wildcard *.cpp)
OBJ		:=	$(SOURCES:%.cpp=%.o)

enigma: $(OBJ)
	g++ -g -o enigma $(OBJ)

$(OBJ): $(SOURCES)
	g++ -g -c $(SOURCES)

clean:
	rm -rf enigma *.o

.phony: clean
