#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

int main(void) {
	unsigned int pid;
	unsigned int child_pid;
	time_t t;
	struct tm tm;
	int fd;
	char *argv[3];
	char buf[512];
	char *str;
	int fd0, fd1, fd2;
	int status;
	int minute = -1;
	int hour = -1;

	fd = open("./crontab", O_RDWR);
	pid = fork();
	if(pid == -1) return -1;
	if(pid != 0)
		exit(0);
	if(setsid() < 0)
		exit(0);
	if(chdir("/") < 0)
		exit(0);
	umask(0);
	close(0);
	close(1);
	close(2);

	fd0 = open("/dev/null", O_RDWR);
	fd1 = open("/dev/null", O_RDWR);
	fd2 = open("/dev/null", O_RDWR);
	setlogmask(LOG_UPTO(LOG_DEBUG));
	openlog("mycron", LOG_PID, LOG_LPR);
	syslog(LOG_DEBUG, "now running");
	t = time(NULL);
	localtime_r(&t, &tm);

	buf[0] = '\0';
	if (read(fd, buf, sizeof(buf)) != 0) {
		int stat;
		char *pos;
		int i = 0;
		str = strtok_r(buf, " ", &pos);
		while (str != NULL) {
			argv[i++] = str;
			str = strtok_r(NULL, " ", &pos);
		}

		minute = atoi(argv[0]);
		hour = atoi(argv[1]);
		if (argv[0][0] == '*') minute = -1;
		if (argv[1][0] == '*') hour = -1;

		syslog(LOG_DEBUG, "%d %d", minute, hour);
	}

	while (1)
	{
		if ((minute == tm.tm_min || minute == -1) && (hour == tm.tm_hour || hour == -1)) {
			if ((child_pid = fork()) == 0) {
				waitpid(child_pid, NULL, WNOHANG);
			}
			else {
				syslog(LOG_DEBUG, "%s", argv[2]);
				execl("/bin/sh", "/bin/sh", "-c", argv[2], (char *) NULL);
				syslog(LOG_DEBUG, "will not show");
			}
		}
		syslog(LOG_DEBUG, "waiting");
		t = time(NULL);
		localtime_r(&t, &tm);
		sleep(60 - tm.tm_sec % 60);
	}

	syslog(LOG_DEBUG, "END");
	return 0;
}
