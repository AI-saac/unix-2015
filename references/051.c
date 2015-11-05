/*
 * mypc.c	2010/11/05, tera
 *
 * syntax: "mycp file1 file2"
 */

#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>

//#define BUFSIZE		1
#define BUFSIZE		1024

int
main(int argc, char **argv)
{
	char buf[BUFSIZE], lbuf[80];
	int fd1, fd2;
	int cnt;

	if (argc != 3) {
		fprintf(stderr, "syntax: mycp file1 file2\n");
		exit(1);
	}

	if ((fd1 = open(argv[1], O_RDONLY)) < 0) {
		perror("open");
		exit(1);
	}

	if ((fd2 = open(argv[2], O_WRONLY|O_CREAT|O_EXCL, 0644)) < 0) {
		if (errno != EEXIST) {
			perror("open");
			close(fd1);
			exit(1);
		}
		fprintf(stderr, "overwrite ok (yes/no): ");
		if (fgets(lbuf, sizeof lbuf, stdin) == NULL) {
			if (ferror(stdin)) {
				perror("fgets");
				exit(1);
			}
			if (feof(stdin)) {
				fprintf(stderr, "stdin EOF\n");
				exit(1);
			}
		}
		if (*lbuf != 'y') {
			close(fd1);
			exit(0);
		}
		if ((fd2 = open(argv[2], O_WRONLY|O_CREAT|O_TRUNC, 0644)) < 0) {
			perror("open");
			close(fd1);
			exit(1);
		}
	}
	while ((cnt = read(fd1, buf, sizeof buf))) {
		if (cnt < 0) {
			perror("read");
			close(fd1);
			close(fd2);
			exit(1);
		}
		if (write(fd2, buf, cnt) < 0) {
			perror("write");
			close(fd1);
			close(fd2);
			exit(1);
		}
	}
	close(fd1);
	close(fd2);
	return 0;
}
