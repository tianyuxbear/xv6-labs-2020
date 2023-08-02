#include "../kernel/types.h"
#include "../kernel/param.h"
#include "./user.h"

void invoke(char *execargs[]);

int main(int argc, char *argv[])
{
	if (argc >= MAXARG - 1)
	{
		fprintf(2, "too many arguments\n");
		exit(1);
	}

	char *execargs[MAXARG] = {0};
	for (int i = 0; i < argc - 1; i++)
	{
		execargs[i] = argv[i + 1];
	}

	char line[512], ch;
	memset(line, 0, sizeof(line));
	int i = 0;
	while (read(0, &ch, 1) == 1)
	{
		if (ch == '\n')
		{
			line[i++] = '\0';
			execargs[argc - 1] = line;
			execargs[argc] = 0;
			invoke(execargs);
			memset(line, 0, sizeof(line));
			i = 0;
		}
		else
		{
			line[i++] = ch;
		}
	}
	if (strlen(line) != 0)
	{
		execargs[argc - 1] = line;
		execargs[argc] = 0;
		invoke(execargs);
		memset(line, 0, sizeof(line));
		i = 0;
	}
	exit(0);
}

void invoke(char *execargs[])
{

	if (fork() == 0)
	{
		exec(execargs[0], execargs);
	}
	else
	{
		wait(0);
	}
}