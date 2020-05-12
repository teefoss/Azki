#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "cmdlib.h"

#define CMWC_CYCLE 4096
#define CMWC_C_MAX 809430660

static uint32_t Q[CMWC_CYCLE];
static uint32_t c;
static unsigned ri;

int     myargc;
char ** myargv;

void error (char *message, ...)
{
    va_list argptr;

    va_start(argptr, message);
    vprintf(message, argptr);
    va_end(argptr);
    printf("\n");
    exit(1);
}

int CheckParameter (char *p)
{
    int i;
    
    for (i=0 ; i<myargc ; i++) {
        if (!strcmp(p, myargv[i]))
            return i;
    }
    return 0;
}

char *strupper (char *start)
{
    char *in;
    in = start;
    while (*in)
    {
        *in = toupper(*in);
        in++;
    }
    return start;
}

void filebasename (char *path, char *dest, size_t size)
{
    char * src;
    int length;

    src = path + strlen(path) - 1;

//  back up until a \ or the start
    while (src != path && *(src-1) != PATHSEPARATOR)
        src--;

//  copy up to eight characters
    memset(dest, 0, size);
    length = 0;
    while (*src && *src != '.')
    {
        if (++length == size + 1)
            Quit ("Filename base of %s > %zu chars", path, size);
        *dest++ = toupper(*src++);
    }
}

// make 32-bit Randomom number
static uint32_t rand32 (void)
{
    uint32_t result;
    
    result = rand();
    return result << 16 | rand();
}

// init the state with seed
void SeedRandom (unsigned int seed)
{
    int i;
    
    srand(seed);
    for (i = 0; i < CMWC_CYCLE; i++)
        Q[i] = rand32();
    
    do
        c = rand32();
    while (c >= CMWC_C_MAX);
    ri = CMWC_CYCLE - 1;
}

uint32_t Random (void)
{
    uint64_t const a = 18782;
    uint32_t const m = 0xfffffffe;
    uint64_t t;
    uint32_t x;

    ri = (ri + 1) & (CMWC_CYCLE - 1);
    t = a * Q[ri] + c;
    c = t >> 32;
    x = (uint32_t)(t + c);
    if (x < c) {
        x++;
        c++;
    }
    return Q[ri] = m - x;
}
