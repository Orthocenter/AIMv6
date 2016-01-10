.global intp_table

intp_table:
    NOP // reset
    NOP // undefined 
    B SWI_handler
    NOP // prefetch abort
    NOP // data abort
    B IRQ_handler
    NOP // FIQ

_SWI_handler:
    STMFD SP!, {R0-R12, R14}
    BL SWI_handler
    LDMFD SP!, {R0-R12, PC}^

_IRQ_handler:
    SUB R14, R14, #4
    STMFD SP!, {R0-R12, R14}
    BL IRQ_handler
    LDMFD SP!, {R0-R12, PC}^
