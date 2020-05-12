#ifndef cmdlib_h
#define cmdlib_h

#include <SDL2/SDL.h>

#define clamp(x,a,b)    (((x)>(b))?(b):(((x)<(a))?(a):(x)))
#define sign(a)         ((a) > 0 ? 1 : (a) < 0 ? -1 : 0)
#define max(a, b)   ((a) > (b) ? (a) : (b));

typedef unsigned char byte;

#ifdef _WIN32
    #define PATHSEPARATOR '\\'
#else
    #define PATHSEPARATOR '/'
#endif

extern int     myargc;
extern char ** myargv;

void error (char *error, ...);  // print an error message and terminate
int CheckParameter (char *check);     // check if parameters contains check
char *strupper (char *start);   // capitalize a string

//  "/path/to/filename.ext" becomes "filename"
void filebasename (char *path, char *dest, size_t size);

void SeedRandom (unsigned int seed);
uint32_t Random (void);

#endif /* cmdlib_h */
