#ifndef __INSTR_HELPER_H__
#define __INSTR_HELPER_H__

// the type of an instruction entry
typedef int (*instr_func)(uint32_t eip, uint8_t opcode);

void print_asm_0(char *instr, char *suffix, uint8_t len);
void print_asm_1(char *instr, char *suffix, uint8_t len, OPERAND *opr_1);
void print_asm_2(char *instr, char *suffix, uint8_t len, OPERAND *opr_1, OPERAND *opr_2);
void print_asm_3(char *instr, char *suffix, uint8_t len, OPERAND *opr_1, OPERAND *opr_2, OPERAND *opr_3);

// macro for making an instruction entry
#define make_instr_func(name) int name(uint32_t eip, uint8_t opcode)

// macro for generating the implementation of an instruction with one operand
#define make_instr_impl_1op(inst_name, src_type, suffix)                                                                        \
 make_instr_func(concat5(inst_name, _, src_type, _, suffix))                                                                 \
 {                                                                                                                           \
  int len = 1;                                                                                                            \
  concat(decode_data_size_, suffix)                                                                                       \
  concat3(decode_operand, _, src_type)                                                                                \
  print_asm_1(#inst_name, opr_src.data_size == 8 ? "b" : (opr_src.data_size == 16 ? "w" : "l"), len, &opr_src); \
  instr_execute_1op();                                                                                                    \
  return len;                                                                                                             \
 }

// macro for generating the implementation of an instruction with one operand and condition
// for jcc and setcc, the opcode type are always fixed so it will not appear in the function name
#define make_instr_impl_1op_cc(inst_name, src_type, suffix, cc) \
 make_i