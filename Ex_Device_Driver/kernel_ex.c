#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/input.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/jiffies.h>

#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#include <linux/cdev.h>
#include <linux/mm.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
// #include <mach/hardware.h>
//#include <plat/gpio-cfg.h>

#include "ex_device.h"

#define DeviceName  "hello"

static int dev_major;
static int dev_minor;
static int val = 77;
//=========================================================================
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

    default: 
      printk("err cmd = %d\n", cmd);       
      return -ENOTTY;
  }
  return 0;
}

//=================device attribute  ======================================
static ssize_t hello_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    printk("hello_show \n");
    
    return 0;
}

static ssize_t hello_store(struct device *dev, 
                                   struct device_attribute *attr,
                                   const char *buf, size_t size)
{
    printk("hello_store  \n");
    return -1;
}

static struct device_attribute hello_adv_setting = {
    .attr = {
        .name = "hello_adv",
        .mode = 0664,
    },
    .show = hello_show,
    .store = hello_store,
};

static struct device_attribute *hello_adv_group[] = {
    &hello_adv_setting,
//    &hello_adv_setting2,
    
};

struct my_struct {
    struct platform_device *pdev;
    struct cdev mdev;
};

struct file_operations devEx_fops = {
  .owner = THIS_MODULE,
  .open = devEx_open,
  .release = devEx_release,
  .read = devEx_read,
  .unlocked_ioctl = devEx_ioctl,
  .write = devEx_write,
};

static int  hello_probe(struct platform_device *pdev)
{
    int i=0, ret = 0;
    dev_t dev;
    struct my_struct *my;

    printk("hello_probe\n");
    //此目的應是將 driver 和 device 結合在一起
    //Probe()函数必须验证指定设备的硬件是否真的存在，probe()可以使用设备的资源，包括时钟，platform_data等，Platform driver可以通过下面的函数完成对驱动的注册：
    //注册的过程会通过probe来进行相应资源的申请，以及硬件的初始化，如果probe执行成功，则device和driver的绑定就成功了。
    //设备注册的时候同样会在总线上寻找相应的驱动，如果找到他也会试图绑定，绑定的过程同样是执行probe。
    
    my = kmalloc(sizeof(*my), GFP_KERNEL);
    if (!my)
    {
        printk("kmalloc failed\n");
        return -ENOMEM;
    }
    platform_set_drvdata(pdev, my);
    my->pdev = pdev;

    ret = alloc_chrdev_region(&dev, 0, 1, "ExDev");
    if(ret < 0)
    {
        printk("can't alloc chrdev\n");
        return ret;
    }
    dev_major = MAJOR(dev);
    dev_minor = MINOR(dev);
    printk("register chrdev(%d,%d)\n",dev_major,dev_minor);

    //my->mdev.name   = DeviceName;
    //my->mdev.fops   = &devEx_fops;
    cdev_init(&my->mdev, &devEx_fops);
    my->mdev.owner = THIS_MODULE;
    ret = cdev_add(&my->mdev, MKDEV(dev_major, dev_minor),1);
    if(ret < 0){
        printk("add chr dev failed\n");
        return ret;
   }

    for (i = 0; i < ARRAY_SIZE(hello_adv_group); i++) {
        ret = device_create_file(&pdev->dev, hello_adv_group[i]);
        if (ret) {
            dev_err(&pdev->dev, "failed: sysfs file %s\n",
                    hello_adv_group[i]->attr.name);
        }else
        {
            printk("initial device_attribute\n");
        }
    }
    
    return 0;
}

static int __exit hello_remove(struct platform_device *pdev)
{
    printk("hello_remove\n");
    return 0;
}

static int hello_suspend(struct platform_device *pdev,pm_message_t state)
{
    printk("hello_suspend\n");
    return 0;
}

static int hello_resume(struct platform_device *pdev)
{
    printk("hello_resume\n");
    return 0;
}

void hello_release(struct device *dev)
{
    printk("hello release.\n");
}
/*
struct resource hello_res[] = {
    {
        .name = "hello_res_name",
        .start = 0x56000000,
        .end = 0x57000000,
        .flags = IORESOURCE_IO,
    },

    {
        .start = IRQ_NONE,
        .end = IRQ_NONE,
        .flags = IORESOURCE_IRQ,
    },
};
*/
static struct platform_driver hello_driver = {
    .probe        = hello_probe,
    .remove        = __exit_p(hello_remove),
    .driver        = {
        .name    = DeviceName, //此name 需要 driver 和 device 相同
        .owner    = THIS_MODULE,
    },
    .suspend    = hello_suspend,
    .resume = hello_resume,
};

static struct platform_device hello_device = {
    .name = DeviceName,        //此name 需要 driver 和 device 相同
    .dev = {
        .release = hello_release, //.platform_data = &hello_data,
    }
    //.num_resources = ARRAY_SIZE(hello_res),
    //.resource = hello_res,   
};

EXPORT_SYMBOL_GPL(hello_device);

static int __init hello_init(void)
{
    int ret = 0;
    //static struct *pdev;

    ret = platform_device_register(&hello_device);  //內已有 platform_device_add
/*    pdev = platform_device_alloc(DeviceName, 0);
    if(pdev)
        {printk("platform_device_alloc failed\n");}
    else
        {printk("platform_device alloc successful\n");}

    if(ret = platform_device_add(pdev))
    {
        printk("platform_device_add failed\n");
    }
    else
    {
        printk("platform_device add successful\n");
        printk("platform_device_register  \n");
    }
*/    
    if (ret == 0) {
        ret = platform_driver_register(&hello_driver);
        printk("platform_driver_register \n");
    }


    
    return ret;
}
module_init(hello_init);

static void __exit hello_exit(void)
{


    platform_driver_unregister(&hello_driver);
    platform_device_unregister(&hello_device);
    printk("platform_device_unregister  \n");
}
module_exit(hello_exit);


MODULE_AUTHOR("<guangyaw.twbbs.org>");
MODULE_DESCRIPTION("hello driver");
MODULE_LICENSE("GPL");

