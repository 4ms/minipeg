/**
  ******************************************************************************
  * @file      startup_stm32f413xx.s
  * @author    MCD Application Team
  * @brief     STM32F413xx Devices vector table for GCC based toolchains. 
  *            This module performs:
  *                - Set the initial SP
  *                - Set the initial PC == Reset_Handler,
  *                - Set the vector table entries with the exceptions ISR address
  *                - Branches to main in the C library (which eventually
  *                  calls main()).
  *            After Reset the Cortex-M4 processor is in Thread mode,
  *            priority is Privileged, and the Stack is set to Main.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
    
  .syntax unified
  .cpu cortex-m4
  .fpu softvfp
  .thumb

.global  g_pfnVectors
.global  Default_Handler

/* start address for the initialization values of the .data section. 
defined in linker script */
.word  _sidata
/* start address for the .data section. defined in linker script */
.word  _sdata
/* end address for the .data section. defined in linker script */
.word  _edata
/* start address for the .bss section. defined in linker script */
.word  _sbss
/* end address for the .bss section. defined in linker script */
.word  _ebss
/* stack used for SystemInit_ExtMemCtl; always internal RAM used */

/**
 * @brief  This is the code that gets called when the processor first
 *          starts execution following a reset event. Only the absolutely
 *          necessary set is performed, after which the application
 *          supplied main() routine is called. 
 * @param  None
 * @retval : None
*/

    .section  .text.Reset_Handler
  .weak  Reset_Handler
  .type  Reset_Handler, %function
Reset_Handler:  
  ldr   sp, =_estack       /* set stack pointer */

/* Copy the data segment initializers from flash to SRAM */
  ldr r0, =_sdata
  ldr r1, =_edata
  ldr r2, =_sidata
  movs r3, #0
  b LoopCopyDataInit

CopyDataInit:
  ldr r4, [r2, r3]
  str r4, [r0, r3]
  adds r3, r3, #4

LoopCopyDataInit:
  adds r4, r0, r3
  cmp r4, r1
  bcc CopyDataInit
  
/* Zero fill the bss segment. */
  ldr r2, =_sbss
  ldr r4, =_ebss
  movs r3, #0
  b LoopFillZerobss

FillZerobss:
  str  r3, [r2]
  adds r2, r2, #4

LoopFillZerobss:
  cmp r2, r4
  bcc FillZerobss

/* Call the clock system intitialization function.*/
  bl  SystemInit   
/* Call static constructors */
    bl __libc_init_array
/* Call the application's entry point.*/
  bl  main
  bx  lr    
.size  Reset_Handler, .-Reset_Handler

/**
 * @brief  This is the code that gets called when the processor receives an 
 *         unexpected interrupt.  This simply enters an infinite loop, preserving
 *         the system state for examination by a debugger.
 * @param  None     
 * @retval None       
*/
    .section  .text.Default_Handler,"ax",%progbits
Default_Handler:
Infinite_Loop:
  b  Infinite_Loop
  .size  Default_Handler, .-Default_Handler
/******************************************************************************
*
* The minimal vector table for a Cortex M3. Note that the proper constructs
* must be placed on this to ensure that it ends up at physical address
* 0x0000.0000.
* 
*******************************************************************************/
   .section  .isr_vector,"a",%progbits
  .type  g_pfnVectors, %object
  .size  g_pfnVectors, .-g_pfnVectors
    
g_pfnVectors:
  .word  _estack
  .word  Reset_Handler
  .word  NMI_Handler
  .word  HardFault_Handler
  .word  MemManage_Handler
  .word  BusFault_Handler
  .word  UsageFault_Handler
  .word  0
  .word  0
  .word  0
  .word  0
  .word  SVC_Handler
  .word  DebugMon_Handler
  .word  0
  .word  PendSV_Handler
  .word  SysTick_Handler

  /* External Interrupts */
  .word IRQ_Trampoline_0
  .word IRQ_Trampoline_1
  .word IRQ_Trampoline_2
  .word IRQ_Trampoline_3
  .word IRQ_Trampoline_4
  .word IRQ_Trampoline_5
  .word IRQ_Trampoline_6
  .word IRQ_Trampoline_7
  .word IRQ_Trampoline_8
  .word IRQ_Trampoline_9
  .word IRQ_Trampoline_10
  .word IRQ_Trampoline_11
  .word IRQ_Trampoline_12
  .word IRQ_Trampoline_13
  .word IRQ_Trampoline_14
  .word IRQ_Trampoline_15
  .word IRQ_Trampoline_16
  .word IRQ_Trampoline_17
  .word IRQ_Trampoline_18
  .word IRQ_Trampoline_19
  .word IRQ_Trampoline_20
  .word IRQ_Trampoline_21
  .word IRQ_Trampoline_22
  .word IRQ_Trampoline_23
  .word IRQ_Trampoline_24
  .word IRQ_Trampoline_25
  .word IRQ_Trampoline_26
  .word IRQ_Trampoline_27
  .word IRQ_Trampoline_28
  .word IRQ_Trampoline_29
  .word IRQ_Trampoline_30
  .word IRQ_Trampoline_31
  .word IRQ_Trampoline_32
  .word IRQ_Trampoline_33
  .word IRQ_Trampoline_34
  .word IRQ_Trampoline_35
  .word IRQ_Trampoline_36
  .word IRQ_Trampoline_37
  .word IRQ_Trampoline_38
  .word IRQ_Trampoline_39
  .word IRQ_Trampoline_40
  .word IRQ_Trampoline_41
  .word IRQ_Trampoline_42
  .word IRQ_Trampoline_43
  .word IRQ_Trampoline_44
  .word IRQ_Trampoline_45
  .word IRQ_Trampoline_46
  .word IRQ_Trampoline_47
  .word IRQ_Trampoline_48
  .word IRQ_Trampoline_49
  .word IRQ_Trampoline_50
  .word IRQ_Trampoline_51
  .word IRQ_Trampoline_52
  .word IRQ_Trampoline_53
  .word IRQ_Trampoline_54
  .word IRQ_Trampoline_55
  .word IRQ_Trampoline_56
  .word IRQ_Trampoline_57
  .word IRQ_Trampoline_58
  .word IRQ_Trampoline_59
  .word IRQ_Trampoline_60
  .word IRQ_Trampoline_61
  .word IRQ_Trampoline_62
  .word IRQ_Trampoline_63
  .word IRQ_Trampoline_64
  .word IRQ_Trampoline_65
  .word IRQ_Trampoline_66
  .word IRQ_Trampoline_67
  .word IRQ_Trampoline_68
  .word IRQ_Trampoline_69
  .word IRQ_Trampoline_70
  .word IRQ_Trampoline_71
  .word IRQ_Trampoline_72
  .word IRQ_Trampoline_73
  .word IRQ_Trampoline_74
  .word IRQ_Trampoline_75
  .word IRQ_Trampoline_76
  .word IRQ_Trampoline_77
  .word IRQ_Trampoline_78
  .word IRQ_Trampoline_79
  .word IRQ_Trampoline_80
  .word IRQ_Trampoline_81
  .word IRQ_Trampoline_82
  .word IRQ_Trampoline_83
  .word IRQ_Trampoline_84
  .word IRQ_Trampoline_85
  .word IRQ_Trampoline_86
  .word IRQ_Trampoline_87
  .word IRQ_Trampoline_88
  .word IRQ_Trampoline_89
  .word IRQ_Trampoline_90
  .word IRQ_Trampoline_91
  .word IRQ_Trampoline_92
  .word IRQ_Trampoline_93
  .word IRQ_Trampoline_94
  .word IRQ_Trampoline_95
  .word IRQ_Trampoline_96
  .word IRQ_Trampoline_97
  .word IRQ_Trampoline_98
  .word IRQ_Trampoline_99
  .word IRQ_Trampoline_100
  .word IRQ_Trampoline_101

; /*******************************************************************************
; *
; * Provide weak aliases for each Exception handler to the Default_Handler. 
; * As they are weak aliases, any function with the same name will override 
; * this definition.
; *
; *******************************************************************************/
   .weak      NMI_Handler
   .thumb_set NMI_Handler,Default_Handler

   .weak      HardFault_Handler
   .thumb_set HardFault_Handler,Default_Handler

   .weak      MemManage_Handler
   .thumb_set MemManage_Handler,Default_Handler
  
   .weak      BusFault_Handler
   .thumb_set BusFault_Handler,Default_Handler

   .weak      UsageFault_Handler
   .thumb_set UsageFault_Handler,Default_Handler

   .weak      SVC_Handler
   .thumb_set SVC_Handler,Default_Handler

   .weak      DebugMon_Handler
   .thumb_set DebugMon_Handler,Default_Handler

   .weak      PendSV_Handler
   .thumb_set PendSV_Handler,Default_Handler

   .weak      SysTick_Handler
   .thumb_set SysTick_Handler,Default_Handler

