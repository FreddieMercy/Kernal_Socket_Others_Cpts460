#include "type.h"

PROC proc[NPROC], *running, *freeList, *readyQueue, *sleepList;
int procSize = sizeof(PROC);
int nproc = 0;

int body();
int time = InterruptTime;
int directUmode = 1;
char *pname[]={"Sun", "Mercury", "Venus", "Earth",  "Mars", "Jupiter", 
               "Saturn", "Uranus", "Neptune" };

#include "vid.c" 
#include "int.c"
#include "timer.c"

int init()
{
    PROC *p;
    int i;

    printf("init ....");

    for (i=0; i<NPROC; i++){   // initialize all procs
        p = &proc[i];
        p->pid = i;
        p->status = FREE;
        p->priority = 0;  
        strcpy(proc[i].name, pname[i]);
        p->inkmode = 1;   
        p->next = &proc[i+1];
    }
    freeList = &proc[0];      // all procs are in freeList
    proc[NPROC-1].next = 0;
    readyQueue = sleepList = 0;

    /**** create P0 as running ******/
    p = get_proc(&freeList);
    p->status = RUNNING;
    p->ppid   = 0;
    p->parent = p;
    running = p;
    nproc = 1;
    printf("done\n");
} 

int scheduler()
{
    if (running->status == RUNNING){
       running->status = READY;
       enqueue(&readyQueue, running);
    }
    running = dequeue(&readyQueue);
    running->status = RUNNING;

    time = InterruptTime;
    color = 0x09 + running->pid;
    printf("next running=%d  time: %d\n", running->pid, time);
}

int int80h(), tinth();

int set_vec(vector, addr) u16 vector, addr;
{
    u16 location,cs;
    location = vector << 2;
    put_word(addr, 0, location);
    put_word(0x1000,0,location+2);
}
            
main()
{

    vid_init();
    lock();

    printf("MTX starts in main()\n");
    init();      // initialize and create P0 as running
    itimer_init();
    set_vec(80,int80h);
    set_vec(8,tinth);
    
    kfork("/bin/u1");     // P0 kfork() P1
    kfork("/bin/u1"); 
    kfork("/bin/u1"); 
    kfork("/bin/u1"); 
    kfork("/bin/u1"); 

    timer_init();
    
    scheduler();
    goUmode();
    
    while(1){
      printf("P0 running\n");
      if (nproc==2 && proc[1].status != READY)
      {
    	  printf("no runable process, system halts\n");
      }
      while(!readyQueue);
      printf("P0 switch process\n");
      tswitch();   // P0 switch to run P1
   }
}
