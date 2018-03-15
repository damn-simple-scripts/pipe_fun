#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define BUFFSIZE (16*1024)

int main(int argc, const char *argv[])
{
	size_t read_bytes = 0;

	if(argc != 2){
		printf("Usage: give me string to add linebreak before\n");
	}
	const char* search_string = argv[1];
	size_t search_string_len = strlen(search_string);
	char buff[search_string_len];
	char c;
	size_t cmp_pos = 0;

	for(;;){
		read_bytes = fread(&c, 1, 1, stdin);
		if(read_bytes == 0){
			if(feof(stdin)){
				return 0;
			}
		}else{
			if(c == search_string[cmp_pos]){
				buff[cmp_pos] = c;
				cmp_pos += 1;
				if(cmp_pos == search_string_len){
					printf("\n"), fflush(stdout);
					printf("%s", search_string);
					cmp_pos = 0;
				}
			}else{
				if(cmp_pos != 0){
					if(fwrite(buff, 1, cmp_pos, stdout) != cmp_pos){
						fprintf(stderr, "could not write whole buffer\n");
						return 1;
					}
					cmp_pos = 0;
				}
				printf("%c", c);
			}
		}
	}

	return 0;
}
