#include "../kernel/types.h"
#include "./user.h"

int main()
{
	int p[2];
	char buf[1];

	int ret = pipe(p);
	if (ret != 0)
	{
		fprintf(2, "pipe error!\n");
		exit(1);
	}

	if (fork() == 0)
	{
		read(p[0], buf, 1);
		printf("%d: received ping\n", getpid());
		write(p[1],"P",1);
		exit(0);
	}
	else
	{
		write(p[1],"P",1);
		wait(0);
		read(p[0], buf, 1);
		printf("%d: received pong\n", getpid());
		exit(0);
	}
}