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
// #include <mach/hardware.h>
//#include <plat/gpio-cfg.h>


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

static int  hello_probe(struct platform_device *pdev)
{
    int i=0, ret = 0;
    printk("hello_probe\n");
    for (i = 0; i < ARRAY_SIZE(hello_adv_group); i++) {
        ret = device_create_file(&pdev->dev, hello_adv_group[i]);
        if (ret) {
            dev_err(&pdev->dev, "failed: sysfs file %s\n",
                    hello_adv_group[i]->attr.name);
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

static struct platform_driver hello_driver = {
    .probe        = hello_probe,
    .remove        = __exit_p(hello_remove),
    .driver        = {
        .name    = "hello",
        .owner    = THIS_MODULE,
    },
    .suspend    = hello_suspend,
    .resume = hello_resume,
};

static struct platform_device hello_device = {
    .name = "hello",
    .dev = {
        //.platform_data = &hello_data,
    }   
};

static int __init hello_init(void)
{
    int ret = 0;


    ret = platform_device_register(&hello_device);
    printk("platform_device_register  \n");
    
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
}
module_exit(hello_exit);


MODULE_AUTHOR("<guangyaw.twbbs.org>");
MODULE_DESCRIPTION("hello driver");
MODULE_LICENSE("GPL");

