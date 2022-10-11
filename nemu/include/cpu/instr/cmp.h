#ifndef __INSTR_CMP_H__
#define __INSTR_CMP_H__
/*
Put the declarations of `cmp' instructions here.
*/
make_instr_func(cmp,rm,r,b);
make_instr_func(cmp,rm,r,v);
make_instr_func(cmp,r,rm,b);
make_instr_func(cmp,r,rm,v);
make_instr_func(cmp,i,a,b);
make_instr_func(cmp,i,a,v);


#endif
