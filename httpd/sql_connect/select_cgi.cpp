
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include "sqlApi.h"

int main()
{
	char *method = getenv("METHOD");
	char *query_string = NULL;
	char buf[1024];
	if(method){
		if(strcasecmp(method, "GET") == 0){
			query_string = getenv("QUERY_STRING");
			strcpy(buf, query_string);
		}else{//POST
			int i = 0;
			int content_len = atoi(getenv("CONTENT_LENGTH"));
			for(; i < content_len; i++){
				read(0, &buf[i], 1);
			}
			buf[i] = 0;
		}
		query_string = buf;
    }
	sqlApi obj;
	obj.myConnect();
	obj.mySelect();

	return 0;
}








