#ifndef XV6_SIGNAL
#define XV6_SIGNAL 

typedef struct{
  int signum; 
}siginfo_t;

typedef void (*sighandler_t)(siginfo_t);

#define SIGFPE 0 
#define SIGALRM 1 

#endif
