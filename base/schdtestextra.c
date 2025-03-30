#include "types.h"
#include "defs.h"

/* ATTENTION: to ensure correct compilation of the base code, 
   stub functions for the system call user space wrapper functions are provided. 
   REMEMBER to disable the stub functions (by commenting the following macro) to 
   allow your implementation to work properly. */
//#define STUB_FUNCS
//#ifdef STUB_FUNCS
//void set_sched(int scheduler) {}
//int tickets_owned(int pid) {return 0;}
//int transfer_tickets(int pid, int tickets) {return 0;}
//#endif

/* IMPORTANT INSTRUCTION: the test code below should not be changed. 
   Failure to follow this instruction will lead to zero point for this part */

#define SCHEDULER_DEFAULT  0
#define SCHEDULER_STRIDE 1

#define LOOP_CNT 0x10000000

#define MAX_CHILD_COUNT 6

struct {
    int pid;
} child[MAX_CHILD_COUNT];

unsigned int avoid_optm = 0; 

void do_child(void)
{
    unsigned int cnt = 0;
    unsigned int tmp = 0;

    while(cnt < LOOP_CNT)
    {
        tmp += cnt;
        cnt ++;
    }
        
    avoid_optm = tmp;
}


void do_parent(void)
{
    unsigned int cnt = 0;
    unsigned int tmp = 0;

    while(cnt < LOOP_CNT)
    {
        tmp += cnt;
        cnt ++;
    }
    
    avoid_optm = tmp;
}

void do_parent_partial(void)
{
    unsigned int cnt = 0;
    unsigned int tmp = 0;

    while(cnt < LOOP_CNT/2)
    {
        tmp += cnt;
        cnt ++;
    }
    
    avoid_optm = tmp;
}

void create_child_process(int cnt)
{
    int child_cnt = cnt > MAX_CHILD_COUNT ? MAX_CHILD_COUNT : cnt;
    int i = 0;

    for (i = 0; i < child_cnt; i++)
    {
        child[i].pid = fork();

        if (child[i].pid < 0)
        {
            printf(1, "fork() failed!\n");
            exit();
        }
        else if (child[i].pid == 0) // child
        {                       
            do_child(); // doing some computation for some time
            exit();
        }
        else // parent
        {
            //do nothing here
        }
    }        
}

void print_proc_tickets(int cnt)
{
    int child_cnt = cnt > MAX_CHILD_COUNT ? MAX_CHILD_COUNT : cnt;
    int i = 0; 
    
    printf(1, "parent (pid %d) has %d tickets.\n", getpid(), tickets_owned(getpid()));
    
    for (i = 0; i < child_cnt; i++)
    {  
        printf(1, "child (pid %d) has %d tickets.\n", child[i].pid, tickets_owned(child[i].pid));     
    }    
}

void wait_on_child_processes(int cnt)
{
    int child_cnt = cnt > MAX_CHILD_COUNT ? MAX_CHILD_COUNT : cnt;
    int i = 0; 
    
    for (i = 0; i < child_cnt; i++)
    {   
       if (wait() < 0)
       {
            printf(1, "\nwait() on child-%d failed!\n", i);
       }
    }            

}

void test_case_7(void){
    int child_num = 2;
    int scheduler = SCHEDULER_STRIDE;

    printf(1, "==== Test case 7: processes creation and redistribution at runtime ====\n");
    set_sched(scheduler);
    create_child_process(child_num);
    print_proc_tickets(child_num);
    enable_sched_trace(1);
    do_parent();
    wait_on_child_processes(child_num);
    enable_sched_trace(0);

    printf(1,"process must have finished\n");
    create_child_process(1);
    print_proc_tickets(1);

    enable_sched_trace(1);
    do_parent();
    wait_on_child_processes(1);
    enable_sched_trace(0);

    printf(1,"\n");
}

void test_case_8(void){
    int child_num = 1;
    int scheduler = SCHEDULER_STRIDE;
    int transfer_amt = 10;

    printf(1, "==== Test case 8: sequential transfers ====\n");
    set_sched(scheduler);
    create_child_process(child_num);
    print_proc_tickets(child_num);

    printf(1, "Parent %d transfering %d tickets to child %d \n", getpid(), transfer_amt, child[0].pid);
    transfer_tickets(child[0].pid, transfer_amt);
    printf(1, "%d\n",tickets_owned(getpid()));
    print_proc_tickets(child_num);

    transfer_amt = 5;
    printf(1, "Parent %d transfering %d tickets to child %d \n", getpid(), transfer_amt, child[0].pid);
    transfer_tickets(child[0].pid, transfer_amt);
    printf(1, "%d\n",tickets_owned(getpid()));
    print_proc_tickets(child_num);

    transfer_amt = 0;
    printf(1, "Parent %d transfering %d tickets to child %d \n", getpid(), transfer_amt, child[0].pid);
    transfer_tickets(child[0].pid, transfer_amt);
    printf(1, "%d\n",tickets_owned(getpid()));
    print_proc_tickets(child_num);

    enable_sched_trace(1);
    do_parent();
    wait_on_child_processes(child_num);
    enable_sched_trace(0);
    printf(1, "\n");
}

int
main(int argc, char *argv[])
{
    test_case_7();
    test_case_8();
    exit();
}

