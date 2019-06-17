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

#include <linux/moduleparam.h>
// #include <mach/hardware.h>
//#include <plat/gpio-cfg.h>

#include "ex_device.h"

#define DeviceName  "ExDev"

static char *whom = "world";
module_param(whom, charp, S_IRUGO);  //或是在 module_param_array(name,type,num,perm); 才限制比較嚴格呢 ?
MODULE_PARM_DESC(whom, "who do u want to say hello");

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
  int tv  = 987;

  printk("devEx_read \n");

  if(copy_to_user((int __user *)buffer, &tv, length))
  {
    printk("devEx_read fault \n");
    return -1;
  }
  return 0;
}


static ssize_t devEx_write(struct file *file, const char *buffer, size_t length, loff_t * offset)
{
  int tv  = 0;

  printk("devEx_write \n");

  if(copy_from_user(&tv,(int __user *)buffer,1))
  {
    printk("devEx_write fault \n");
    return -1;
  }
  else
  {
    printk("get user set is %d \n", tv);
  }
  return length;
}

static long devEx_ioctl(struct file *filp, unsigned int cmd, unsigned long args)
{ //此的 結構與 function 引數皆有修改
  //ioctl  =>  unlocked_ioctl
  int tmp = 20;
  int err=0;

  //printk("ioctl rx cmd = %d\n", cmd); 
  printk("\n enter ioctl\n");

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
    printk("\n device_attribute say hello to u \n");
    
    return 0;
}

static ssize_t hello_store(struct device *dev, 
                                   struct device_attribute *attr,
                                   const char *buf, size_t size)
{
    printk("\n device_attribute store \n");
    return -1;
}

static struct device_attribute hello_adv_setting = {
    .attr = {
        .name = "hello_adv",
        .mode = 0666,
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

static int add_chac_device(struct platform_device *pdev)
{
  int ret = 0;
  dev_t dev;
  struct my_struct *my;

  my = kmalloc(sizeof(*my), GFP_KERNEL);
  if (!my)
  {
      printk("kmalloc failed\n");
      return -ENOMEM;
  }
  platform_set_drvdata(pdev, my);
  my->pdev = pdev;

  ret = alloc_chrdev_region(&dev, 0, 1, DeviceName);
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

  return ret;
}

static int  hello_probe(struct platform_device *pdev)
{
    int i=0, ret = 0;


    printk("hello_probe\n");
    //此目的應是將 driver 和 device 結合在一起
    //Probe()函数必须验证指定设备的硬件是否真的存在，probe()可以使用设备的资源，包括时钟，platform_data等，Platform driver可以通过下面的函数完成对驱动的注册：
    //注册的过程会通过probe来进行相应资源的申请，以及硬件的初始化，如果probe执行成功，则device和driver的绑定就成功了。
    //设备注册的时候同样会在总线上寻找相应的驱动，如果找到他也会试图绑定，绑定的过程同样是执行probe。
    

    if(add_chac_device(pdev) < 0){
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

static struct platform_driver exercise_driver = {
    .probe        = hello_probe,
    .remove        = __exit_p(hello_remove),
    .driver        = {
        .name    = DeviceName, //此name 需要 driver 和 device 相同
        .owner    = THIS_MODULE,
    },
    .suspend    = hello_suspend,
    .resume = hello_resume,
};

static struct platform_device exercise_device = {
    .name = DeviceName,        //此name 需要 driver 和 device 相同
    .dev = {
        .release = hello_release, //.platform_data = &hello_data,
    }
    //.num_resources = ARRAY_SIZE(hello_res),
    //.resource = hello_res,   
};

EXPORT_SYMBOL_GPL(exercise_device);

static int __init hello_init(void)
{
    int ret = 0;
    //static struct *pdev;

    printk(KERN_ALERT "\n Hello, %s\n", whom);
    ret = platform_device_register(&exercise_device);  //內已有 platform_device_add
 
    if (ret == 0) {
        ret = platform_driver_register(&exercise_driver);
        printk("platform_driver_register \n");
    }
    return ret;
}
module_init(hello_init);

static void __exit hello_exit(void)
{
    platform_driver_unregister(&exercise_driver);
    platform_device_unregister(&exercise_device);
    printk("platform_device_unregister  \n");
}
module_exit(hello_exit);


MODULE_AUTHOR("SamTsai");
MODULE_DESCRIPTION("device driver exercise");
MODULE_LICENSE("GPL");

