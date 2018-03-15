#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define BUFFSIZE (16*1024)

int main()
{
	size_t read_bytes;
	char c;
	char last_break = 0;

	printf("\"");
	for(;;){
		read_bytes = fread(&c, 1, 1, stdin);
		if(read_bytes == 0){
			if(feof(stdin)){
				break;
			}
		}else{
			if(c == '\n'){
				if(!last_break){
					printf("\"\n"), fflush(stdout);
					last_break = 1;
				}
			}else{
				if(last_break){
					printf("\"");	
					last_break = 0;
				}
				printf("%c", tolower(c));
			}
		}
	}
	if(!last_break){
		printf("\"\n");
	}

	return 0;
}
