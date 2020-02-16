OUT = $(shell basename $(PWD))

build:
	cc main.c -o $(OUT)

clean:
	rm $(OUT)
