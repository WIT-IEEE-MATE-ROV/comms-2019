#ifndef NUGGETAPI
#define NUGGETAPI
#include <stdbool.h>

enum TYPE {
    _int,
    _double,
    _string
};

typedef struct _PUBLISHER_ {
    int fd;
    enum TYPE t;
    char filepath[64];
} PUBLISHER;

typedef struct _LISTENER_ {
    int fd;
    enum TYPE t;
} LISTENER;

bool startapi();                        // Start the API by opening the contents of ./api/ as a series of named pipes. Call once at startup.
void shutdownapi();                     // Shut the API down
LISTENER  get_listener(char* path, enum TYPE);     // Get a file descriptor that points to the named pipe.
PUBLISHER get_publisher(char* path, enum TYPE);    // Get a file descriptor that points to the named pipe.

int  get_int(LISTENER);                 // Get an integer from the LISTENER
bool post_int(PUBLISHER, int);          // Post an integer to the PUBLISHER

double get_double(LISTENER);            // Get a double from the LISTENER
bool   post_double(PUBLISHER, double);  // Post a double to the PUBLISHER

char* get_string(LISTENER);             // Get a string from the LISTENER
bool  post_string(PUBLISHER, char*);    // Post a string to the PUBLISHER

#endif
