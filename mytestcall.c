#include <linux/unistd.h>
#include <stdio.h>
#include <linux/kernel.h>    /* Needed for KERN_INFO */
#include <linux/sched.h>
#include <linux/tty.h>
#include <linux/sched.h>
#include <stdlib.h> 

#define __NR_my_syscall 359
#define HOUR 3600 
#define MIN 60

struct infoList{
    
    int pid;
    char tty[64];
    long time;
    char cmd[16];
};

 

int main()
{

   //struct infoList array[3000];
	struct infoList array[3000];
        
	int count = syscall(__NR_my_syscall, 0, 0, NULL, NULL, array);
    
  //  printf("Fount %d items", count);
    printf("  PID TTY\t    TIME CMD\n");
    
    int i;
    for(i = 0; i < count; i++) {
    
        // This prints out the PID numbers
        if(array[i].pid < 10) {
            printf("    %d", array[i].pid);
        } else if(array[i].pid < 100) {
            printf("   %d", array[i].pid);
        } else if(array[i].pid < 1000) {
            printf("  %d", array[i].pid);
        } else if(array[i].pid < 10000) {
            printf(" %d", array[i].pid);
        } else {
            printf("ERROR");
        }
        
        // This prints the TTY stuff
        printf(" %s", array[i].tty);//array[i].tty);
        
        // This prints the TIME stuff
        long seconds=array[i].time;

	long hours = 0, minutes = 0;

	while (seconds > 59)
	{
		if (seconds >= 3600)
		{
			seconds -= 3600;
			hours++;
		}
		else if (seconds >= 60)
		{
			seconds -= 60;
			minutes++;
		}
	}
   	if(strcmp(array[i].tty,"?") == 0)
	{
	    printf("\t\t%.2ld:%.2ld:%.2ld",hours,minutes,seconds);
	}
	else
	{
		printf("\t%.2ld:%.2ld:%.2ld",hours,minutes,seconds);
	}
        
        // This prints the CMD stuff
        printf(" %s\n", array[i].cmd);
    }
    
    return 0;
}

