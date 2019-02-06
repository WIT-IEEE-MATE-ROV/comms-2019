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

#pragma GCC diagnostic ignored "-Wunused-parameter"
static int close_named_pipe(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf)
{
    if(tflag == FTW_F)
        unlink(fpath);
    return 0;
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

    int flags = 0;
    if(nftw(API_PATH, close_named_pipe, 20, flags) == -1) {
        perror("nftw on close");
        exit(EXIT_FAILURE);
    }
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
    char buff[BUFFSIZE];
    int amountread = read(l.fd, buff, BUFFSIZE);
    buff[amountread+1] = '\0'; // Make sure we're null terminated
    return atoi(buff);
}

bool post_int(PUBLISHER p, int content)
{
    int digitcount = (int) ceil(log10(content));
    char buff[digitcount];
    sprintf(buff, "%i", content);
    write(p.fd, buff, digitcount);

    return true; // TODO: return false on fail
}

double get_double(LISTENER l)
{
//    char buff[BUFFSIZE];
//    int amountread = read(l.fd, buff, BUFFSIZE-1);
//    
//    int readval;
//    buff[amountread+1] = '\0'; // Make sure we're null terminated
//    readval = atoi(buff);
//    return readval/1000;
//
    char buff[BUFFSIZE];
    int amountread = read(l.fd, buff, BUFFSIZE);
    buff[amountread+1] = '\0'; // Make sure we're null terminated
    return atoi(buff)/1000;

}

bool post_double(PUBLISHER p, double content)
{
//    char buff[BUFFSIZE];
//    sprintf(buff, "%lf", content*1000);
//    write(p.fd, buff, strlen(buff));
//
//    return true; // TODO: RETURN false on fail
//
    int icontent = (int) content * 1000;
    //printf("%i", icontent);
    int digitcount = (int) ceil(log10(icontent));
    char buff[digitcount];
    sprintf(buff, "%i", icontent);
    write(p.fd, buff, digitcount);

    return true; // TODO: return false on fail
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
    write(p.fd, content, strlen(content));
    return true; // TODOL return false on fail
}

