#include "types.h"
#include "user.h"

int main(void){
  printf(1,"hello"); 
  int realPid = getpid(); 
    int myPid = getmypid();
  printf(1,"Original Pid = %d, My Pid = %d\n", realPid, myPid);
  exit();

}
