#include <stdlib.h>
#include <stdio.h>
#include "vm_opcodes.h"
#include "vm_processor.h"

//#define VM_DEBUG 1
struct VM_CPU my_cpu;

void VM_INIT()
{
	//Clear out registers
	my_cpu.vm_reg1 = my_cpu.vm_reg2 = my_cpu.vm_reg3 = my_cpu.vm_reg4 = my_cpu.vm_reg5 = my_cpu.vm_reg6 = my_cpu.vm_reg7 =\
	my_cpu.vm_reg8 = my_cpu.vm_reg9 = my_cpu.vm_reg10 = my_cpu.vm_reg11 = my_cpu.vm_reg12 = my_cpu.vm_reg13 = my_cpu.vm_reg14 =\
	my_cpu.vm_reg15 = my_cpu.vm_reg16 = 0;

	//Allocate a stack of 10kb
	my_cpu.vm_esp = my_cpu.vm_stack = (unsigned long) malloc(1024*10);
	my_cpu.vm_stack_max = my_cpu.vm_stack + (1024*10);
}

void VM_CLEANUP()
{
	free((void *)my_cpu.vm_stack);
}

void VM_ENTER()
{
    //Restore esp
    __asm__ __volatile__ ("pop %ebp\n");
    
    //Set vm_ip
    __asm__ __volatile__ ("pop %eax\n");
    __asm__ __volatile__ ("mov %%eax, %0\n" :"=m"(my_cpu.vm_ip));
    
    //Call VM_ENTER
    __asm__ __volatile__ ("call VM_PARSER\n");

    //Restore vm_ip
    __asm__ __volatile__ ("mov %0, %%eax\n" :"=m"(my_cpu.vm_ip));
    __asm__ __volatile__ ("push %eax\n");
    __asm__ __volatile__ ("ret\n");
}

void VM_CRITICAL_ERROR()
{
	perror("Reactor Core has become unstable...emergency shutdown activated!!\n");
    exit(-1);
}

unsigned char VM_POP_INSTR_1()
{
	unsigned char ret;
	unsigned char *ucp;
	ucp = (unsigned char *)my_cpu.vm_ip;
	ret = *ucp;
	my_cpu.vm_ip += 1;
	return ret;
}

unsigned short VM_POP_INSTR_2()
{
	unsigned short ret;
	unsigned char *ucp;
	ucp = (unsigned char *)my_cpu.vm_ip;
	ret = *ucp;
	ucp++;
	ret <<=8;
	ret += *ucp;
	my_cpu.vm_ip += 2;
	return ret;
}

unsigned long VM_POP_INSTR_4()
{
	unsigned long ret;
	unsigned char *ucp;
	ucp = (unsigned char *)my_cpu.vm_ip;
	ret = *ucp;
	ucp++;
	ret <<=8;
	ret += *ucp;
	ucp++;
	ret <<=8;
	ret += *ucp;
	ucp++;
	ret <<=8;
	ret += *ucp;
	my_cpu.vm_ip += 4;
	return ret;
}

void SET_VM_REGISTER(unsigned char vm_reg_id, unsigned long value)
{
		unsigned long *pRegs[] = {&my_cpu.vm_reg1, &my_cpu.vm_reg2, &my_cpu.vm_reg3, &my_cpu.vm_reg4, &my_cpu.vm_reg5,\
							 &my_cpu.vm_reg6, &my_cpu.vm_reg7, &my_cpu.vm_reg8, &my_cpu.vm_reg9, &my_cpu.vm_reg10,\
							 &my_cpu.vm_reg11, &my_cpu.vm_reg12, &my_cpu.vm_reg13, &my_cpu.vm_reg14, &my_cpu.vm_reg15,\
							 &my_cpu.vm_reg16};

	unsigned long *reg_offset;
	switch(vm_reg_id)
	{
	case OP_REG1:
		reg_offset = pRegs[0];
		break;
	case OP_REG2:
		reg_offset = pRegs[1];
		break;
	case OP_REG3:
		reg_offset = pRegs[2];
		break;
	case OP_REG4:
		reg_offset = pRegs[3];
		break;
	case OP_REG5:
		reg_offset = pRegs[4];
		break;
	case OP_REG6:
		reg_offset = pRegs[5];
		break;
	case OP_REG7:
		reg_offset = pRegs[6];
		break;
	case OP_REG8:
		reg_offset = pRegs[7];
		break;
	case OP_REG9:
		reg_offset = pRegs[8];
		break;
	case OP_REG10:
		reg_offset = pRegs[9];
		break;
	case OP_REG11:
		reg_offset = pRegs[10];
		break;
	case OP_REG12:
		reg_offset = pRegs[11];
		break;
	case OP_REG13:
		reg_offset = pRegs[12];
		break;
	case OP_REG14:
		reg_offset = pRegs[13];
		break;
	case OP_REG15:
		reg_offset = pRegs[14];
		break;
	case OP_REG16:
		reg_offset = pRegs[15];
		break;
	default:
		VM_CRITICAL_ERROR();
	}
	*reg_offset = value;
}

unsigned long GET_VM_REGISTER(unsigned char vm_reg_id)
{
	unsigned long *pRegs[] = {&my_cpu.vm_reg1, &my_cpu.vm_reg2, &my_cpu.vm_reg3, &my_cpu.vm_reg4, &my_cpu.vm_reg5,\
							 &my_cpu.vm_reg6, &my_cpu.vm_reg7, &my_cpu.vm_reg8, &my_cpu.vm_reg9, &my_cpu.vm_reg10,\
							 &my_cpu.vm_reg11, &my_cpu.vm_reg12, &my_cpu.vm_reg13, &my_cpu.vm_reg14, &my_cpu.vm_reg15,\
							 &my_cpu.vm_reg16};

	unsigned long *reg_offset;
	unsigned long reg_value = 0;
	switch(vm_reg_id)
	{
	case OP_REG1:
		reg_offset = pRegs[0];
		break;
	case OP_REG2:
		reg_offset = pRegs[1];
		break;
	case OP_REG3:
		reg_offset = pRegs[2];
		break;
	case OP_REG4:
		reg_offset = pRegs[3];
		break;
	case OP_REG5:
		reg_offset = pRegs[4];
		break;
	case OP_REG6:
		reg_offset = pRegs[5];
		break;
	case OP_REG7:
		reg_offset = pRegs[6];
		break;
	case OP_REG8:
		reg_offset = pRegs[7];
		break;
	case OP_REG9:
		reg_offset = pRegs[8];
		break;
	case OP_REG10:
		reg_offset = pRegs[9];
		break;
	case OP_REG11:
		reg_offset = pRegs[10];
		break;
	case OP_REG12:
		reg_offset = pRegs[11];
		break;
	case OP_REG13:
		reg_offset = pRegs[12];
		break;
	case OP_REG14:
		reg_offset = pRegs[13];
		break;
	case OP_REG15:
		reg_offset = pRegs[14];
		break;
	case OP_REG16:
		reg_offset = pRegs[15];
		break;
	default:
		VM_CRITICAL_ERROR();
	}
	reg_value = *reg_offset;
	return reg_value;
}

//OP_MOVZX.2		REG_DST.1		REG_SRC.1		MOV_COUNT.1
void VM_MOVZX_Process()
{
	unsigned char reg_dst = VM_POP_INSTR_1();
	unsigned char reg_src = VM_POP_INSTR_1();
	unsigned char byte_count = VM_POP_INSTR_1();

	if(byte_count <= 0 || byte_count > 4)
		VM_CRITICAL_ERROR();

	unsigned char *ucp = (unsigned char *) GET_VM_REGISTER(reg_src);
	unsigned long new_value = 0;
    int i;
	for(i=0; i<byte_count; i++)
	{
		new_value <<= 8;
		new_value += *ucp;
		ucp++;
	}
	SET_VM_REGISTER(reg_dst, new_value);
#ifdef VM_DEBUG
	printf("VM_MOVZX(REG_DST, 0x%.2X, REG_SRC, 0x%.2X, COUNT, %d, VALUE, 0x%.8x)\n", reg_dst, reg_src, byte_count, new_value);
#endif
}

//OP_INC.2		REG_DST.1
void VM_DEC_Process()
{
	unsigned char reg_dst = VM_POP_INSTR_1();
	unsigned long value = GET_VM_REGISTER(reg_dst);
	value--;
	SET_VM_REGISTER(reg_dst, value);
#ifdef VM_DEBUG
	printf("VM_INC(REG, 0x%.2X)\n", reg_dst);
#endif
}

//OP_INC.2		REG_DST.1
void VM_INC_Process()
{
	unsigned char reg_dst = VM_POP_INSTR_1();
	unsigned long value = GET_VM_REGISTER(reg_dst);
	value++;
	SET_VM_REGISTER(reg_dst, value);
#ifdef VM_DEBUG
	printf("VM_INC(REG, 0x%.2X)\n", reg_dst);
#endif
}

//Called from VM_Generic_Process()
//OP_OR.2		REG_DST.1		OP_REG.1		REG_SRC.1
//OP_OR.2		REG_DST.1		OP_CONST.1		VALUE.4
void VM_OR_Process(unsigned char reg_dst, unsigned long value)
{
	unsigned long new_value = GET_VM_REGISTER(reg_dst);
	new_value |= value;
	SET_VM_REGISTER(reg_dst, new_value);
#ifdef VM_DEBUG
	printf("VM_OR(REG, 0x%.2X, 0x%.8X)\n", reg_dst, value);
#endif
}

//Called from VM_Generic_Process()
//OP_AND.2		REG_DST.1		OP_REG.1		REG_SRC.1
//OP_AND.2		REG_DST.1		OP_CONST.1		VALUE.4
void VM_AND_Process(unsigned char reg_dst, unsigned long value)
{
	unsigned long new_value = GET_VM_REGISTER(reg_dst);
	new_value &= value;
	SET_VM_REGISTER(reg_dst, new_value);
#ifdef VM_DEBUG
	printf("VM_AND(REG, 0x%.2X, 0x%.8X)\n", reg_dst, value);
#endif
}

//Called from VM_Generic_Process()
//OP_SHR.2		REG_DST.1		OP_REG.1		REG_SRC.1
//OP_SHR.2		REG_DST.1		OP_CONST.1		VALUE.4
void VM_SHR_Process(unsigned char reg_dst, unsigned long value)
{
	unsigned long new_value = GET_VM_REGISTER(reg_dst);
	new_value >>= value;
	SET_VM_REGISTER(reg_dst, new_value);
#ifdef VM_DEBUG
	printf("VM_SHR(REG, 0x%.2X, 0x%.8X)\n", reg_dst, value);
#endif
}

//Called from VM_Generic_Process()
//OP_SHL.2		REG_DST.1		OP_REG.1		REG_SRC.1
//OP_SHL.2		REG_DST.1		OP_CONST.1		VALUE.4
void VM_SHL_Process(unsigned char reg_dst, unsigned long value)
{
	unsigned long new_value = GET_VM_REGISTER(reg_dst);
	new_value <<= value;
	SET_VM_REGISTER(reg_dst, new_value);
#ifdef VM_DEBUG
	printf("VM_SHL(REG, 0x%.2X, 0x%.8X)\n", reg_dst, value);
#endif
}

//Called from VM_Generic_Process()
//OP_SUB.2		REG_DST.1		OP_REG.1		REG_SRC.1
//OP_SUB.2		REG_DST.1		OP_CONST.1		VALUE.4
void VM_SUB_Process(unsigned char reg_dst, unsigned long value)
{
	unsigned long new_value = GET_VM_REGISTER(reg_dst);
	new_value -= value;
	SET_VM_REGISTER(reg_dst, new_value);
#ifdef VM_DEBUG
	printf("VM_SUB(REG, 0x%.2X, 0x%.8X)\n", reg_dst, value);
#endif
}

//OP_CMP.2		REG_DST.1		OP_REG.1		REG_TEST2.1
//OP_CMP.2		REG_DST.1		OP_CONST.1		VALUE.4
void VM_CMP_Process(unsigned char reg_dst, unsigned long value)
{
	unsigned long dst_value = GET_VM_REGISTER(reg_dst);

    __asm__ __volatile__ ("mov %0, %%eax\n" :"=m"(dst_value));
    __asm__ __volatile__ ("mov %0, %%ecx\n" :"=m"(value));
    __asm__ __volatile__ ("cmp %ecx, %eax\n");
    __asm__ __volatile__ ("pushf\n");
    __asm__ __volatile__ ("mov (%esp), %eax\n");
    __asm__ __volatile__ ("mov %%eax, %0\n" :"=m"(my_cpu.vm_cmpbit));
    __asm__ __volatile__ ("popf\n");
	/*__asm
	{
		mov eax, dst_value
		mov ecx, value
		cmp eax, ecx
		pushfd
		mov eax, dword ptr [esp]
		mov my_cpu.vm_cmpbit, eax
		popfd
	}*/
#ifdef VM_DEBUG
	printf("VM_CMP(REG, 0x%.2X, REG_VAL, 0x%.8x, 0x%.8X)\n", reg_dst, dst_value, value);
#endif
}

//Called from VM_Generic_Process()
//OP_ADD.2		REG_DST.1		OP_REG.1		REG_SRC.1
//OP_ADD.2		REG_DST.1		OP_CONST.1		VALUE.4
void VM_ADD_Process(unsigned char reg_dst, unsigned long value)
{
	unsigned long new_value = GET_VM_REGISTER(reg_dst);
	new_value += value;
	SET_VM_REGISTER(reg_dst, new_value);
#ifdef VM_DEBUG
	printf("VM_ADD(REG, 0x%.2X, 0x%.8X)\n", reg_dst, value);
#endif
}

//Called from VM_Generic_Process()
//OP_XOR.2		REG_DST.1		OP_REG.1		REG_SRC.1
//OP_XOR.2		REG_DST.1		OP_CONST.1		VALUE.4
void VM_XOR_Process(unsigned char reg_dst, unsigned long value)
{
	unsigned long new_value = GET_VM_REGISTER(reg_dst);
	new_value ^= value;
	SET_VM_REGISTER(reg_dst, new_value);
#ifdef VM_DEBUG
	printf("VM_XOR(REG, 0x%.2X, 0x%.8X)\n", reg_dst, value);
#endif
}

//OP_POP.2		REG_DST.1
void VM_POP_Process()
{
	unsigned char reg_dst;
	unsigned long pop_4;

	//Limit check
	if(my_cpu.vm_esp-4 < my_cpu.vm_stack)
		VM_CRITICAL_ERROR();

	reg_dst = VM_POP_INSTR_1();
	pop_4 = *((unsigned long *) my_cpu.vm_esp);
	my_cpu.vm_esp -= 4;
	SET_VM_REGISTER(reg_dst, pop_4);
#ifdef VM_DEBUG
	printf("VM_POP(REG, 0x%.2X, 0x%.8X)\n", reg_dst, pop_4);
#endif
}

//OP_PUSH.2		OP_CON.1		VALUE.4
//OP_PUSH.2		OP_REG.1		REG_SRC.1
void VM_PUSH_Process()
{
	unsigned char push_type;
	unsigned char reg_src;
	unsigned long push_4;

	push_type = VM_POP_INSTR_1();

	//Do a limit check to make sure that pushing to the stack dosent fuck up
	if(my_cpu.vm_esp+4 > my_cpu.vm_stack_max)
		VM_CRITICAL_ERROR();

#ifdef VM_DEBUG
	printf("VM_PUSH(");
#endif

	switch(push_type)
	{
	//Constant
	case OP_CONST:
		push_4 = VM_POP_INSTR_4();
#ifdef VM_DEBUG
			printf("0x%.8X)\n", push_4);
#endif
		break;

	//Register
	case OP_REG:
		reg_src = VM_POP_INSTR_1();
		push_4 = GET_VM_REGISTER(reg_src);
#ifdef VM_DEBUG
			printf("REG, 0x%.2X, 0x%.8X)\n", reg_src, push_4);
#endif
		break;

	//Offset
	//case OP_OFFSET:		//NOT_IMPLEMENTED
	//	break;
	default:
		VM_CRITICAL_ERROR();
	}

	my_cpu.vm_esp += 4;
	*((unsigned long *) my_cpu.vm_esp) = push_4;
}

//Handles
//OP_XXX.2		REG_DST.1		OP_REG.1		REG_SRC.1
//OP_XXX.2		REG_DST.1		OP_CONST.1		VALUE.4
//OP_XXX.2		REG_DST.1		OP_OFFSET.1		OFFSET_VALUE.5
void VM_Generic_Process(unsigned short OP_TYPE)
{
	unsigned char reg_dst, reg_src, type;
	unsigned long value;

	reg_dst = VM_POP_INSTR_1();
	type = VM_POP_INSTR_1();

	switch(type)
	{
	case OP_CONST:
		value = VM_POP_INSTR_4();
		break;
	case OP_REG:
		reg_src = VM_POP_INSTR_1();
		value = GET_VM_REGISTER(reg_src);
		break;
	case OP_OFFSET:
		VM_POP_INSTR_1();	//Ignore the first byte
		value = VM_POP_INSTR_4();
		value = (value>>24) | ((value<<8) & 0x00FF0000) | ((value>>8) & 0x0000FF00) | (value<<24);	//Endian swap
		break;
	default:
		VM_CRITICAL_ERROR();
	}

	switch(OP_TYPE)
	{
	case OP_AND:
		VM_AND_Process(reg_dst, value);
		break;
	case OP_SHR:
		VM_SHR_Process(reg_dst, value);
		break;
	case OP_SHL:
		VM_SHL_Process(reg_dst, value);
		break;
	case OP_SUB:
		VM_SUB_Process(reg_dst, value);
	case OP_CMP:
		VM_CMP_Process(reg_dst, value);
		break;
	case OP_XOR:
		VM_XOR_Process(reg_dst, value);
		break;
	case OP_ADD:
		VM_ADD_Process(reg_dst, value);
		break;
	case OP_MOV:
		SET_VM_REGISTER(reg_dst, value);
#ifdef VM_DEBUG
	printf("VM_MOV(REG, 0x%.2X, 0x%.8X)\n", reg_dst, value);
#endif
		break;
	default:
#ifdef VM_DEBUG
			printf("VM_Generic_Process Unimplemented instruction 0x%.4x", OP_TYPE);
#endif
		VM_CRITICAL_ERROR();
	}
}

void VM_PARSER()
{
	unsigned short opcode;
	int bExitLoop = 1;

	while(bExitLoop != 0)
	{
		//Read 2 bytes from IP
		opcode = VM_POP_INSTR_2();
		switch(opcode)
		{
		case OP_VM_EXIT:
#ifdef VM_DEBUG
			printf("VM_EXIT()\n");
#endif
			bExitLoop = 0;
			break;

		case OP_PUSH:
			VM_PUSH_Process();
			break;

		case OP_POP:
			VM_POP_Process();
			break;

		case OP_DEC:
			VM_DEC_Process();
			break;

		case OP_INC:
			VM_INC_Process();
			break;

		case OP_MOVZX:
			VM_MOVZX_Process();
			break;

		case OP_AND:
		case OP_OR:
		case OP_CMP:
		case OP_SUB:
		case OP_MOV:
		case OP_ADD:
		case OP_SHR:
		case OP_SHL:
		case OP_XOR:
			VM_Generic_Process(opcode);
			break;

		default:
#ifdef VM_DEBUG
			printf("Unimplemented instruction 0x%.4x", opcode);
#endif
			VM_CRITICAL_ERROR();
		}
	}
}

void VM_GET_CMPBIT()
{
    //Restore esp
    __asm__ __volatile__ ("pop %ebp\n");
    
    __asm__ __volatile__ ("mov %0, %%eax\n" :"=m"(my_cpu.vm_cmpbit));
    __asm__ __volatile__ ("push %eax\n");
    __asm__ __volatile__ ("popf\n");
    __asm__ __volatile__ ("ret\n");
/*
	__asm
	{
		mov eax, my_cpu.vm_cmpbit
		push eax
		popfd
		ret
	}
*/
}

//**************************************************************************************************
//
// VM General functions
//
//**************************************************************************************************
int vm_strlen(char *in_str)
{
	unsigned long pin_str = (unsigned long)in_str;
	SET_VM_REGISTER(OP_REG12, pin_str);

        VM_ENTER();
		VM_MOV VM_REG13 VM_REG VM_REG12
		VM_EXIT
__asm__ __volatile__("strlen_loop:");
		VM_ENTER();
		VM_MOVZX VM_REG14 VM_REG13 __asm__ __volatile__(".byte 0x01\n");
		VM_INC VM_REG13
		VM_CMP VM_REG14 VM_CONST __asm__ __volatile__(".byte 0x00\n .byte 0x00\n .byte 0x00\n .byte 0x00");
		VM_EXIT
		__asm__ __volatile__("call VM_GET_CMPBIT\n");
		__asm__ __volatile__("jnz strlen_loop");

		VM_ENTER();
		VM_SUB VM_REG13 VM_REG VM_REG12
		VM_DEC VM_REG13
		VM_EXIT

#ifdef VM_DEBUG
	printf("vm_strlen returned %d\n", GET_VM_REGISTER(OP_REG13));
#endif
	return GET_VM_REGISTER(OP_REG13);
}