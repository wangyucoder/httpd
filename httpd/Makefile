
ROOT=$(shell pwd)

CC=gcc
FLAGS=#-D_STDOUT_
LDFLAGS=-lpthread
cgi_path=$(ROOT)/wwwroot/cgi-bin $(ROOT)/sql_connect

bin=httpd
src=$(shell ls *.c)

.PHONY:all
all: $(bin) cgi

$(bin):$(src)
	$(CC) -o $@ $^ $(FLAGS) $(LDFLAGS) 

.PHONY:cgi
cgi:
	for i in `echo $(cgi_path)`;\
	do\
		cd $$i;\
		make;\
		cd -;\
	done

.PHONY:output
output:
	mkdir -p output/wwwroot/cgi-bin
	cp wwwroot/index.html output/wwwroot/ -a
	cp wwwroot/imag output/wwwroot/ -a
	cp conf output -a
	cp log output -a
	cp $(bin) output
	cp plugin/server_ctl.sh output/
	cp lib output/ -a
	for i in `echo $(cgi_path)`;\
	do\
		cd $$i;\
		make output;\
		cd -;\
	done

.PHONY:clean
clean:
	rm -rf $(bin) output
	for i in `echo $(cgi_path)`;\
	do\
		cd $$i;\
		make clean;\
		cd -;\
	done

.PHONY:debug
debug:
	echo $(src)












