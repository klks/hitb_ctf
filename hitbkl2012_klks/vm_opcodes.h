#pragma once

//Exiting Virtual Machine
#define VM_EXIT							__asm__ __volatile__ (".byte 0x03\n .byte 0x17\n");

//Helps with defining size
#define OP_SIZE_1				0x01
#define VM_SIZE_1						__asm__ __volatile__ (".byte 0x01\n");

#define OP_SIZE_2				0x02
#define VM_SIZE_2						__asm__ __volatile__ (".byte 0x02\n");

#define OP_SIZE_4				0x04
#define VM_SIZE_4						__asm__ __volatile__ (".byte 0x04\n");

//Defines if the next bytes are a constant, register or offset
#define OP_CONST				0x00
#define VM_CONST						__asm__ __volatile__ (".byte 0x00\n");

#define OP_REG					0x01
#define VM_REG							__asm__ __volatile__ (".byte 0x01\n");

#define OP_OFFSET				0x02
#define VM_OFFSET						__asm__ __volatile__ (".byte 0x02\n");

//Comparison Operators
#define OP_CMP_EQ				0xFE
#define OP_CMP_NE				0xAE

//CPU Registers
#define OP_REG1					0x00
#define VM_REG1							__asm__ __volatile__ (".byte 0x00\n");

#define OP_REG2					0x01
#define VM_REG2							__asm__ __volatile__ (".byte 0x01\n");

#define OP_REG3					0x02
#define VM_REG3							__asm__ __volatile__ (".byte 0x02\n");

#define OP_REG4					0x03
#define VM_REG4							__asm__ __volatile__ (".byte 0x03\n");

#define OP_REG5					0x04
#define VM_REG5							__asm__ __volatile__ (".byte 0x04\n");

#define OP_REG6					0x05
#define VM_REG6							__asm__ __volatile__ (".byte 0x05\n");

#define OP_REG7					0x06
#define VM_REG7							__asm__ __volatile__ (".byte 0x06\n");

#define OP_REG8					0x07
#define VM_REG8							__asm__ __volatile__ (".byte 0x07\n");

#define OP_REG9					0x08
#define VM_REG9							__asm__ __volatile__ (".byte 0x08\n");

#define OP_REG10				0x09
#define VM_REG10						__asm__ __volatile__ (".byte 0x09\n");

#define OP_REG11				0x0A
#define VM_REG11						__asm__ __volatile__ (".byte 0x0a\n");

#define OP_REG12				0x0B
#define VM_REG12						__asm__ __volatile__ (".byte 0x0b\n");

#define OP_REG13				0x0C
#define VM_REG13						__asm__ __volatile__ (".byte 0x0c\n");

#define OP_REG14				0x0D
#define VM_REG14						__asm__ __volatile__ (".byte 0x0d\n");

#define OP_REG15				0x0E
#define VM_REG15						__asm__ __volatile__ (".byte 0x0e\n");

#define OP_REG16				0x0F
#define VM_REG16						__asm__ __volatile__ (".byte 0x0f\n");

//Raw opcode definition
#define OP_MOV					0x0100
#define VM_MOV							__asm__ __volatile__ (".byte 0x01\n .byte 0x00\n");
#define OP_MOVZX				0x0101
#define VM_MOVZX						__asm__ __volatile__ (".byte 0x01\n .byte 0x01\n");

//Arithmetic
#define OP_SHR					0x0200
#define VM_SHR							__asm__ __volatile__ (".byte 0x02\n .byte 0x00\n");

#define OP_SHL					0x0201
#define VM_SHL							__asm__ __volatile__ (".byte 0x02\n .byte 0x01\n");

#define OP_ROR					0x0202
#define OP_ROL					0x0203

#define OP_ADD					0x0204
#define VM_ADD							__asm__ __volatile__ (".byte 0x02\n .byte 0x04\n");

#define OP_SUB					0x0205
#define VM_SUB							__asm__ __volatile__ (".byte 0x02\n .byte 0x05\n");

#define OP_XOR					0x0206
#define VM_XOR							__asm__ __volatile__ (".byte 0x02\n .byte 0x06\n");

#define OP_DIV					0x0207
#define OP_IDIV					0x0208
#define OP_MUL					0x0209
#define OP_IMUL					0x020A
#define OP_MOD					0x020B

#define OP_AND					0x020C
#define VM_AND							__asm__ __volatile__ (".byte 0x02\n .byte 0x0C\n");

#define OP_OR					0x020D
#define VM_OR							__asm__ __volatile__ (".byte 0x02\n .byte 0x0D\n");

//VM Related
#define OP_VM_EXIT				0x0317

//Stack
#define OP_PUSH					0x0400
#define VM_PUSH							__asm__ __volatile__ (".byte 0x04\n .byte 0x00\n");
#define OP_POP					0x0401
#define VM_POP							__asm__ __volatile__ (".byte 0x04\n .byte 0x01\n");

//JXX
#define OP_JMP					0x0500

//INC DEC
#define OP_INC					0x0600
#define VM_INC							__asm__ __volatile__ (".byte 0x06\n .byte 0x00\n");
#define OP_DEC					0x0601
#define VM_DEC							__asm__ __volatile__ (".byte 0x06\n .byte 0x01\n");

//CMP
#define OP_CMP					0x0700
#define VM_CMP							__asm__ __volatile__ (".byte 0x07\n .byte 0x00\n");

//NOP
#define OP_NOP					0x0909
