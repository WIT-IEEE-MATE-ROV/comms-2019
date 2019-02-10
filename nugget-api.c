#include <fcntl.h>
#include <ftw.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <unistd.h>
#ifndef NUGGETAPI_SOURCE
#define NUGGETAPI_SOURCE

#include "nugget-api.h"
#define _XOPEN_SOURCE 500 // nftw likes having this around but can live without it
#define BUFFSIZE 512

char* API_PATH = "/home/chris/Projects/rov2019/comms/api/";
bool API_IS_RUNNING = false;

#pragma GCC diagnostic ignored "-Wunused-parameter"
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
    chdir(API_PATH);
    int flags = 0;

	// So a fun fact about this file is that it assumes nftw hasn't been declared-- it's
	// in ftw.h, so it's totally there and usable.
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
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
	for(uint i = 2; i < rlim.rlim_cur; ++i) close(i);
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
    strcpy(p.filepath, path);
    p.t = t;
    return p;
}

void clear_file_content(char *filepath)
{
	chdir(API_PATH);
	FILE *fp;
	fp = fopen(filepath, "w");
	fclose(fp);
}
int get_int(LISTENER l)
{
    char buff[BUFFSIZE];
    int amountread = read(l.fd, buff, BUFFSIZE);
    buff[amountread+1] = '\0'; // Make sure we're null terminated
    return atoi(buff);
}

bool post_int(PUBLISHER p, int content)
{
    clear_file_content(p.filepath);
    int digitcount = (int) ceil(log10(content));
    char buff[digitcount];
    sprintf(buff, "%i", content);
    write(p.fd, buff, digitcount);

    return true; // TODO: return false on fail
}

double get_double(LISTENER l)
{
    return (1.0*get_int(l))/1000;
}

bool post_double(PUBLISHER p, double content)
{
    clear_file_content(p.filepath);
    int icontent = (int) (content * 1000);
    return post_int(p, icontent);
}

char* get_string(LISTENER l)
{
    static char buff[BUFFSIZE];
    int amountread = read(l.fd, buff, BUFFSIZE);
    buff[amountread+1] = '\0';
    return buff;
}

bool post_string(PUBLISHER p, char* content)
{
    clear_file_content(p.filepath);
    write(p.fd, content, strlen(content));
    return true; // TODO: return false on fail
}

#endif
