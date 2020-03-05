#include <monitor/difftest.h>
#include "../local-include/csr.h"

static inline make_EHelper(csrrw) {
  uint32_t addr = *dsrc2;
  csr_read(s0, addr);
  rtl_sr(s, id_dest->reg, s0, 8);
  csr_write(addr, dsrc1);

  print_asm_template3("csrrw");
}

static inline make_EHelper(csrrs) {
  uint32_t addr = *dsrc2;
  csr_read(s0, addr);
  rtl_sr(s, id_dest->reg, s0, 8);
  if (id_src1->reg != 0) {
    rtl_or(s, s0, s0, dsrc1);
    csr_write(addr, s0);
  }

  print_asm_template3("csrrs");
}

static inline make_EHelper(csrrc) {
  uint32_t addr = id_src2->val;
  csr_read(s0, addr);
  rtl_sr(s, id_dest->reg, s0, 8);
  if (id_src1->reg != 0) {
    rtl_not(s, s1, dsrc1);
    rtl_and(s, s0, s0, s1);
    csr_write(addr, s0);
  }

  print_asm_template3("csrrc");
}

static inline make_EHelper(priv) {
  uint32_t type = s->isa.instr.csr.csr;
  switch (type) {
    case 0:
      raise_intr(s, 8 + cpu.mode, cpu.pc);
      print_asm("ecall");
      break;
    case 0x102:
      mstatus->sie = mstatus->spie;
#ifdef DIFF_TEST
      // this is bug of QEMU
      mstatus->spie = 0;
#else
      mstatus->spie = 1;
#endif
      change_mode(mstatus->spp);
      mstatus->spp = MODE_U;
      rtl_li(s, s0, sepc->val);
      rtl_jr(s, s0);
      print_asm("sret");
      break;
    case 0x120:
      print_asm("sfence.vma");
      break;
    case 0x105:
      //panic("Executing wfi now will exit NEMU\n"
      //    "TODO: how to let NEMU execute wfi as REF in DiffTest?");
      print_asm("wfi");

      // let the clock go quickly to reduce idle time in Linux
#if !_SHARE
      void clint_intr(void);
      clint_intr();
#endif
      break;
    case 0x302:
      mstatus->mie = mstatus->mpie;
#ifdef DIFF_TEST
      // this is bug of QEMU
      mstatus->mpie = 0;
#else
      mstatus->mpie = 1;
#endif
      change_mode(mstatus->mpp);
      mstatus->mpp = MODE_U;
      rtl_li(s, s0, mepc->val);
      rtl_jr(s, s0);
      print_asm("mret");
      break;
    default: panic("unimplemented priv instruction type = 0x%x", type);
  }

  difftest_skip_dut(1, 2);
}

static inline make_EHelper(fence) {
  difftest_skip_dut(1, 2);
}