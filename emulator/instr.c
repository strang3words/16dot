#include "instr.h"
#include "type.h"
#include "isa.h"
#include "log.h"
#include "cpu.h"
#include <stdlib.h>

i8 instr_nop(struct cpu *cpu, u8 nib1, u8 nib2, u8 nib3)
{
	return 0;
}

i8 instr_add(struct cpu *cpu, u8 nib1, u8 nib2, u8 nib3)
{
	cpu->reg[nib1] = cpu->reg[nib2] + cpu->reg[nib3];
	return 0;
}

i8 instr_sub(struct cpu *cpu, u8 nib1, u8 nib2, u8 nib3)
{
	cpu->reg[nib1] = cpu->reg[nib2] - cpu->reg[nib3];
	return 0;
}

i8 instr_mul(struct cpu *cpu, u8 nib1, u8 nib2, u8 nib3)
{
	cpu->reg[nib1] = cpu->reg[nib2] * cpu->reg[nib3];
	return 0;
}

i8 instr_div(struct cpu *cpu, u8 nib1, u8 nib2, u8 nib3)
{
	cpu->reg[nib1] = cpu->reg[nib2] / cpu->reg[nib3];
	return 0;
}

i8 instr_slt(struct cpu *cpu, u8 nib1, u8 nib2, u8 nib3)
{
	cpu->reg[nib1] = ((i16)cpu->reg[nib2] < (i16)cpu->reg[nib3]) ? 1 : 0;
	return 0;
}

i8 instr_or(struct cpu *cpu, u8 nib1, u8 nib2);
i8 instr_xor(struct cpu *cpu, u8 nib1, u8 nib2);
i8 instr_not(struct cpu *cpu, u8 nib1, u8 nib2);
i8 instr_and(struct cpu *cpu, u8 nib1, u8 nib2);
i8 instr_loa(struct cpu *cpu, u8 nib1, u8 nib2);
i8 instr_sto(struct cpu *cpu, u8 nib1, u8 nib2);
i8 instr_jpz(struct cpu *cpu, u8 nib1, u8 nib2);
i8 instr_jnz(struct cpu *cpu, u8 nib1, u8 nib2);
i8 instr_lsr(struct cpu *cpu, u8 nib1, u8 nib2);
i8 instr_lsl(struct cpu *cpu, u8 nib1, u8 nib2);
i8 instr_asr(struct cpu *cpu, u8 nib1, u8 nib2);
i8 instr_asl(struct cpu *cpu, u8 nib1, u8 nib2);
i8 instr_2op(struct cpu *cpu, u8 nib1, u8 nib2, u8 nib3)
{
	if (cpu->instr_tab->op2_tab[nib3](cpu, nib1,
		nib2) == 1) {
		log_err_args("failed to execute subinstruction \"0x%04X\"\n", nib3);
		return 1;
	}
	return 0;
}

i8 instr_jto(struct cpu *cpu, u8 nib1);
i8 instr_psh(struct cpu *cpu, u8 nib1);
i8 instr_pop(struct cpu *cpu, u8 nib1);
i8 instr_cll(struct cpu *cpu, u8 nib1);
i8 instr_1op(struct cpu *cpu, u8 nib1, u8 nib2, u8 nib3)
{
	if (cpu->instr_tab->op1_tab[nib3]
		(cpu, nib1) == 1) {
		log_err_args("failed to execute subinstruction \"0x%04X\"\n", nib3);
		return 1;
	}
	return 0;
}

i8 instr_ldl(struct cpu *cpu, u8 nib1, u8 nib2, u8 nib3)
{
	if (nib1 == 0) return 0;
	u8 imm8 = (nib2 << 4) | nib3;
	cpu->reg[nib1] = (cpu->reg[nib1] & 0xFF00) | (u16)imm8;
	return 0;
}

i8 instr_ldu(struct cpu *cpu, u8 nib1, u8 nib2, u8 nib3)
{
	if (nib1 == 0) return 0;
	u8 imm8 = (nib2 << 4) | nib3;
	cpu->reg[nib1] = (cpu->reg[nib1] & 0x00FF) | ((u16)imm8 << 8);
	return 0;
}

i8 instr_hlt(struct cpu *cpu);
i8 instr_sys(struct cpu *cpu);
i8 instr_xrt(struct cpu *cpu);
i8 instr_trp(struct cpu *cpu);
i8 instr_nnt(struct cpu *cpu);
i8 instr_int(struct cpu *cpu);
i8 instr_drg(struct cpu *cpu);
i8 instr_brk(struct cpu *cpu);
i8 instr_ctr(struct cpu *cpu, u8 nib1, u8 nib2, u8 nib3)
{
	if (cpu->instr_tab->ctr_tab[nib3](cpu) == 1) {
		log_err_args("failed to execute"
			"subinstruction \"0x%04X\"\n", nib3);
		return 1;
	}
	return 0;
}

i8 instr_jmp(struct cpu *cpu, u8 nib1, u8 nib2, u8 nib3)
{
	u16 pc = ((nib1 & 0xF) << 8) | ((nib2 & 0xF) << 4) | (nib3 & 0xF);
	cpu->reg[REG_PC] += pc;
	(void)pc;
	return 0;
}

i8 instr_or(struct cpu *cpu, u8 nib1, u8 nib2)
{
	cpu->reg[nib1] = cpu->reg[nib1] | cpu->reg[nib2];
	return 0;
}

i8 instr_xor(struct cpu *cpu, u8 nib1, u8 nib2)
{ 
	cpu->reg[nib1] = cpu->reg[nib1] ^ cpu->reg[nib2];
	return 0;
}

i8 instr_not(struct cpu *cpu, u8 nib1, u8 nib2)
{ 
	cpu->reg[nib1] = ~cpu->reg[nib2];
	return 0;
}

i8 instr_and(struct cpu *cpu, u8 nib1, u8 nib2)
{ 
	cpu->reg[nib1] = cpu->reg[nib1] & cpu->reg[nib2];
	return 0;
}
i8 instr_loa(struct cpu *cpu, u8 nib1, u8 nib2)
{
	cpu->reg[nib1] = cpu->mem[cpu->reg[nib2]];
	return 0;
}
i8 instr_sto(struct cpu *cpu, u8 nib1, u8 nib2)
{ 
	cpu->mem[cpu->reg[nib2]] = cpu->reg[nib1]; 
	return 0;
}
i8 instr_jpz(struct cpu *cpu, u8 nib1, u8 nib2)
{ 
	if (!cpu->reg[nib2]) {
		cpu->reg[REG_PC] = cpu->reg[nib1];
	}
	return 0;
}
i8 instr_jnz(struct cpu *cpu, u8 nib1, u8 nib2)
{ 
	if (cpu->reg[nib2]) {
		cpu->reg[REG_PC] = cpu->reg[nib1];
	}
	return 0;
}

i8 instr_lsr(struct cpu *cpu, u8 nib1, u8 nib2)
{ 
	cpu->reg[nib1] = cpu->reg[nib1] >> nib2;	
	return 0;
}

i8 instr_lsl(struct cpu *cpu, u8 nib1, u8 nib2)
{ 
	cpu->reg[nib1] = cpu->reg[nib1] << nib2;	
	return 0;
}
i8 instr_asr(struct cpu *cpu, u8 nib1, u8 nib2)
{ 
	cpu->reg[nib1] = (i16)cpu->reg[nib1] >> nib2;
	return 0;
}
i8 instr_asl(struct cpu *cpu, u8 nib1, u8 nib2)
{ 
	cpu->reg[nib1] = (i16)cpu->reg[nib1] << nib2;
	return 0;
}

i8 instr_jto(struct cpu *cpu, u8 nib1)
{ 
	cpu->reg[REG_PC] = cpu->mem[cpu->reg[nib1]];
	return 0;
}

i8 instr_psh(struct cpu *cpu, u8 nib1)
{ 
	log_info("stack is unimplemented, skipping");
	//cpu->mem[cpu->reg[REG_SP]--] = cpu->reg[nib1];
	return 0;
}

i8 instr_pop(struct cpu *cpu, u8 nib1)
{ 
	log_info("stack is unimplemented, skipping");
	//cpu->reg[nib1] = cpu->mem[cpu->reg[REG_SP]++];
	return 0;
}

i8 instr_cll(struct cpu *cpu, u8 nib1)
{ 
	cpu->reg[REG_LR] = cpu->reg[REG_PC];
	cpu->reg[REG_PC] = cpu->mem[cpu->reg[nib1]];
	return 0;
}

i8 instr_hlt(struct cpu *cpu)
{
	cpu->running = 0;
	return 0;
}

i8 instr_sys(struct cpu *cpu)
{
	(void)cpu;
	log_info("TODO(sys)\n");
	return 0;
}

i8 instr_xrt(struct cpu *cpu)
{
	(void)cpu;
	log_info("TODO(xrt)\n");
	return 0;
}

i8 instr_trp(struct cpu *cpu)
{
	(void)cpu;
	log_info("TODO(trp)\n");
	return 0;
}

i8 instr_nnt(struct cpu *cpu)
{
	(void)cpu;
	log_info("TODO(nnt)\n");
	return 0;
}

i8 instr_int(struct cpu *cpu)
{
	(void)cpu;
	log_info("TODO(int)\n");
	return 0;
}

i8 instr_dbi(struct cpu *cpu)
{
	for (i32 i = 0; i < 16; i++) {
		printf("cpu->reg[%d]: %d\n", i, cpu->reg[i]);
	}
	printf("\n");
	printf("cpu->reg: %p\n", (void *)cpu->reg);
	printf("cpu->mem: %p\n", (void *)cpu->mem);
	printf("cpu->priv: %d\n", cpu->priv);
	printf("cpu->running: %d\n", cpu->running);
	printf("cpu->instr_tab: %p\n\n", (void *)cpu->instr_tab);
	return 0;
}

i8 instr_brk(struct cpu *cpu)
{
	(void)cpu;
	log_info("TODO(brk)\n");
	return 0;
}

i8 op2_load(op2_handler *tab)
{
	if (!tab) {
		log_err("invalid pointer argument");
		return 1;
	}
	tab[SUBOP_OR] = instr_or;
	tab[SUBOP_XOR] = instr_xor;
	tab[SUBOP_NOT] = instr_not;
	tab[SUBOP_AND] = instr_and;
	tab[SUBOP_LOA] = instr_loa;
	tab[SUBOP_STO] = instr_sto;
	tab[SUBOP_JPZ] = instr_jpz;
	tab[SUBOP_JNZ] = instr_jnz;
	tab[SUBOP_LSR] = instr_lsr;
	tab[SUBOP_LSL] = instr_lsl;
	tab[SUBOP_ASR] = instr_asr;
	tab[SUBOP_ASL] = instr_asl;
	return 0;
}

i8 op1_load(op1_handler *tab)
{
	if (!tab) {
		log_err("invalid pointer argument");
		return 1;
	}
	tab[SUBOP_JTO] = instr_jto;
	tab[SUBOP_PSH] = instr_psh;
	tab[SUBOP_POP] = instr_pop;
	tab[SUBOP_CLL] = instr_cll;
	return 0;
}

i8 ctr_load(ctr_handler *tab)
{
	if (!tab) {
		log_err("invalid pointer argument");
		return 1;
	}
	tab[SUBOP_HLT] = instr_hlt;
	tab[SUBOP_SYS] = instr_sys;
	tab[SUBOP_XRT] = instr_xrt;
	tab[SUBOP_TRP] = instr_trp;
	tab[SUBOP_NNT] = instr_nnt;
	tab[SUBOP_INT] = instr_int;
	tab[SUBOP_DBI] = instr_dbi;
	tab[SUBOP_BRK] = instr_brk;
	return 0;
}

i8 instr_table_load(struct instr_table *tab)
{
	if (!tab) {
		log_err("invalid pointer argument");
		return 1;
	}
	tab->top_tab = calloc(1, 16 * sizeof(instr_handler));
	tab->op2_tab = calloc(1, 16 * sizeof(op2_handler));
	tab->op1_tab = calloc(1, 16 * sizeof(op1_handler));
	tab->ctr_tab = calloc(1, 16 * sizeof(ctr_handler));
	tab->top_tab[OP_NOP] = instr_nop;
	tab->top_tab[OP_ADD] = instr_add;
	tab->top_tab[OP_SUB] = instr_sub;
	tab->top_tab[OP_MUL] = instr_mul;
	tab->top_tab[OP_DIV] = instr_div;
	tab->top_tab[OP_SLT] = instr_slt;
	tab->top_tab[OP_2OP] = instr_2op;
	tab->top_tab[OP_1OP] = instr_1op;
	tab->top_tab[OP_LDL] = instr_ldl;
	tab->top_tab[OP_LDU] = instr_ldu;
	tab->top_tab[OP_CTR] = instr_ctr;
	tab->top_tab[OP_JMP] = instr_jmp;
	op2_load(tab->op2_tab);
	op1_load(tab->op1_tab);
	ctr_load(tab->ctr_tab);

	return 0;
}


i8 instr_table_unload(struct instr_table *tab)
{
	free(tab->top_tab);
	free(tab->op2_tab);
	free(tab->op1_tab);
	free(tab->ctr_tab);

	return 0;
}
