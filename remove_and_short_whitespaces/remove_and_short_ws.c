#include <ctype.h>
#include <stdio.h>

#define BUFFSIZE (16*1024)

int main()
{
	char buff[BUFFSIZE];
	char out_buff[BUFFSIZE];
	size_t read_bytes = 0;
	size_t out_buffer_filled = 0;
	size_t out_buff_cum = 0;
	size_t write_bytes = 0;
	char is_space = 0;
	char current;

	for(;;){
		read_bytes = fread(buff, 1, BUFFSIZE, stdin);
		if(read_bytes == 0){
			if(feof(stdin)){
				return 0;
			}
		}else{
			out_buffer_filled = 0;
			for(size_t i = 0; i < read_bytes; ++i){
				current = buff[i];
				if(isspace(current)){
					if(!is_space){
						out_buff[out_buffer_filled++] = ' ';
						is_space = 1;
					}
				}else{
					is_space = 0;
					out_buff[out_buffer_filled++] = current;
				}
			}
			write_bytes = fwrite(out_buff, 1, out_buffer_filled, stdout);
			if(write_bytes != out_buffer_filled){
				fprintf(stderr, "Could not write whole buffer to stdout!\n");
				return 1;
			}
			out_buff_cum += out_buffer_filled;
			if(out_buff_cum >= BUFFSIZE){
				out_buff_cum -= BUFFSIZE;
				fflush(stdout);
			}
		}
	}

	return 0;
}
