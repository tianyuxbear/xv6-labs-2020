#include "../kernel/types.h"
#include "./user.h"

int primes(int lpipe[2]);

int main()
{
	int p[2];
	pipe(p);

	for (int i = 2; i <= 35; i++)
	{
		int t = i;
		write(p[1], &t, 4);
	}
	if (fork() == 0)
	{
		primes(p);
	}
	else
	{
		close(p[0]);
		close(p[1]);
		wait(0);
		exit(0);
	}
	exit(0);
}

int primes(int lpipe[2])
{
	close(lpipe[1]);

	int rpipe[2];
	pipe(rpipe);

	int n = 0, t = 0;
	read(lpipe[0], &n, 4);
	printf("prime %d\n", n);
	while (read(lpipe[0], &t, 4) == 4)
	{
		if (t % n != 0)
		{
			write(rpipe[1], &t, 4);
		}
	}
	close(lpipe[0]);
	if (t == 0)
	{
		exit(0);
	}

	if (fork() == 0)
	{
		primes(rpipe);
	}
	else
	{
		close(rpipe[0]);
		close(rpipe[1]);
		wait(0);
		exit(0);
	}
	exit(0);
}