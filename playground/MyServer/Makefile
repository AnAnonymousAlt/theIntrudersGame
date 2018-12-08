CXX = g++ -pthread -g -fPIC 
NETLIBS= -lnsl

all: myhttpd daytime-server use-dlopen hello.so jj-mod

daytime-server : daytime-server.o
	$(CXX) -o $@ $@.o $(NETLIBS)

myhttpd : myhttpd.o dir_functions.o slaves.o tools.o mywebsocket.o
	$(CXX)  -o $@ $@.o dir_functions.o slaves.o tools.o mywebsocket.o $(NETLIBS) -ldl -lrt

use-dlopen: use-dlopen.o
	$(CXX) -o $@ $@.o $(NETLIBS) -ldl

hello.so: hello.o
	ld -G -o hello.so hello.o

jj-mod: jj-mod.o util.o
	gcc -shared -o jj-mod.so jj-mod.o util.o

jj-mod.o: 
	gcc -c -g -fPIC jj-mod.c -o jj-mod.o

util.o:
	gcc -c -g -fPIC util.c -o util.o

%.o: %.cc
	@echo 'Building $@ from $<' 
	$(CXX) -o $@ -c -I. $< 

.PHONY: git-commit
git-commit:
	git checkout
	git add *.cc *.h Makefile >> .local.git.out  || echo
	git commit -a -m 'Commit' >> .local.git.out || echo
	git push origin master 

.PHONY: clean
clean:
	rm -f *.o use-dlopen hello.so jj-mod.so
	rm -f *.o daytime-server myhttpd dir_functions slaves tools

