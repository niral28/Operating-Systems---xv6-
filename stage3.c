#include "types.h"
#include "stat.h"
#include "user.h"
#include "signal.h"

static int numTraps=0;
static int totalTrapTime =0;
void handle_signal(siginfo_t info)
{
 numTraps++;
 if(numTraps == 1000000){
  stopTraps();
// __asm__("subl $0x10,%ebp\n\t movl %ebp,%esp\n\t ret\n\t");
  return;
 }
}

int main(int argc, char *argv[])
{
	signal(SIGFPE, handle_signal);
	int x = 5;
        int y = 0;
        int time1 = uptime();
        x = x/y;
        int time2 = uptime();
         totalTrapTime = time2-time1;
         float avg = (totalTrapTime)/ ((float) numTraps); 
       
        //  printf(1,"reached back to main\n");
	printf(1, "Traps Performed: %d\n",numTraps);
	printf(1, "Total Elapsed Ticks: %d ticks\n",totalTrapTime);
	printf(1, "Average Ticks Per Trap: %d microTicks\n", (int)(avg*1000000));
        totalTrapTime = 0; 
        numTraps=0;
	exit();
}
