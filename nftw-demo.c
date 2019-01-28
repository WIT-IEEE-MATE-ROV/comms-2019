#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

static int
get_file(const char *fpath, const struct stat *sb,
             int tflag, struct FTW *ftwbuf)
{
    if(tflag == FTW_F)
        printf("%s\n", fpath);
    return 0;           /* To tell nftw() to continue */
}

int
main(int argc, char *argv[])
{
    int flags = 0;
    char *path = "/home/chris/Projects/rov2019/comms/api/";

    if (
            nftw(path, get_file, 20, flags)
            == -1) {
        perror("nftw");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
