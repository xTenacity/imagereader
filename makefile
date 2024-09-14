all: output

output: main.cpp
	clear
	g++ -o output main.cpp `pkg-config --cflags --libs opencv4`

run: output
	clear
	./output

clean:
	rm -f output