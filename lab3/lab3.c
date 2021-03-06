#include <lcom/lcf.h>

#include "keyboard.h"
#include "i8042.h"
#include <lcom/timer.h>

#include <stdbool.h>
#include <stdint.h>

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

//FUNCTIONS
//////////////////////////////////////////////////////////////////

int (kbd_test_scan)(bool (assembly))
{
  uint8_t bit_no_kb;

  if (kbd_subscribe_int(&bit_no_kb) != OK)
  {
    return 1;
  }
  // printf("%d", irq_set);

  uint32_t irq_set_keyboard = BIT(bit_no_kb);

  uint8_t byte1[1], byte2[2];
  int ipc_status, r, size = 1;
  bool esc = true, make = true, wait = false;
  message msg;
 
  while(esc) 
  {   /* You may want to use a different condition */ 
      /* Get a request message. */ 
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 )
    {
      printf("driver_receive failed with: %d", r); 
      continue;
    }
    if (is_ipc_notify(ipc_status))
    { /* received notification */ 
      switch (_ENDPOINT_P(msg.m_source))
      {
        case HARDWARE: /* hardware interrupt notification */ 
          if (msg.m_notify.interrupts & irq_set_keyboard)
          { /* subscribed interrupt */

            if(!assembly)
            {
              kbc_ih();
            }
            else
            {
              kbc_asm_ih();
            }
            //printf("status: %x", status);

            if(error == true)
              continue;

            if(status == MSB)
            {
              wait = true;
              continue;
            } 

            if(wait == true)
            {
              wait = false;
              size = 2;
            }

            if(status == ESC_BK)
            {
              esc = false;
              make = false;
            }
            
            if((status >> 7) == BIT(0))
            {
              make = false;
            }

            if(size == 1)
            {
              byte1[0] = status;

              kbd_print_scancode(make, size, byte1);
            } 

            if (size == 2)
            {
              byte2[0] = MSB;
              byte2[1] = status;

              kbd_print_scancode(make, size, byte2);
            }
          }

          tickdelay(micros_to_ticks(DELAY_US));

          size = 1;
          make = true;

          break;
        default: 
          break; /* no other notifications expected: do nothing */ 
      } 
    } 
    else 
    { /* received a standard message, not a notification */ 
      /* no standard messages expected: do nothing */ 
    }
  }

  if (kbd_unsubscribe_int() != OK)
  {
    return 1;
  }

  if (!assembly)
  {
    kbd_print_no_sysinb(counter);
  }

  return 0;
}

//////////////////////////////////////////////////////////////////

int (kbd_test_poll)() 
{ 
  uint8_t byte1[1], byte2[2];
  bool esc = true, make = true, wait = false;
  int size = 1;

  while(esc)
  {
    if(kbd_scan_poll() == -1)
    {
      continue;
    } 

    if(status == MSB)
    {
      wait = true;
      continue;
    }

    if(wait == true)
    {
      wait = false;
      size = 2;
    }

    if(status == ESC_BK)
    {
      esc = false;
    }
            
    if((status >> 7) == BIT(0))
    {
      make = false;
    }

    if(size == 1)
    {
      byte1[0] = status;
      
      kbd_print_scancode(make, size, byte1);
    } 

    if (size == 2)
    {
      byte2[0] = MSB;
      byte2[1] = status;

      kbd_print_scancode(make, size, byte2);
    }

    size = 1;
    make = true;

    tickdelay(micros_to_ticks(DELAY_US));
  }

  kbd_print_no_sysinb(counter);
  kbd_poll_cmd();

  return 0;
}

//////////////////////////////////////////////////////////////////

int (kbd_test_timed_scan)(uint8_t n)
{
  if (n < 0)
  {
    return 1;
  }

  uint8_t bit_no_kb, bit_no_timer;

  if (kbd_subscribe_int(&bit_no_kb) != OK)
  {
    return 1;
  }
  // printf("%d", irq_set);
  if (timer_subscribe_int(&bit_no_timer) != OK)
  {
    return 1;
  }

  uint32_t irq_set_keyboard = BIT(bit_no_kb);
  uint32_t irq_set_timer = BIT(bit_no_timer);

  uint8_t byte1[1], byte2[2];
  int ipc_status, r, size = 1;
  bool esc = true, make = true, wait = false;
  message msg;
 
  while(esc && timer_counter < (n * (uint8_t) sys_hz()) )
  { 
   /* Get a request message. */ 
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 )
    {
      printf("driver_receive failed with: %d", r); 
      continue;
    }
    if (is_ipc_notify(ipc_status))
    { /* received notification */ 
      switch (_ENDPOINT_P(msg.m_source))
      {
        case HARDWARE: /* hardware interrupt notification */ 
          if (msg.m_notify.interrupts & irq_set_timer)
          {
              timer_int_handler();
          }
          if (msg.m_notify.interrupts & irq_set_keyboard) 
          { /* subscribed interrupt */ 

            timer_counter = 0; //resets the timer if another interrupt is needed

            kbc_ih();
          
            if(status == MSB)
            {
              wait = true;
              continue;
            } 

            if(wait == true)
            {
              wait = false;
              size = 2;
            }

            if(status == ESC_BK)
            {
              esc = false;
              make = false;
            }
            
            if((status >> 7) == BIT(0))
            {
              make = false;
            }

            if(size == 1)
            {
              byte1[0] = status;

              kbd_print_scancode(make, size, byte1);
            } 

            if (size == 2)
            {
              byte2[0] = MSB;
              byte2[1] = status;

              kbd_print_scancode(make, size, byte2);
            }
          }
          break;
        default:
          break; /* no other notifications expected: do nothing */ 
      } 
    } 
    else 
    { /* received a standard message, not a notification */ 
      /* no standard messages expected: do nothing */ 
    }

    size = 1;
    make = true;
  }

  if (kbd_unsubscribe_int() != OK)
  {  
    return 1;
  }

  if (timer_unsubscribe_int() != OK)
  {
    return 1;
  }
 
  kbd_print_no_sysinb(counter);

  return 0;
}
