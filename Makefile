OUT = $(shell basename $(PWD))

build: .gitignore
	cc main.c -g -o $(OUT)

.gitignore:
	echo $(OUT) > .gitignore
	echo .gitignore >> .gitignore

clean:
	rm $(OUT)
