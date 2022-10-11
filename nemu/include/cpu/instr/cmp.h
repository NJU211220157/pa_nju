#ifndef __INSTR_CMP_H__
#define __INSTR_CMP_H__
/*
Put the declarations of `cmp' instructions here.
*/
make_instr_impl_2op(cmp,rm,r,b);
make_instr_impl_2op(cmp,rm,r,v);
make_instr_impl_2op(cmp,r,rm,b);
make_instr_impl_2op(cmp,r,rm,v);
make_instr_impl_2op(cmp,i,a,b);
make_instr_impl_2op(cmp,i,a,v);
#endif
