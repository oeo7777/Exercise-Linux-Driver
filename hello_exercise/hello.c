#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

static char *whom = "world";
static int howmany = 1;
module_param(howmany, int, S_IRUGO); //此就算沒給 write 權限 (S_IRUGO|S_IWUSR) 一樣可輸入值
module_param(whom, charp, S_IRUGO);  //或是在 module_param_array(name,type,num,perm); 才限制比較嚴格呢 ?

static int hello_init(void)
{
	int i, len;
	len = (howmany <=0) ? (1):(howmany);
	for(i=0; i<howmany; i++)
		printk(KERN_ALERT "Hello, %s\n", whom);
	return 0;
}

static void hello_exit(void)
{
	printk(KERN_ALERT "Goodbye, cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Sam Tsai");
MODULE_DESCRIPTION("Hello driver");
