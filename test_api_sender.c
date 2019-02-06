#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "nugget-api.h"

int main(void)
{
    startapi();

    srand(time(NULL));
    int randint = rand();
    double randdouble = (1.0*rand())/1000;

    char buff[10];
    for(int n = 0; n < 9; ++n) {
        char randchar = (rand() % 25) + 'a';
        buff[n] = randchar;
    }
    buff[9] = '\0';

    PUBLISHER p_int = get_publisher("testing/int", _int);
    PUBLISHER p_double = get_publisher("testing/double", _double);
    PUBLISHER p_string = get_publisher("testing/string", _string);

    printf("Sending integer: %i\n", randint);
    bool status = post_int(p_int, randint);
    if(!status) printf("post int failed");

    printf("Sending double: %lf\n", randdouble);
    status = post_double(p_double, randdouble);
    if(!status) printf("post double failed");

    printf("Sending string %s\n", buff);
    status = post_string(p_string, buff);
    if(!status) printf("post string failed");

    printf("\nNow attempting to read from API.\n");

    LISTENER l_int = get_listener("testing/int", _int);
    LISTENER l_double = get_listener("testing/double", _double);
    LISTENER l_string = get_listener("testing/string", _string);

    int gotint = get_int(l_int);
    double gotdouble = get_double(l_double);
    char gotstring[10];
    strcpy(gotstring, get_string(l_string));

    printf("Got %i, %lf, %s\n", gotint, gotdouble, gotstring);

    shutdownapi();
    return 0;
    
}
