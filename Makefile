MAIN = demaxsat

all: compile

compile:
	gcc -g -o $(MAIN) main.c -lm --debug -Wall

run:
	./$(MAIN)

clean:
	rm $(MAIN)
