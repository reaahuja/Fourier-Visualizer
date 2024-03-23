#include <stdio.h>
int test = 6;
#define KEY_BASE 0xFF200050
volatile int* keyPtr = (int*)KEY_BASE;
// add contents of  "nios2_ctrl_reg_macros.h"
#define NIOS2_READ_STATUS(dest) \
  do {                          \
    dest = __builtin_rdctl(0);  \
  } while (0)
#define NIOS2_WRITE_STATUS(src) \
  do {                          \
    __builtin_wrctl(0, src);    \
  } while (0)
#define NIOS2_READ_ESTATUS(dest) \
  do {                           \
    dest = __builtin_rdctl(1);   \
  } while (0)
#define NIOS2_READ_BSTATUS(dest) \
  do {                           \
    dest = __builtin_rdctl(2);   \
  } while (0)
#define NIOS2_READ_IENABLE(dest) \
  do {                           \
    dest = __builtin_rdctl(3);   \
  } while (0)
#define NIOS2_WRITE_IENABLE(src) \
  do {                           \
    __builtin_wrctl(3, src);     \
  } while (0)
#define NIOS2_READ_IPENDING(dest) \
  do {                            \
    dest = __builtin_rdctl(4);    \
  } while (0)
#define NIOS2_READ_CPUID(dest) \
  do {                         \
    dest = __builtin_rdctl(5); \
  } while (0)

int main(void);
void interrupt_handler(void);
void interval_timer_ISR(void);
void pushbutton_ISR(void);

/* The assembly language code below handles CPU reset processing */
void the_reset(void) __attribute__((section(".reset")));
void the_reset(void)
/*******************************************************************************
 * Reset code. By giving the code a section attribute with the name ".reset" we
 * allow the linker program to locate this code at the proper reset vector
 * address. This code just calls the main program.
 ******************************************************************************/
{
  __asm__(".set noat");    /* Instruct the assembler NOT to use reg at (r1) as
                            * a temp register for performing optimizations */
  __asm__(".set nobreak"); /* Suppresses a warning message that says that
                            * some debuggers corrupt regs bt (r25) and ba
                            * (r30)
                            */
  __asm__("movia r2, main");  // Call the C language main program
  __asm__("jmp r2");
}
/* The assembly language code below handles CPU exception processing. This
 * code should not be modified; instead, the C language code in the function
 * interrupt_handler() can be modified as needed for a given application.
 */
void the_exception(void) __attribute__((section(".exceptions")));
void the_exception(void)
/*******************************************************************************
 * Exceptions code. By giving the code a section attribute with the name
 * ".exceptions" we allow the linker program to locate this code at the proper
 * exceptions vector address.
 * This code calls the interrupt handler and later returns from the exception.
 ******************************************************************************/
{
  __asm__("subi sp, sp, 128");
  __asm__("stw et, 96(sp)");
  __asm__("rdctl et, ctl4");
  __asm__("beq et, r0, SKIP_EA_DEC");  // Interrupt is not external
  __asm__("subi ea, ea, 4");           /* Must decrement ea by one instruction
                                        * for external interupts, so that the
                                        * interrupted instruction will be run */
  __asm__("SKIP_EA_DEC:");
  __asm__("stw r1, 4(sp)");  // Save all registers
  __asm__("stw r2, 8(sp)");
  __asm__("stw r3, 12(sp)");
  __asm__("stw r4, 16(sp)");
  __asm__("stw r5, 20(sp)");
  __asm__("stw r6, 24(sp)");
  __asm__("stw r7, 28(sp)");
  __asm__("stw r8, 32(sp)");
  __asm__("stw r9, 36(sp)");
  __asm__("stw r10, 40(sp)");
  __asm__("stw r11, 44(sp)");
  __asm__("stw r12, 48(sp)");
  __asm__("stw r13, 52(sp)");
  __asm__("stw r14, 56(sp)");
  __asm__("stw r15, 60(sp)");
  __asm__("stw r16, 64(sp)");
  __asm__("stw r17, 68(sp)");
  __asm__("stw r18, 72(sp)");
  __asm__("stw r19, 76(sp)");
  __asm__("stw r20, 80(sp)");
  __asm__("stw r21, 84(sp)");
  __asm__("stw r22, 88(sp)");
  __asm__("stw r23, 92(sp)");
  __asm__("stw r25, 100(sp)");  // r25 = bt (skip r24 = et, because it is saved
  // above)
  __asm__("stw r26, 104(sp)");  // r26 = gp
  // skip r27 because it is sp, and there is no point in saving this
  __asm__("stw r28, 112(sp)");  // r28 = fp
  __asm__("stw r29, 116(sp)");  // r29 = ea
  __asm__("stw r30, 120(sp)");  // r30 = ba
  __asm__("stw r31, 124(sp)");  // r31 = ra
  __asm__("addi fp, sp, 128");
  __asm__("call interrupt_handler");  // Call the C language interrupt handler
  __asm__("ldw r1, 4(sp)");           // Restore all registers
  __asm__("ldw r2, 8(sp)");
  __asm__("ldw r3, 12(sp)");
  __asm__("ldw r4, 16(sp)");
  __asm__("ldw r5, 20(sp)");
  __asm__("ldw r6, 24(sp)");
  __asm__("ldw r7, 28(sp)");
  __asm__("ldw r8, 32(sp)");
  __asm__("ldw r9, 36(sp)");
  __asm__("ldw r10, 40(sp)");
  __asm__("ldw r11, 44(sp)");
  __asm__("ldw r12, 48(sp)");
  __asm__("ldw r13, 52(sp)");
  __asm__("ldw r14, 56(sp)");
  __asm__("ldw r15, 60(sp)");
  __asm__("ldw r16, 64(sp)");
  __asm__("ldw r17, 68(sp)");
  __asm__("ldw r18, 72(sp)");
  __asm__("ldw r19, 76(sp)");
  __asm__("ldw r20, 80(sp)");
  __asm__("ldw r21, 84(sp)");
  __asm__("ldw r22, 88(sp)");
  __asm__("ldw r23, 92(sp)");
  __asm__("ldw r24, 96(sp)");
  __asm__("ldw r25, 100(sp)");  // r25 = bt
  __asm__("ldw r26, 104(sp)");  // r26 = gp
  // skip r27 because it is sp, and we did not save this on the stack
  __asm__("ldw r28, 112(sp)");  // r28 = fp
  __asm__("ldw r29, 116(sp)");  // r29 = ea
  __asm__("ldw r30, 120(sp)");  // r30 = ba
  __asm__("ldw r31, 124(sp)");  // r31 = ra
  __asm__("addi sp, sp, 128");
  __asm__("eret");
}

void interrupt_handler(void) {
  int ipending;
  NIOS2_READ_IPENDING(ipending);
  if (ipending & 0x1)  // interval timer is interrupt level 0
  {
    interval_timer_ISR();
  }
  if (ipending & 0x2)  // pushbuttons are interrupt level 1
  {
    pushbutton_ISR();
    test = 5;
  }
  // else, ignore the interrupt
  *(keyPtr + 3) = 0xf;
  return;
}

void interval_timer_ISR(void) { printf("isr interval reached!"); }

void pushbutton_ISR(void) { return; }

int main() {
  *(keyPtr + 2) = 0xf;  // clear mask and edge cap
  NIOS2_WRITE_IENABLE(0b11);
  NIOS2_WRITE_STATUS(0b1);
  printf("Hello");
  return 0;
}