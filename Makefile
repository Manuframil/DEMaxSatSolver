MAIN = main
GWSAT = gwsatMain

all: compile

compile:
	gcc -g -o $(MAIN) $(MAIN).c -lm --debug -Wall

gwsat:
	gcc -g -o $(GWSAT) $(GWSAT).c -lm --debug -Wall

run:
	./$(MAIN)

clean:
	rm $(MAIN)
