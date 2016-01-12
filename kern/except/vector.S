.global intp_table

intp_table:
    NOP // reset
    NOP // undefined 
    B _SWI_handler
    NOP // prefetch abort
    NOP // data abort
    NOP // undefined
    B _IRQ_handler
    NOP // FIQ

_SWI_handler:
    STMFD SP, {R0-R14}  // store to SP - 4 to SP - 4 * 15, R14 to R0 respectively
    MRS R0, SPSR        
    STR R0, [SP, #-64]        // store SPSR to SP
    SUB R0, SP, #64          // pass SP to SWI_handler

    MRS R1, CPSR
    ORR R1, R1, #0x1F
    MSR CPSR, R1

    MOV R1, SP              // SP in user mode

    LDR SP, =#0x800FFFFC // FIXME, TODO: USE MARCO. HERE CREATE A NEW KERN STACK
    MOV FP, SP
    B SWI_handler

_IRQ_handler:
    SUB R14, R14, #4
    STMFD SP, {R0-R14}  // store to SP - 4 to SP - 4 * 15, R14 to R0 respectively
    MRS R0, SPSR        
    STR R0, [SP, #-64]        // store SPSR to SP
    SUB R0, SP, #64          // pass SP to SWI_handler

    MRS R1, CPSR
    ORR R1, R1, #0x1F
    MSR CPSR, R1
    
    MOV R1, SP              // SP in user mode

    LDR SP, =#0x800FFFFC // TODO: USE MARCO. HERE CREATE A NEW KERN STACK
    MOV FP, SP
    B IRQ_handler
