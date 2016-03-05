
#include <linux/module.h> // Needed by all modules
#include <linux/kernel.h> // Needed for KERN_ALERT
#include <linux/init.h> // Needed for init and exit macros
#include <linux/list.h> // Needed for doubly linked lists and for_each_process macro ???
#include <linux/tty.h> // Needed to access the different tty information ???
#include <linux/sched.h> // Needed for the task lists
#include <sys/syscall.h> // Needed to replace system calls with custom system calls

#define LICENSE "N/A" // license of the module
#define AUTHOR "Joshua Monson <jlmonso1@asu.edu>, Daniel Caruso II <~~~@asu.edu>" // The people who wrote this module
#define DESCRIPTION "Module designed to detect and defuse fork bombs" // Description of the module

/**
 * --< Start Command Line Argument stuff >--
 */
/*
 * The default values of the command line parameters as well as where the set value is stored
 */
static int maxForksPerSecond = 50 // Number of forks we allow per second
static int maxTasksPerUser = 100 // Number of tasks we allow per user

/*
 * The different command line parameters we allow
 */
MODULE_PARAM(maxForksPerSecond, "i");
MODULE_PARAM(maxTasksPerUser, "i");

/*
 * Descriptions of the different parameters we allow
 */
MODULE_PARAM_DESC(maxForksPerSecond, "How many forks allowed per second.");
MODULE_PARAM_DESC(maxTasksPerUser, "How many tasks allowed per user.");
/**
 * --< End Command Line Argument stuff >--
 */

/**
 * --< Start System Call Replacement stuff >--
 */
/*
 * The table of all system call functions.
 */
extern void *sys_call_table[];

/*
 * The location of the original system calls so that we can use them ourselves and properly replace them when finished.
 */
asmlinkage int (*original_sys_fork)(struct pt_regs);
asmlinkage int (*original_sys_vfork)(struct pt_regs); // This calls the same underlying method as sys_fork
asmlinkage int (*original_sys_clone)(struct pt_regs); // as does this

/*
 * Modified System call functions with fork bomb detections.
 */
 /*
  * Modified version of sys_fork to notify us whenever it is called and allow us to run the detection code.
  */
asmlinkage int modified_sys_fork(struct pt_regs regs)
{
	printk(KERN_ALERT "%d< %s >( %d ) - Forking.", current->uid, current->comm, current->pid);
	return original_sys_fork(regs);
}

/*
 * Modified version of sys_vfork to notify us whenever it is called and allow us to run the detection code.
 */
asmlinkage int modified_sys_vfork(struct pt_regs regs)
{
	printk(KERN_ALERT "%d< %s >( %d ) - VForking.", current->uid, current->comm, current->pid);
	return original_sys_vfork(regs);
}

/*
 * Modified version of sys_clone to notify us whenever it is called and allow us to run the detection code.
 */
asmlinkage int modified_sys_clone(struct pt_regs regs)
{
	printk(KERN_ALERT "%d< %s >( %d ) - Cloning.", current->uid, current->comm, current->pid);
	return original_sys_clone(regs);
}
/**
 * --< End System Call Replacement stuff >--
 */

/*
 * 
 */
static int bomb_defuse_init(void)
{
	printk(KERN_ALERT "Setting up bomb_defuse Module\n");
	
	// Save current system calls to variables so they can be used and restored later
	original_sys_fork = sys_call_table[SYS_fork];
	original_sys_vfork = sys_call_table[SYS_vfork];
	original_sys_clone = sys_call_table[SYS_clone];
	
	// Replace current system calls with custom system calls
	sys_call_table[SYS_fork] = modified_sys_fork;
	sys_call_table[SYS_vfork] = modified_sys_vfork;
	sys_call_table[SYS_clone] = modified_sys_clone;
	
	printk(KERN_ALERT "Finished setting up bomb_defuse Module\n");
	return 0;
}

/*
 *
 */
static void bomb_defuse_exit(void)
{
	
	// Replace custom system calls with the regular system calls
	sys_call_table[SYS_fork] = original_sys_fork;
	sys_call_table[SYS_vfork] = original_sys_vfork;
	sys_call_table[SYS_clone] = original_sys_clone;
	
	printk(KERN_ALERT "Exiting bomb_defuse Module\n");
	return;
}

/*
 * Macro to tell the module what its init function is.
 */
module_init(bomb_defuse_init);

/*
 * Macro to tell the module what its exit function is.
 */
module_exit(bomb_defuse_exit);

/*
 * Tell the module what its license is.
 */
MODULE_LICENSE(LICENSE);

/*
 * Tell the module who its author(s) are.
 */
MODULE_AUTHOR(AUTHOR);

/*
 * Tell the module what its description is.
 */
MODULE_DESCRIPTION(DESCRIPTION);
