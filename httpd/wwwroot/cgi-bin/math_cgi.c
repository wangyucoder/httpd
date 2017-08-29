#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>

void mymath(char *query_string)
{
	//firstdata=0&lastdata=0
	char *argv[3];
	int i = 0;
	char *start = query_string;
	while(*start){
		if(*start == '='){
			argv[i++] = start+1;
		}
		if(*start == '&'){
			*start = '\0';
		}
		start++;
	}
	argv[i] = NULL;
	int fdata = atoi(argv[0]);
	int ldata = atoi(argv[1]);
	printf("<html>");
	printf("<h2>%d + %d = %d</h2><br>\n", fdata, ldata, fdata+ldata);
	printf("<h2>%d - %d = %d</h2><br>\n", fdata, ldata, fdata-ldata);
	printf("<h2>%d * %d = %d</h2><br>\n", fdata, ldata, fdata*ldata);
	printf("<h2>%d / %d = %d</h2><br>\n", fdata, ldata, ldata==0?-1:fdata/ldata);
	printf("<h2>%d %% %d = %d</h2><br>\n", fdata, ldata, fdata%ldata);
	printf("</html>");
}

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
	if(query_string){
		mymath(query_string);
	}

	return 0;
}



