#include "types.h"
#include "stat.h"
#include "user.h"
#include "signal.h"

static int numTraps=0;
static int totalTrapTime=0;
void handle_signal(siginfo_t info)
{
  //printf(1,"inside sig handler!\n");
 numTraps++;
 totalTrapTime+= uptime();
 if(totalTrapTime >= 10000){
  printf(1,"Traps performed: %d\n",numTraps);
  printf(1,"Total Elapsed Time %d:\n",totalTrapTime);
  printf(1,"Average Time Per Trap %d\n");
 __asm__("subl $0x18,%ebp\n\t movl %ebp,%esp\n\t ret\n\t");
  return;
 }
//printf(1,"finished\n");
//reurn;
}

int main(int argc, char *argv[])
{
	signal(SIGFPE, handle_signal);
	int x = 5;
        int y = 0;
        x = x/y;
        //  printf(1,"reached back to main\n");
	printf(1, "Traps Performed: XXXX\n");
	printf(1, "Total Elapsed Time: XXXX\n");
	printf(1, "Average Time Per Trap: XXXXX\n");

	exit();
}
