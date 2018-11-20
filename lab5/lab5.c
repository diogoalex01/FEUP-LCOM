// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include "interface.h"
#include "keyboard.h"
#include "i8042.h"

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int (video_test_init)(uint16_t mode, uint8_t delay) 
{
  vg_init(mode);
  sleep(delay);
  vg_exit();
  
  return 0;
}

int (video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                       uint16_t width, uint16_t height, uint32_t color)
{

  uint8_t irq_set, byte1[1], byte2[2];
  int ipc_status, r, size = 1;
  bool esc = true, make = true, wait = false;
  message msg;

  vg_init(mode);

  if (kbd_subscribe_int(&irq_set) != OK)
  {
    return 1;
  }
  draw_rectangle(x, y, width, color, height);

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
          if (msg.m_notify.interrupts & BIT(irq_set)) 
          { /* subscribed interrupt */
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
  }

  if (kbd_unsubscribe_int() != OK)
  {
    return 1;
  }
   vg_exit();

  return 0;
}

int (video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
 uint8_t irq_set, byte1[1], byte2[2];
  int ipc_status, r, size = 1;
  bool esc = true, make = true, wait = false;
  message msg;

  vg_init(mode);

  if (kbd_subscribe_int(&irq_set) != OK)
  {
    return 1;
  }
  draw_pattern(no_rectangles, first, step);

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
          if (msg.m_notify.interrupts & BIT(irq_set)) 
          { /* subscribed interrupt */
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
  }

  if (kbd_unsubscribe_int() != OK)
  {
    return 1;
  }
   vg_exit();

  return 0;
}
