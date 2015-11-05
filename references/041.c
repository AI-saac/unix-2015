#include <stdio.h>
#include <string.h>

#define BUFLEN	256
#define NARGS	256

void getargs(int *, char *[], char *);

int
main()
{
	char lbuf[BUFLEN], *argv[NARGS];
	int argc, i;

	for (;;) {
		fprintf(stderr, "input a line: ");
		if (fgets(lbuf, sizeof lbuf, stdin) == NULL) {
			putchar('\n');
			return 0;
		}
		lbuf[strlen(lbuf) - 1] = '\0';	// replace '\n' with '\0'
		if (*lbuf == '\0')
			continue;

		getargs(&argc, argv, lbuf);

		printf("argc = %d\n", argc);
		for (i = 0; i < argc; i++)
			printf("argv[%d] = `%s' (len = %d)\n",
				i, argv[i], strlen(argv[i]));
	}
}

void
getargs(int *ac, char *av[], char *p)
{
	*ac = 0;
again:
	while (*p == ' ' || *p == '\t')		// skip spaces and tabs
		p++;
	if (*p == '\0')
		return;
	av[(*ac)++] = p;
	while (*p != ' ' && *p != '\t')		// move to next space or tab
		p++;
	if (*p == '\0')
		return;
	*p++ = '\0';				// terminate a string
	goto again;
}
