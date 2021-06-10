MAIN = main

all: compile

compile:
	gcc -g -o $(MAIN) $(MAIN).c -lm --debug -Wall

run:
	./$(MAIN)

clean:
	rm $(MAIN)
