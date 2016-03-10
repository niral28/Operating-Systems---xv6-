#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "traps.h"
#include "spinlock.h"

// Interrupt descriptor table (shared by all CPUs).
struct gatedesc idt[256];
extern uint vectors[];  // in vectors.S: array of 256 entry pointers
struct spinlock tickslock;
uint ticks;

void
tvinit(void)
{
  int i;

  for(i = 0; i < 256; i++)
    SETGATE(idt[i], 0, SEG_KCODE<<3, vectors[i], 0);
  SETGATE(idt[T_SYSCALL], 1, SEG_KCODE<<3, vectors[T_SYSCALL], DPL_USER);
  
  initlock(&tickslock, "time");
}

void
idtinit(void)
{
  lidt(idt, sizeof(idt));
}

//PAGEBREAK: 41
void
trap(struct trapframe *tf)
{
  if(tf->trapno == T_SYSCALL){
    if(proc->killed)
      exit();
    proc->tf = tf;
    syscall();
    if(proc->killed)
      exit();
    return;
  }

  switch(tf->trapno){
  case T_IRQ0 + IRQ_TIMER:
    if(cpu->id == 0){
      acquire(&tickslock);
      ticks++;
      wakeup(&ticks);
      release(&tickslock);
    }
    lapiceoi();
    updateTicks();  
    //cprintf("In Alarm\n");
    break;
  case T_IRQ0 + IRQ_IDE:
    ideintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE+1:
    // Bochs generates spurious IDE1 interrupts.
    break;
  case T_IRQ0 + IRQ_KBD:
    kbdintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_COM1:
    uartintr();
    lapiceoi();
    break;
  case T_IRQ0 + 7:
  case T_IRQ0 + IRQ_SPURIOUS:
    cprintf("cpu%d: spurious interrupt at %x:%x\n",
            cpu->id, tf->cs, tf->eip);
    lapiceoi();
    break;
  case T_DIVIDE :
    if( proc->sigHandlers[SIGFPE] != -1){
	uint old_eip= proc->tf->eip; 
	 proc->tf->eip = proc->sigHandlers[SIGFPE];
	siginfo_t info; 
	info.signum = SIGFPE; 
	int decr = sizeof(info); 
	*((siginfo_t *) (proc->tf->esp - decr)) = info; 
	decr += sizeof(proc->sigHandlers[SIGFPE]);       
	*((uint*) (proc->tf->esp-decr)) = old_eip;
	proc->tf->esp -=decr;
	*( (uint*) (proc->tf->esp - 4)) = old_eip;
	*( (uint*) (proc->tf->esp - 8)) = proc->tf->eax;
	cprintf("eax: %d\n", proc->tf->eax); 
	*( (uint*) (proc->tf->esp - 12)) = proc->tf->ecx;
	cprintf("ecx: %d\n", proc->tf->ecx); 
	*( (uint*) (proc->tf->esp - 16)) = proc->tf->edx;
	cprintf("edx: %d\n", proc->tf->edx); 
	*( (siginfo_t*) (proc->tf->esp - 20)) =info;
	cprintf("info addr: %d\n", (uint)&info); 
	*( (uint*) (proc->tf->esp - 24)) = proc->restore;
	cprintf("trampoline addr: %d\n", proc->restore); 
	proc->tf->esp -= 24;
	cprintf("esp: %d\n", proc->tf->esp); 
      //cprintf("DIVIDE BY ZERO!!\n");
      break;   
     } 
	//maybe think about adding custom kill code
   // 
    
  //PAGEBREAK: 13
  default:
    if(proc == 0 || (tf->cs&3) == 0){
      // In kernel, it must be our mistake.
      cprintf("unexpected trap %d from cpu %d eip %x (cr2=0x%x)\n",
              tf->trapno, cpu->id, tf->eip, rcr2());
      panic("trap");
    }
    // In user space, assume process misbehaved.
  //cprintf("here 3\n");
    cprintf("pid %d %s: trap %d err %d on cpu %d "
            "eip 0x%x addr 0x%x--kill proc\n",
            proc->pid, proc->name, tf->trapno, tf->err, cpu->id, tf->eip, 
            rcr2());
    proc->killed = 1;
  }

  // Force process exit if it has been killed and is in user space.
  // (If it is still executing in the kernel, let it keep running 
  // until it gets to the regular system call return.)
  if(proc && proc->killed && (tf->cs&3) == DPL_USER)
    exit();

  // Force process to give up CPU on clock tick.
  // If interrupts were on while locks held, would need to check nlock.
  if(proc && proc->state == RUNNING && tf->trapno == T_IRQ0+IRQ_TIMER)
    yield();

   if(proc && proc->alarmTicks >0){
	//proc->numTicks++;	
	//updateTicks(); 	
	//cprintf("increment ticks! %d  alarm on at:%d\n", proc->numTicks, proc->alarmTicks); 	
 	if ( proc->numTicks >= proc->alarmTicks){
		cprintf("here:\n"); 
		if(proc->sigHandlers[SIGALRM] == -1){	
		cprintf("here 2:\n"); 	
			cprintf("pid %d %s: trap %d err %d on cpu %d "
            	"eip 0x%x addr 0x%x--kill proc\n",
            	proc->pid, proc->name, tf->trapno, tf->err, cpu->id, tf->eip, 
           	 rcr2());
    		proc->killed = 1; 
		exit(); 
		}
		//cprintf("changing trap frame"); 
		uint old_eip= proc->tf->eip; 
		 proc->tf->eip = proc->sigHandlers[SIGALRM];
		siginfo_t info; 
		info.signum = SIGALRM; 
		//int decr = sizeof(info); 
		//*((siginfo_t *) (proc->tf->esp - decr)) = info; 
		//decr += sizeof(proc->sigHandlers[SIGALRM]);       
		//*((uint*) (proc->tf->esp-decr)) = old_eip;
		//proc->tf->esp -=decr; 	
	cprintf("starting editing:\n"); 
        *( (uint*) (proc->tf->esp - 4)) = old_eip;
	*( (uint*) (proc->tf->esp - 8)) = proc->tf->eax;
	cprintf("eax: %d\n", proc->tf->eax); 
	*( (uint*) (proc->tf->esp - 12)) = proc->tf->ecx;
	cprintf("ecx: %d\n", proc->tf->ecx); 
	*( (uint*) (proc->tf->esp - 16)) = proc->tf->edx;
	cprintf("edx: %d\n", proc->tf->edx); 
	*((siginfo_t*) (proc->tf->esp - 20)) = info;
	cprintf("info addr: %d\n", (uint)&info); 
	*( (uint*) (proc->tf->esp - 24)) = proc->restore;
	cprintf("trampoline addr: %d\n", proc->restore); 
	proc->tf->esp -= 24;
	cprintf("esp: %d\n", proc->tf->esp); 

		proc->numTicks =0; 
		proc->alarmTicks=0; 	
	} 
    }

  // Check if the process has been killed since we yielded
  if(proc && proc->killed && (tf->cs&3) == DPL_USER)
    exit();
}
