#include "cpu/exec.h"
#include "all-instr.h"

static OpcodeEntry load_table [8] = {
  EXW(lds, 1), EXW(lds, 2), EXW(ld, 4), EXW(ld, 8), EXW(ld, 1), EXW(ld, 2), EMPTY, EMPTY
};

static make_EHelper(load) {
  decinfo.width = load_table[decinfo.isa.instr.funct3].width;
  idex(pc, &load_table[decinfo.isa.instr.funct3]);
}

static OpcodeEntry store_table [8] = {
  EXW(st, 1), EXW(st, 2), EXW(st, 4), EXW(st, 8), EMPTY, EMPTY, EMPTY, EMPTY
};

static make_EHelper(store) {
  decinfo.width = store_table[decinfo.isa.instr.funct3].width;
  idex(pc, &store_table[decinfo.isa.instr.funct3]);
}

static OpcodeEntry op_imm_table [8] = {
  EX(add), EX(sll), EX(slt), EX(sltu), EX(xor), EX(srl), EX(or), EX(and)
};

static make_EHelper(op_imm) {
  idex(pc, &op_imm_table[decinfo.isa.instr.funct3]);
}

static OpcodeEntry op_imm_table64 [8] = {
  EX(addw), EX(sllw), EMPTY, EMPTY, EMPTY, EX(sraw), EMPTY, EMPTY
};

static make_EHelper(op_imm64) {
  idex(pc, &op_imm_table64[decinfo.isa.instr.funct3]);
}

static OpcodeEntry op_table [8] = {
  EX(add), EX(sll), EX(slt), EX(sltu), EX(xor), EX(srl), EX(or), EX(and)
};

static OpcodeEntry op2_table [8] = {
  EX(sub), EMPTY, EMPTY, EMPTY, EMPTY, EX(sra), EMPTY, EMPTY
};

static OpcodeEntry muldiv_table [8] = {
  EX(mul), EX(mulh), EMPTY, EX(mulhu), EX(div), EX(divu), EX(rem), EX(remu)
};

static make_EHelper(op) {
  switch (decinfo.isa.instr.funct7) {
    case 0:  idex(pc, &op_table[decinfo.isa.instr.funct3]); break;
    case 1:  idex(pc, &muldiv_table[decinfo.isa.instr.funct3]); break;
    case 32: idex(pc, &op2_table[decinfo.isa.instr.funct3]); break;
    default: assert(0);
  }
}

static OpcodeEntry op_table64 [8] = {
  EX(addw), EX(sllw), EMPTY, EMPTY, EMPTY, EX(srlw), EMPTY, EMPTY
};

static OpcodeEntry op2_table64 [8] = {
  EX(subw), EMPTY, EMPTY, EMPTY, EMPTY, EX(sraw), EMPTY, EMPTY
};

static OpcodeEntry muldiv_table64 [8] = {
  EX(mulw), EMPTY, EMPTY, EMPTY, EX(divw), EX(divuw), EX(remw), EX(remuw)
};

static make_EHelper(op64) {
  switch (decinfo.isa.instr.funct7) {
    case 0:  idex(pc, &op_table64[decinfo.isa.instr.funct3]); break;
    case 1:  idex(pc, &muldiv_table64[decinfo.isa.instr.funct3]); break;
    case 32: idex(pc, &op2_table64[decinfo.isa.instr.funct3]); break;
    default: printf("not implemented\n"); assert(0);
  }
}

static OpcodeEntry system_table [8] = {
  EX(priv), IDEX(csr, csrrw), IDEX(csr, csrrs), EMPTY, EMPTY, EMPTY, EMPTY, EMPTY
};

static make_EHelper(system) {
  idex(pc, &system_table[decinfo.isa.instr.funct3]);
}

static OpcodeEntry opcode_table [32] = {
  /* b00 */ IDEX(ld, load), EMPTY, EMPTY, EMPTY, IDEX(I, op_imm), IDEX(U, auipc), IDEX(I, op_imm64), EMPTY,
  /* b01 */ IDEX(st, store), EMPTY, EMPTY, EMPTY, IDEX(R, op), IDEX(U, lui), IDEX(R, op64), EMPTY,
  /* b10 */ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
  /* b11 */ IDEX(B, branch), IDEX(I, jalr), EX(nemu_trap), IDEX(J, jal), EX(system), EMPTY, EMPTY, EMPTY,
};

void isa_exec(vaddr_t *pc) {
  decinfo.isa.instr.val = instr_fetch(pc, 4);
  assert(decinfo.isa.instr.opcode1_0 == 0x3);
  idex(pc, &opcode_table[decinfo.isa.instr.opcode6_2]);
}