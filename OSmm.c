#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"myQueue.h"
#include"LinkedList.h"
#define mem_size 200
#define q 3//quantum for RR
#define p 6//input processes
#define maxi 10//max instructions allowed per process

int CPU(struct PCB*,int*,tQUEUE *,int*);
void Print(int,tQUEUE*,tQUEUE*,int,int);
struct PCB Fork(struct PCB*,int*);
int main()
{
    printf("\t ===MEMORY MANAGEMENT SIMULATOR - OS===\n\n");
    struct PCB process;
    int running=-1,segcounter=0,id=0,memindex=0,out=-1;
    int mem[mem_size];//main memory
    int * memptr=mem;
    //2d array with input programs (Hardcoded Matrix)
    const int program[p][maxi*2]={
    {0,6,0,500,1,13,1,7,6,13,8,13,9,-999,0,0,0,0,0,0},
    {1,7,0,100,1,5,2,5,4,3,9,99,8,5,5,2,0,0,0,0},
    {2,2,0,100,1,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {3,2,0,100,1,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {4,5,0,500,1,13,1,7,8,13,9,-999,0,0,0,0,0,0,0,0}, 
    {5,4,0,100,1,5,3,99,1,6,0,0,0,0,0,0,0,0,0,0}
    };
    printf(">Processes are being created...\n\n");
    printf("  T |stdout|RUNNING|READY\t|BLOCKED\n");
    //Have PCB stored on them
    tQUEUE newq,readyq,blockedq,exitq;
    newq.pBasic=pInitQueue(&newq);
    readyq.pBasic=pInitQueue(&readyq);
    blockedq.pBasic=pInitQueue(&blockedq);
    exitq.pBasic=pInitQueue(&exitq);
    for(int t=0;t<24;t++)//timer with a limit of 100 seconds
    {
        //if there are processes in new state, they go to ready state
        if(!bIsEmptyQueue(&newq))
        {
            struct PCB pro=front(&newq);
            //set base addresses
            if(memindex<mem_size)//continue creating partitions
            {
                if(pro.ppid!=-1)//if child process is being created
                {
                    pro.segment_table[1].base=memindex;
                    memindex+=pro.segment_table[1].limit;
                    vEnterQueue(&readyq,pro);
                    break;
                }
                pro.segment_table[0].base=memindex;
                pro.pc=pro.segment_table[0].base;
                pro.segment_table[1].base=pro.segment_table[0].base+pro.segment_table[0].limit;
                memindex+=pro.segment_table[0].limit+pro.segment_table[1].limit;
            }
            else if(memindex>=mem_size||memindex+pro.segment_table[0].limit>mem_size)
            {
                bool found=false;
                //dynamic partitions are created, hence we now follow best fit approach
                for(struct node* ptr = head;ptr!=NULL;ptr=ptr->next)//iterate linked list(free segments)
                {
                    //contiguous memory allocation
                    if(pro.segment_table[0].limit==ptr->data.limit&&ptr->next->data.limit==pro.segment_table[1].limit)
                    {
                        //segment found
                        pro.segment_table[0].base=ptr->data.base;
                        pro.segment_table[1].base=ptr->next->data.base;
                        found=true;
                        break;
                    }
                    if(found==false && pro.ppid!=-1)
                    printf(">fork unsuccessful!\n");
                    else
                    printf("Process not created!\n");
                }
            }
            int x=2;
            for(int k=pro.segment_table[0].base;k<pro.segment_table[0].base+pro.segment_table[0].limit;k++)
            {
                mem[k]=program[pro.pid][x];
                x++;
            }
            iOutQueue(&newq);
            //add to ready queue
            vEnterQueue(&readyq,pro);
        }

        //assuming at each instant, a new process is created
        if(id<p)
        {
            if(program[id][0]==t)//check if ini=t
            {
                int max=0;
                //make PCB
                struct PCB pcb;
                pcb.ppid=-1;
                pcb.childrenp=0;
                pcb.pid=id;
                //initializing timers stored in pcb
                pcb.running_timer=0;
                pcb.blocked_timer=0;
                pcb.segment_table[0].limit=program[t][1]*2;
                //find max number of variables for a process
                for(int j=3;j<program[t][1]*2;j=j+2)
                {
                    if(program[t][j]>max&&program[t][j-1]>0&&program[t][j-1]<9&&program[t][j-1]!=6)
                    max=program[t][j];
                }
                pcb.segment_table[1].limit=max+1;//including variable zero
                //add to new queue
                vEnterQueue(&newq,pcb);
                id++;
            }
        }

        //if there are processes in blockedq
        if(!bIsEmptyQueue(&blockedq))
        {
            for(int i=blockedq.qFront;i!=blockedq.qRear;i=i+1%QUEUE_SIZE)//iterate all blocked processes
            {

                if(blockedq.pBasic[i].blocked_timer==5)//check blocked queue timer 
                //and add those processes to readyqueue
                {
                    struct PCB proc=front(&blockedq);
                    proc.blocked_timer=0;//reset timer
                    iOutQueue(&blockedq);
                    vEnterQueue(&readyq,proc);
                }
                else//update timer
                blockedq.pBasic[i].blocked_timer++;
            }
        }
        //if there is a process in exit state, remove it
        if(!bIsEmptyQueue(&exitq))
        {
            struct PCB exitproc=front(&exitq);
            if(exitproc.childrenp==0)
            {
                //insert both segments info in linked list to manage space
                insertFirst(segcounter,exitproc.segment_table[0]);
                segcounter++;
                insertFirst(segcounter,exitproc.segment_table[1]);
                segcounter++;
                iOutQueue(&exitq);
            }
            else
            {
                //has children 
                //free memory
                insertFirst(segcounter,exitproc.segment_table[1]);
                segcounter++;  
            }
        }
        //if there are processes in ready state they're given cpu time
        if(!bIsEmptyQueue(&readyq)&&running==-1)//process in ready state but none in running
        {
            //using fcfs approach to choose process for execution
            process=iOutQueue(&readyq);
            running=process.pid;
        }
        if(running!=-1)//process in running
        {
            out=CPU(&process,memptr,&newq,&id);
            Print(t,&readyq,&blockedq,running,out);
            if(process.running_timer==q)//quantum ends
            {
                process.running_timer=0;//reset timer
                //add process to end of ready queue
                vEnterQueue(&readyq,process);
                running=-1;
            }
            //last instruction reached
            else if(process.pc==process.segment_table[0].base+process.segment_table[0].limit)
            {
                running=-1;
                vEnterQueue(&exitq,process);
            }
            else if(out==-2)//halt instruction
            {
                running=-1;
                vEnterQueue(&exitq,process);
            }
            else if(out==-3)//process blocked
            {
                running=-1;
                vEnterQueue(&blockedq,process);
            }
        }
    }
    printf("All processes completed Execution\n\n");
    printf("Exitting...\n");
}
int CPU(struct PCB * process,int * memptr,tQUEUE * newq,int * id)
{
    //process is getting one instant of cpu time
    process->running_timer++;
    //has a logical address with segment number and offset
    //segment number will be 0 (with executable) and 1 (with variables) only
    //offset is fixed i.e 2 for each instruction
    int offset=2;
    int instr_code=memptr[process->pc];
    int parameter=memptr[process->pc+1];//Var_X
    int baseaddr=process->segment_table[1].base;//for variables segment table

    switch(instr_code)
    {
        case 0://zero
        memptr[baseaddr]=parameter;
        break;
        case 1://copy
        memptr[baseaddr+parameter]=memptr[baseaddr];
        break;
        case 2://decrement
        memptr[baseaddr+parameter]--;
        break;
        case 3://Fork
        vEnterQueue(newq,Fork(process,id));
        break;
        case 4://Jump Forward
        if(process->pc+(parameter*offset)>process->segment_table[0].limit)
        printf(">Segmentation Error in Process%d\n",process->pid+1);
        else
        process->pc=process->pc+parameter*offset;
        break;
        case 5://jump Backward
        if(process->pc-(parameter*offset)<process->segment_table[0].base)
        printf(">Segmentation Error in Process%d\n",process->pid+1);
        else
        process->pc=process->pc-parameter*offset;
        break;
        case 6://DSK: I/O request
        process->pc+=offset;
        return -3;
        break;
        case 7://JIZ: jump to 2nd instr if Var X=0, else move to next
        if(parameter==0)
        {
            if((process->pc+2*offset)>process->segment_table[0].limit)
            printf(">Segmentation Error in Process%d\n",process->pid+1);
            else
            process->pc=process->pc+1*offset;
        }
        break;
        case 8://Print
        process->pc+=offset;
        return memptr[baseaddr+parameter];
        break;
        case 9://Halt
        return -2;
        break;
        default:
        instr_code=0;
    }
    process->pc+=offset;
    return -1;
}
void Print(int t,tQUEUE * readyq,tQUEUE * blockedq,int r,int out)
{
    if(t<10)
    printf(" 0%d |",t);//T
    else
    printf(" %d |",t);//T
    if(out!=-1&&out!=-2&&out!=-3)
    printf(" %d  |",out);//stdout
    else
    printf("      |");
    if(r!=-1)
    printf("   P%d  |",r+1);//running
    if(!bIsEmptyQueue(readyq))
    vTraverseQueue(readyq);
    else
    printf("\t\t");
    printf("|");
    if(!bIsEmptyQueue(blockedq))
    vTraverseQueue(blockedq);
    printf("\n");
}
struct PCB Fork(struct PCB * process,int * id)
{
    process->childrenp++;
    struct PCB childp;
    childp=*process;
    printf("Child process with PID:%d created\n",*id+1);
    childp.pid=*id;
    childp.ppid=process->pid;
    *id++;
    return childp;
}