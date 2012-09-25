////////////////////////////////////////////////////////////////////////////////
///@file    keyboard.cpp
///@short   Keyboard driver
///@author  J.Nider
///@date    11-Jun-2007 03:50:03 PM
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include "kstdio.h"
#include "kvideo.h"
#include "isr.h"  // for RegisterIrqObserver
#include "command.h" // for ProcessCommand
#include "keyboard.h"
//#include "klist.h"
#include "ktask.h"
#include "kmessage.h"

#define KB_FLAG_KEY_RELEASED 0x80   // set when a key is released

#define SCANCODE_LEFT_SHIFT   42
#define SCANCODE_RIGHT_SHIFT  54
#define SCANCODE_ALT          56

static const unsigned char kbdus[128] =
{
    0,  27, '1', '2',  '3', '4', '5', '6', '7', '8',   /* 9 */
  '9', '0', '-', '=', '\b','\t', 'q', 'w', 'e', 'r',   /* 19 */
  't', 'y', 'u', 'i',  'o', 'p', '[', ']', '\n',  0,   /* 29   - Control */
  'a', 's', 'd', 'f',  'g', 'h', 'j', 'k', 'l', ';',   /* 39 */
 '\'', '`',   0, '\\', 'z', 'x', 'c', 'v', 'b', 'n',   /* 49 */
  'm', ',', '.', '/',   0,  '*',   0, ' ',
    0,   /* Caps lock */
    0,   /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   /* < ... F10 */
    0,   /* 69 - Num lock*/
    0,   /* Scroll Lock */
    0,   /* Home key */
    0,   /* Up Arrow */
    0,   /* Page Up */
  '-',
    0,   /* Left Arrow */
    0,
    0,   /* Right Arrow */
  '+',
    0,   /* 79 - End key*/
    0,   /* Down Arrow */
    0,   /* Page Down */
    0,   /* Insert Key */
    0,   /* Delete Key */
    0,   0,   0,
    0,   /* F11 Key */
    0,   /* F12 Key */
    0,   /* All other keys are undefined */
};


//static KPtrList<KeypressObserver> observerList;
static unsigned int observer = 0;

void kb_handler(regs_t* pRegs);

void __registerForKeyPress(unsigned int taskID)
{
   __ASM("add $4, %ebp\n");
   k_registerForKeyPress(taskID);
   __ASM("sub $4, %ebp\n");
   __KERNEL_RETURN();
}

void k_registerForKeyPress(unsigned int taskID)
{
   observer = taskID;
}

//////////
///@author  J.Nider
///@date    11-Jun-2007 03:50:54 PM
///@brief   Register the keyboard handler as an IRQ observer
//////////
void keyboard_install()
{
   ISR_RegisterIRQHandler(1, kb_handler);
}

//////////
///@author  J.Nider
///@date    11-Jun-2007 03:50:31 PM
///@brief   Called when the keyboard interrupt is called
//////////
/*
void kb_handler(regs_t* pRegs)
{
   unsigned char scancode = inportb(0x60);
   
   //k_printf("kb_handler\n");
   //while(1);
   // save the current task so all memory allocations are done in the kernel tables
   task_t* pCurrentTask = k_getCurrentTask();
   k_setCurrentTask(&osTask);
   
   // we are only interested in 'key down' events
   if (!(scancode & KB_FLAG_KEY_RELEASED))
   {
      char key = kbdus[scancode];

      // echo the key on the screen
      //k_putchar(key);

      //k_printf("handling key %c\n", key);

      if (observer)
      {
         //k_printf("posting message\n");
         //while(1);
         k_postMessage(observer, MESSAGE_KEY_PRESSED, key, 0);
         //while(1);
      }
         //observer(key, 0);
      
      for (unsigned int i=0; i < observerList.Count(); i++)
      {
         KeypressObserver* pObserver = observerList[i];
         (*pObserver)(key, 0);
      }
      
   }

   // restore saved task
   k_setCurrentTask(pCurrentTask);
   
   _EOI_MASTER();
}
*/

