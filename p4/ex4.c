#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

void itoa(char line_str[], int line) {
	int tmp = line;
	int cnt = 0;
	while (tmp != 0) {
		tmp /= 10;
		cnt++;
	}
	
	line_str[cnt] = '\0';

	while (cnt != 0) {
		cnt--;
		line_str[cnt] = (char) (line%10 + 48);
		line = line/10;
	}
}

int return_digit(int line) {
	int cnt = 0;
	while (line != 0) {
		line /= 10;
		cnt++;
	}
	return cnt;
}

int main(int argc, char **argv) {
	
	if (argc == 1) {
		printf("Need more argument!");
		exit(1);
	}

	int fd;
	int fd_num;
	int retval;
	int line = 1;
	
	char c;
	char num = '1';
	char line_str[5];
	char format[] = " | ";
	char new_file_name[20];
	char new[] = "_num.txt";

	for (int i = 0; i < sizeof(argv[1]); i++) {
		new_file_name[i] = argv[1][i];
		if (argv[1][i] == '.') {
			for (int j = 0; j < sizeof(new); j++) {
				new_file_name[i+j] = new[j];
			}
			break;
		}
	}

	if ((fd = open(argv[1], O_RDONLY)) < 0 || (fd_num = open(new_file_name, O_RDWR | O_CREAT, 0755)) < 0) {
		perror("Open");
		exit(1);	
	}
	
	int digit = return_digit(line); 
	itoa(line_str, line);
	write(fd_num, line_str, 1);
	write(fd_num, format, 3);
	while (read(fd, &c, 1) != 0) {
		if (c == '\n') {
			line++;
			itoa(line_str, line);
			digit = return_digit(line);


			write(fd_num, "\n", 1);
			write(fd_num, line_str, digit);
			write(fd_num, format, 3);

			continue;
		}
		write(fd_num, &c, 1);
	}	

	if ((retval = close(fd)) < 0 || (retval = close(fd_num)) < 0) {
		perror("close");
		exit(1);
	}
	exit(0);
}
