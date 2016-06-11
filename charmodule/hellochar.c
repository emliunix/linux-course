#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/stat.h>
#include <linux/kobject.h>
#include <asm/uaccess.h>

#define DEVICE_NAME "hello"
#define CLASS_NAME "hello"
#define DEV_COUNT 256
#define BUF_LENGTH 128

MODULE_LICENSE("GPL");
MODULE_AUTHOR("LiuyhOoQ");
MODULE_DESCRIPTION("Hello world!!! char device");
MODULE_VERSION("0.1");
MODULE_SUPPORTED_DEVICE(DEVICENAME);

//static char* name = "Liu Yuhui";
//module_param(name, charp, 0/* S_IRUGO | S_IWUGO */);
//static char* id = "024";
//module_param(id, charp, 0/* S_IRUGO | S_IWUGO */);

static char* message = "Liu Yuhui 024";
module_param(message, charp, 0644);

static dev_t devnum_start;
static struct cdev*   cdev;
static struct class*  hello_class;
static struct device* hello_device;
static char  msg_buf[BUF_LENGTH];
static short msg_len = 0;
/* static struct class*  hellocharClass = NULL; */
/* static struct device* hellocharDevice = NULL; */

ssize_t hello_read(struct file *file,
                   char *buf,
                   size_t length,
                   loff_t *offset);
ssize_t hello_write(struct file *file,
                    const char *buf,
                    size_t length,
                    loff_t *offset);
int hello_open(struct inode *inode,
               struct file *file);
int hello_release(struct inode *indoe,
                  struct file *file);

static struct file_operations fops = {
    .read	 = hello_read,
    .write	 = hello_write,
    .open	 = hello_open,
    .release = hello_release
};

/* static int __init hello_init(void) { */
/*     // register */
/*     major = register_chrdev(0, DEVICE_NAME, &fops); */
/*     if(major < 0) { // error */
/*         printk(KERN_ERR "Hello: module init error.\n"); */
/*         return major; */
/*     } */
/*     printk(KERN_INFO "Hello: registered with major number %d.\n", major); */

/*     // init msg buf */
/*     msg_len = snprintf(msg_buf, BUF_LENGTH, "name: %s\tid: %s\n", name, id); */
/*     return 0; */
/* } */

int __init hello_init(void) {
    dev_t dev;
    int result = alloc_chrdev_region(&dev, 0, DEV_COUNT, DEVICE_NAME);
    devnum_start = dev;
    if(result < 0) { // error
        printk(KERN_ERR "Hello: failed to allocate major number region.\n");
        goto chrdev_err;
    }
    
    cdev = cdev_alloc();
    cdev_init(cdev, &fops);
    cdev->owner = THIS_MODULE;
    kobject_set_name(&cdev->kobj, "%s", DEVICE_NAME);
    
    result = cdev_add(cdev, MKDEV(MAJOR(dev), 0), DEV_COUNT);
    if(result < 0) { // error
        printk(KERN_ERR "Hello: failed to init character device.\n");
        goto dev_err;
    }

    hello_class = class_create(THIS_MODULE, CLASS_NAME);
    if(IS_ERR(hello_class)) {
        printk(KERN_ERR "Hello: failed to init class.\n");
        goto class_err;
    }

    hello_device = device_create(hello_class, NULL, dev, NULL, DEVICE_NAME);
    if(IS_ERR(hello_device)) {
        printk(KERN_ERR "Hello: failed to init device.\n");
        goto device_err;
    }

    printk(KERN_INFO "Hello: successfully initialized major:%d, minor:%d.\n",
           (int)MAJOR(devnum_start),
           (int)MINOR(devnum_start));
    kobject_uevent(&cdev->kobj, KOBJ_ADD);
    
    /* // init msg buf */
    /* msg_len = snprintf(msg_buf, BUF_LENGTH, "name: %s\tid: %s\n", name, id); */
    return 0;

 device_err:
    device_destroy(hello_class, dev);
 class_err:
    class_unregister(hello_class);
    class_destroy(hello_class);
 dev_err:
    cdev_del(cdev);
    unregister_chrdev_region(devnum_start, DEV_COUNT);
 chrdev_err:
    
    return -EFAULT;
}

static void __exit hello_exit(void) {
    kobject_uevent(&cdev->kobj, KOBJ_REMOVE);
    device_destroy(hello_class, devnum_start);
    class_unregister(hello_class);
    class_destroy(hello_class);
    cdev_del(cdev);
    unregister_chrdev_region(devnum_start, DEV_COUNT);
    printk(KERN_INFO "Hello: Well, I'm going to exit. good Bye.\n");
}

/* static void __exit hello_exit(void) { */
/*     unregister_chrdev(major, DEVICE_NAME); */
/*     printk(KERN_ALERT "Hello: Well, I'm going to exit. good Bye.'"); */
/* } */

module_init(hello_init);
module_exit(hello_exit);

/* device operations */

ssize_t hello_read(struct file *file,
                   char *buf,
                   size_t length,
                   loff_t *offset) {

    int bytes_read = 0;
    char* msg_ptr = NULL;
    msg_len = snprintf(msg_buf, BUF_LENGTH, "%s", message);

    msg_ptr = msg_buf;
    while(length && *msg_ptr) {
        put_user(*(msg_ptr++), buf++);
        
        --length;
        ++bytes_read;
    }
    return bytes_read;
}

ssize_t hello_write(struct file *file,
                    const char *buf,
                    size_t length,
                    loff_t *offset) {
    return -EFAULT; // write not supported
}

int hello_open(struct inode *inode,
               struct file *file) {
    /* MOD_INC_USE_COUNT; //outdated api */
    printk(KERN_INFO "Hello: device opened.\n");
    return 0;
}

int hello_release(struct inode *indoe,
                  struct file *file) {
    /* MOD_DEC_USE_COUNT; //outdated api */
    printk(KERN_INFO "Hello: device closed.\n");
    return 0;
}
