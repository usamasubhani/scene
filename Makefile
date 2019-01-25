LIBS = -lGL -lGLU -lglut

project2: Source.cpp
	g++ Source.cpp -o out $(LIBS)

clean:
	rm -f out
