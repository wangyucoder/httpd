

ROOT=$(shell pwd)
LIB=$(ROOT)/../lib
LIB_PATH=-L $(LIB)/lib
INCLUDE= -I $(LIB)/include

LDFLAGS=-lmysqlclient

.PHONY:all
all:insert_cgi select_cgi

insert_cgi:insert_cgi.cpp sqlApi.cpp
	g++ -o $@ $^ $(INCLUDE) $(LIB_PATH) $(LDFLAGS)

select_cgi:select_cgi.cpp sqlApi.cpp 
	g++ -o $@ $^ $(INCLUDE) $(LIB_PATH) $(LDFLAGS)

.PHONY:output
output:
	cp insert_cgi ../output/wwwroot/cgi-bin -a
	cp select_cgi ../output/wwwroot/cgi-bin -a

.PHONY:clean
clean:
	rm -f select_cgi insert_cgi












