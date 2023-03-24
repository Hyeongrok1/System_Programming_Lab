#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

//change integer to string
void itoa(char line_str[], int line_num) {
	int tmp = line_num;
	int cnt = 0;
	while (tmp != 0) {
		tmp /= 10;
		cnt++;
	}
	
	line_str[cnt] = '\0';

	while (cnt != 0) {
		cnt--;
		line_str[cnt] = (char) (line_num%10 + 48);
		line_num = line_num/10;
	}
}

//return the digit number of the line
int return_digit(int line) {
	int cnt = 0;
	while (line != 0) {
		line /= 10;
		cnt++;
	}
	return cnt;
}

int main(int argc, char **argv) {
	
	if (argc > 2) {
		write(STDERR_FILENO, "Too many arguments\n", 20);
		exit(1);
	}
	else if (argc < 2) {
		write(STDERR_FILENO, "Need more argument\n", 20);
		exit(1);
	}

	int fd_read;
	int fd_write;
	int retval;
	int line_num = 1;
	int digit;
	
	char c;
	char line_str[5];
	char format[] = " | ";
	char new_file_name[20];
	char new[] = "_num.txt";
	
	//add \"_num.txt\" to file name
	for (int i = 0; i < sizeof(argv[1]); i++) {
		new_file_name[i] = argv[1][i];
		if (argv[1][i] == '.' || argv[1][i] == '\0') {
			for (int j = 0; j < sizeof(new); j++) {
				new_file_name[i+j] = new[j];
			}
			break;
		}
	}

	if ((fd_read = open(argv[1], O_RDONLY)) < 0 || (fd_write = open(new_file_name, O_RDWR | O_CREAT, 0755)) < 0) {
		perror("Open");
		exit(1);	
	}
	
	digit = return_digit(line_num); 

	itoa(line_str, line_num);
	write(fd_write, line_str, 1);
	write(fd_write, format, 3);

	while (read(fd_read, &c, 1) != 0) {
		//check if it is '\n' and not the end of the file
		if (c == '\n' && read(fd_read, &c, 1) != 0) {
			line_num++;
			itoa(line_str, line_num);
			digit = return_digit(line_num);

			write(fd_write, "\n", 1);
			write(fd_write, line_str, digit);
			write(fd_write, format, 3);
			
			lseek(fd_read, -1, SEEK_CUR);

			continue;
		}
		write(fd_write, &c, 1);
	}

	if ((retval = close(fd_read)) < 0 || (retval = close(fd_write)) < 0) {
		perror("close");
		exit(1);
	}
	exit(0);
}
