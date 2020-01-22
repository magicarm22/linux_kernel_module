#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/ktime.h>
#include <linux/sched/signal.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Robert W. Oliver II");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.01");

#define DEVICE_NAME "my_kernel"


static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static int major_num;
static int device_open_count = 0;
static char *msg_ptr;

static struct file_operations file_ops = 
{
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release
};

static ssize_t device_read(struct file *flip, char *buffer, size_t len, loff_t *offset) 
{
	int bytes_read = 0;
	char text[512];
	char pid[10];
	struct task_struct *task;
	struct timespec time;
	char time_str[128];

	strcpy(text, "\n\n\n\nList of programs with pid: \n");
	msg_ptr = text;
	while (len && *msg_ptr) {
		put_user(*(msg_ptr++), buffer++);
		len--;
		bytes_read++;
	}
	getnstimeofday(&time);
    for_each_process(task)
    {
    	sprintf(time_str, "%.2lu:%.2lu:%.2lu:%.6lu ",
				                   (time.tv_sec / 3600) % (24),
				                   (time.tv_sec / 60) % (60),
				                   time.tv_sec % 60,
				                   time.tv_nsec / 1000);
    	strcpy(text, time_str);
    	strcat(text, task->comm);
    	sprintf(pid, " (%d)\n", task->pid);
    	strcat(text, pid);
    	msg_ptr = text;
    	while (len && *msg_ptr) {
			put_user(*(msg_ptr++), buffer++);
			len--;
			bytes_read++;
		}
    }
	return bytes_read;
}

static ssize_t device_write(struct file *flip, const char *buffer, size_t len, loff_t *offset) 
{
	char message[128];
	int signum = SIGKILL;
	struct kernel_siginfo info;
	struct task_struct *task;
	int ret;

	sprintf(message, "%s", buffer);
	printk(KERN_INFO "%s", message);
	if (strncmp(message, "kill", 4) == 0)
	{
		for_each_process(task)
    	{
    		printk(KERN_INFO "%s - %s", task->comm, message + 5);
    		if ( (strncmp(task->comm, message + 5, strlen(message + 5) - 1) == 0 ) )
    		{
    			printk(KERN_INFO "Task %d %s killed", task->pid, task->comm);
    			break ;
    		}
    	}
		memset(&info, 0, sizeof(struct kernel_siginfo));
		info.si_signo = signum;
		ret = send_sig_info(signum, &info, task);
		if (ret < 0) {
		  printk(KERN_INFO "error sending signal\n");
		}
	}
	return 0;
}

static int device_open(struct inode *inode, struct file *file) 
{
	if (device_open_count) {
		return -EBUSY;
	}
	device_open_count = 1;
	try_module_get(THIS_MODULE);
	return 0;
}

static int device_release(struct inode *inode, struct file *file) 
{
	device_open_count = 0;
	module_put(THIS_MODULE);
	return 0;
}

static int __init lkm_example_init(void) 
{
	major_num = register_chrdev(0, "my_kernel", &file_ops);
	if (major_num < 0) 
	{
		printk(KERN_ALERT "Could not register device: %d\n", major_num);
		return major_num;
	} 
	else
	{
		printk(KERN_INFO "my_kernel module loaded with device major number %d\n", major_num);
		return 0;
	}
}
static void __exit lkm_example_exit(void) 
{
	unregister_chrdev(major_num, DEVICE_NAME);
	printk(KERN_INFO "Goodbye, World!\n");
}


module_init(lkm_example_init);
module_exit(lkm_example_exit);