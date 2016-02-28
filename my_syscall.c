/*---Start of mycall.c----*/

#include <linux/syscalls.h>
#include <linux/kernel.h>    /* Needed for KERN_INFO */
#include <linux/list.h>
#include <linux/sched.h>
#include <linux/tty.h> /* Needed to access TTY stuff */
#include <linux/jiffies.h> /* Needed for time */
#include <linux/printk.h> /* Needed to print to the kernal log file */
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/string.h>

/*
 *
 * struct used for storing data and then passing that data to the userspace
 *
 */
struct infoList{
    
    int pid;
    char tty[64];
    long time;
    char cmd[16];
};



/* a and b and c not needed */
asmlinkage int sys_my_syscall(int a, int *b, char *c, struct infoList *list, struct infoList *output)
{

   // extern unsigned long volatile jiffies;
    struct task_struct *task;
    int counter = 0;
    
    /*
     * Count the number of tasks there are
     */
    for_each_process(task) 
    {
        counter++;
    }
    
    struct infoList array[1];
    
    int i = 0;
    for_each_process(task) 
    {
        cputime_t utime = 0;
 	cputime_t stime = 0;
	cputime_t jiff;

// pr_info("Started Reading\n"); // debug
        array[0].pid = task->pid;
       // pr_info("PID Finished\n"); // debug
        if(task->signal != NULL && task->signal->tty != NULL) {
            strncpy(array[0].tty, task->signal->tty->name, 64);
        } else {
            strncpy(array[0].tty, "?", 64);
        }
       

	thread_group_cputime_adjusted(task, &utime, &stime);
	jiff = utime + stime;

// pr_info("TTY Finished\n"); // debug
        array[0].time = jiff / HZ;
       // pr_info("TIME Finished\n"); // debug
        strncpy(array[0].cmd, task->comm, 16); // copy cmd string to the cmd struct value
       // pr_info("Finished Reading\n"); // debug

pr_info("%d  %s  %ld  %s\n",array[0].pid, array[0].tty,array[0].time, array[0].cmd);
   	/*
    	* Copy the whole array from Kernel to User space
    	*/
    	copy_to_user(output+i, &array[0], sizeof(array));
        i++;
    }
    

    
    return counter;        
}

/*---End of mycall.c------*/
