#include "../kernel/types.h"
#include "../kernel/stat.h"
#include "./user.h"
#include "../kernel/fs.h"

void recur_trav(const char *path, const char *filename);

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		fprintf(2, "usage: find <path> <filename> \n");
		exit(1);
	}
	char *path = argv[1], *filename = argv[2];

	recur_trav(path, filename);

	exit(0);
}

void recur_trav(const char *path, const char *filename)
{
	int fd;
	char buf[512], *p;
	struct stat st;
	struct dirent de;
	fd = open(path, 0);
	while (read(fd, &de, sizeof(de)) == sizeof(de))
	{
		if (de.inum == 0)
			continue;
		memset(buf, 0, sizeof buf);
		strcpy(buf, path);
		p = buf + strlen(buf);
		*p++ = '/';
		memmove(p, de.name, DIRSIZ);
		p[DIRSIZ] = 0;
		stat(buf, &st);
		if (st.type == T_DIR)
		{
			if (strcmp(de.name, ".") != 0 && strcmp(de.name, "..") != 0)
			{
				recur_trav(buf, filename);
			}
		}
		else
		{
			if (strcmp(de.name, filename) == 0)
			{
				printf("%s\n", buf);
			}
		}
	}
}