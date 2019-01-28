#include <fcntl.h>
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <unistd.h>

#include "nugget-api.h"
#define _XOPEN_SOURCE 500 // nftw likes having this around but can live without it

bool API_IS_RUNNING = false;

static int open_as_named_pipe(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf)
{
    if(tflag == FTW_F)		// Look only at _F_iles, not directories
        mkfifo(fpath, 666); // Open named pipe with read/write permissions
    return 0;               /* To tell nftw() to continue */
}

bool startapi(void)
{
	if(API_IS_RUNNING) {
		perror("API is already running."); //TODO: replace with roverr
		return false;
	}
	API_IS_RUNNING = true;

    // TODO- make relative to allow for universal usage
	char* API_PATH = "/home/chris/Projects/rov2019/comms/api/";
    chdir(API_PATH);
    int flags = 0;

	// So a fun fact about this file is that it assumes nftw hasn't been declared-- it's
	// in ftw.h, so it's totally there and usable.
    if (nftw(API_PATH, open_as_named_pipe, 20, flags) == -1) {
        perror("nftw"); //TODO: Replace with roverr stuff
        exit(EXIT_FAILURE);
    }
	return true;
}

void shutdownapi(void)
{
	// Get the max number of file descriptors
	struct rlimit rlim;
	getrlimit(RLIMIT_NOFILE, &rlim);
	// Start at first created file descriptor and close from there
	for(int i = 2; i < rlim.rlim_cur; ++i) close(i);
}

LISTENER get_listener(char* path, enum TYPE t)
{
    LISTENER l;
    l.fd = open(path, O_RDONLY);
    l.t  = t;
    return l;
}

PUBLISHER get_publisher(char* path, enum TYPE t)
{
    PUBLISHER p;
    p.fd = open(path, O_WRONLY);
    p.t = t;
    return p;
}

int get_int(LISTENER l)
{
    return 0;
}

bool post_int(PUBLISHER p, int content)
{
    return false;
}

double get_double(LISTENER l)
{
    return .0;
}

bool post_double(PUBLISHER p, double content)
{
    return false;
}

char* get_string(LISTENER l)
{
    return "";
}

bool post_string(PUBLISHER p, char* content)
{
    return false;
}

