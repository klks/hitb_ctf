#pragma once

struct VM_CPU
{
	unsigned long vm_reg1;
	unsigned long vm_reg2;
	unsigned long vm_reg3;
	unsigned long vm_reg4;
	unsigned long vm_reg5;
	unsigned long vm_reg6;
	unsigned long vm_reg7;
	unsigned long vm_reg8;
	unsigned long vm_reg9;
	unsigned long vm_reg10;
	unsigned long vm_reg11;
	unsigned long vm_reg12;
	unsigned long vm_reg13;
	unsigned long vm_reg14;
	unsigned long vm_reg15;
	unsigned long vm_reg16;
	//unsigned long vm_flags;
	unsigned long vm_esp;
	unsigned long vm_stack;
	unsigned long vm_stack_max;
	unsigned long vm_cmpbit;
	unsigned long vm_ip;
};

void VM_INIT();
void VM_CLEANUP();
void VM_ENTER();
void VM_PARSER();

//Pop instructions from ip
unsigned char VM_POP_INSTR_1();
unsigned short VM_POP_INSTR_2();
unsigned long VM_POP_INSTR_4();

//Internal functions
void VM_MOVZX_Process();
void VM_DEC_Process();
void VM_INC_Process();
void VM_OR_Process(unsigned char reg_dst, unsigned long value);
void VM_AND_Process(unsigned char reg_dst, unsigned long value);
void VM_SHR_Process(unsigned char reg_dst, unsigned long value);
void VM_SHL_Process(unsigned char reg_dst, unsigned long value);
void VM_SUB_Process(unsigned char reg_dst, unsigned long value);
void VM_CMP_Process(unsigned char reg_dst, unsigned long value);
void VM_ADD_Process(unsigned char reg_dst, unsigned long value);
void VM_XOR_Process(unsigned char reg_dst, unsigned long value);
void VM_POP_Process();
void VM_PUSH_Process();

void VM_GET_CMPBIT();
void SET_VM_REGISTER(unsigned char vm_reg_id, unsigned long value);
unsigned long GET_VM_REGISTER(unsigned char vm_reg_id);
void VM_CRITICAL_ERROR();

