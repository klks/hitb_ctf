/* PROTOTYPES should be set to one if and only if the compiler supports
  function argument prototyping.
The following makes PROTOTYPES default to 0 if it has not already
  been defined with C compiler flags.
 */
#ifndef PROTOTYPES
#define PROTOTYPES 0
#endif

typedef unsigned char *POINTER;
typedef unsigned short int UINT2;
typedef unsigned long int UINT4;

typedef struct CtfLib_MD5Context {
  unsigned long state[4];	/* state (ABCD) */
  unsigned long count[2];	/* number of bits, modulo 2^64 (lsb first) */
  unsigned char buffer[64];	/* input buffer */
} CTFLIB_MD5_CTX;

char * CtfLib_MD5File(const char *, char *);
char * CtfLib_MD5Data(const unsigned char *, unsigned int, char *);

