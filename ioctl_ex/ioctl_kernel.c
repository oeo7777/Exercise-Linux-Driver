#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h> 

#include "IOC_Test.h"

#define DEVICE_NAME "ExDev"
#define MAJOR_NUM    117
#define NUM_DEVICES  1

static int dev_major;
static int dev_minor;
static int val = 77;
struct cdev *dev_cdevp = NULL;

static int devEx_open(struct inode *inode, struct file *file)
{
  printk("devEx_open \n");
  return 0;
}


static int devEx_release(struct inode *inode, struct file *file)
{
  printk("devEx_release \n");
  return 0;
}


static ssize_t devEx_read(struct file *file, char *buffer, size_t length, loff_t * offset)
{
  printk("devEx_read \n");
  return 0;
}


static ssize_t devEx_write(struct file *file, const char *buffer, size_t length, loff_t * offset)
{
  printk("devEx_write \n");
  return length;
}

static long devEx_ioctl(struct file *filp, unsigned int cmd, unsigned long args)
{ //此的 結構與 function 引數皆有修改
  //ioctl  =>  unlocked_ioctl
  int tmp = 20;
  int err=0;

  printk("ioctl rx cmd = %d\n", cmd); 

  if(_IOC_TYPE(cmd) != IOC_MAGIC)
    err = 0x1;
  if(_IOC_NR(cmd) > IOC_MAXNR)
    err |= 0x10;
  if(_IOC_DIR(cmd) & _IOC_READ){
    err = !access_ok(VERIFY_WRITE, (void __user*)args, _IOC_SIZE(cmd));
  } else if (_IOC_DIR(cmd) & (_IOC_WRITE)){
    err = !access_ok(VERIFY_READ, (void __user *)args,_IOC_SIZE(cmd));
  }

  if (err)
  {
    printk("err = %d\n", err); 
    return -EFAULT;
  }

  switch (cmd) {
    case SETNUM:
     //copy data from args(user) to val(kernel)
     if(copy_from_user(&val,(int __user *)args,1))
         return -1;

     printk("%s():get val from user = %d\n",__FUNCTION__,val);
     break;
    case GETNUM:
      //copy data from val to args
      val = val * 5;
      if(copy_to_user((int __user *)args,&val,1))
      return -1;

      printk("%s():set val to %d\n",__FUNCTION__, val);
      break;
    case XNUM:
      // exchange data passed by user
      if(copy_from_user(&val,(int __user *)args,1))
        return -1;

      if(copy_to_user((int __user *)args, &tmp, 1))
        return -1;

      printk("%s():change val from %d to %d\n",__FUNCTION__,tmp,val);
      break;

    default: /* redundant. as cmd was checked against MAXNR */
      printk("err cmd = %d\n", cmd);       
      return -ENOTTY;
  }
  return 0;
}

struct file_operations devEx_fops = {
  .owner = THIS_MODULE,
  .open = devEx_open,
  .release = devEx_release,
  .read = devEx_read,
  .unlocked_ioctl = devEx_ioctl,
  .write = devEx_write,
};


int __init devEx_init(void)
{
  int ret;
  dev_t dev;
 
  printk("ExDev initial start\n");

  ret = alloc_chrdev_region(&dev, 0, 1, "ExDev");
  if(ret < 0)
  {
    printk("can't alloc chrdev\n");
    return ret;
  }
  dev_major = MAJOR(dev);
  dev_minor = MINOR(dev);
  printk("register chrdev(%d,%d)\n",dev_major,dev_minor);

  dev_cdevp = kmalloc(sizeof(struct cdev), GFP_KERNEL);
  if (dev_cdevp == NULL){
    printk("kmalloc failed\n");
    goto failed;
  }

   // system call handler
   cdev_init(dev_cdevp, &devEx_fops);
   dev_cdevp->owner = THIS_MODULE;
   //register my device to kernel
   ret = cdev_add(dev_cdevp, MKDEV(dev_major, dev_minor),1);
   if(ret < 0){
        printk("add chr dev failed\n");
        goto failed;
   }
  //devEx_class = class_create(THIS_MODULE, DEVICE_NAME);
  //device_create(devEx_class, NULL, MKDEV(MAJOR_NUM, i), NULL, "ExDev%d", i);
  return 0;

  failed:
  if(dev_cdevp) {
    kfree(dev_cdevp);
    dev_cdevp = NULL;
    return -1;
  }
  return 0;
}


void __exit devEx_exit(void)
{
  dev_t dev;
  dev = MKDEV(dev_major, dev_minor);
  if(dev_cdevp){
    cdev_del(dev_cdevp);
    kfree(dev_cdevp);
  }
  unregister_chrdev_region(dev,1);
  printk("unregister chrdev\n");
}


module_init(devEx_init);
module_exit(devEx_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sam Exercise");
MODULE_DESCRIPTION("Exercise driver");
