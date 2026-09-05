#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include "ioctl_led.h"

#define DEVICE_NAME "myled"

static int major;
static struct class *led_class;
static struct cdev led_cdev;
static int led_state = 0; // 0 = off, 1 = on

static long led_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    switch (cmd) {
        case LED_ON:
            led_state = 1;
            printk(KERN_INFO "myled: LED turned ON\n");
            break;
        case LED_OFF:
            led_state = 0;
            printk(KERN_INFO "myled: LED turned OFF\n");
            break;
        case LED_GET:
            if (copy_to_user((int __user *)arg, &led_state, sizeof(led_state)))
                return -EFAULT;
            break;
        default:
            return -ENOTTY;
    }
    return 0;
}

static int led_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "myled: device opened\n");
    return 0;
}

static int led_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "myled: device closed\n");
    return 0;
}

static const struct file_operations led_fops = {
    .owner = THIS_MODULE,
    .open = led_open,
    .release = led_release,
    .unlocked_ioctl = led_ioctl,
};

static int __init led_init(void) {
    dev_t dev;

    if (alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME) < 0) {
        printk(KERN_ALERT "myled: failed to allocate device number\n");
        return -1;
    }
    major = MAJOR(dev);

    cdev_init(&led_cdev, &led_fops);
    if (cdev_add(&led_cdev, dev, 1) < 0) {
        unregister_chrdev_region(dev, 1);
        return -1;
    }

    led_class = class_create("myled_class");
    if (IS_ERR(led_class)) {
        cdev_del(&led_cdev);
        unregister_chrdev_region(dev, 1);
        return PTR_ERR(led_class);
    }

    device_create(led_class, NULL, dev, NULL, DEVICE_NAME);

    printk(KERN_INFO "myled: module loaded, major number %d\n", major);
    return 0;
}

static void __exit led_exit(void) {
    dev_t dev = MKDEV(major, 0);
    device_destroy(led_class, dev);
    class_destroy(led_class);
    cdev_del(&led_cdev);
    unregister_chrdev_region(dev, 1);
    printk(KERN_INFO "myled: module unloaded\n");
}

module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Numan Maldar");
MODULE_DESCRIPTION("Virtual LED character device with ioctl control");
