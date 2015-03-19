#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/sendfile.h>

#define D(str) do { fprintf(stderr, "efcr: " str "\n"); } while(0)

int in_fd = -1;

void __attribute__((noreturn)) usage(int ret)
{
	fprintf(stderr, "usage: efcr evil-content replace-this\n");
	exit(ret);
}

void open_evil_file(char *path)
{
	in_fd = open(path, O_RDONLY);
	assert(in_fd != -1);
	D("evil file opened");
}

int check_for_close(char *path)
{
	D("file was just closed!");
	return 1;
}

void replace_evil_content(char *path)
{
	int fd;
	struct stat st;

	D("Replacing contents...");
	fd = open(path, O_WRONLY | O_TRUNC);
	assert(fd != -1);
	assert(fstat(in_fd, &st) != -1);
	lseek(in_fd, 0, SEEK_SET);
	sendfile(fd, in_fd, NULL, st.st_size);
	close(fd);
	D("... done.");
}

int main(int argc, char *argv[])
{
	if(argc != 3)
		usage(-1);
	open_evil_file(argv[1]);
	while(check_for_close(argv[2]))
		replace_evil_content(argv[2]);
	return EXIT_SUCCESS;
}
