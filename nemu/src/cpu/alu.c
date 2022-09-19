#include "cpu/cpu.h"

void set_CF_add(uint32_t res,uint32_t src, size_t data_size){
    //要先对res与src截取低data_size位后进行符号扩展
    //如果不截取低data_size位，会无法判断溢出情况
    res = sign_ext(res&(0xFFFFFFFF>>(32-data_size)),data_size);
    src = sign_ext(src&(0xFFFFFFFF>>(32-data_size)),data_size);
    cpu.eflags.CF = (res < src);
}
void set_ZF(uint32_t res,size_t data_size){
    res = res&(0xFFFFFFFF>>(32-data_size));
    cpu.eflags.ZF = (res == 0);
}
void set_SF(uint32_t res,size_t data_size){
    //什么时候需要进行符号扩展？
    res = sign_ext(res&(0xFFFFFFFF>>(32-data_size)),data_size);
    cpu.eflags.SF = sign(res);
}
void set_OF_add(uint32_t result, uint32_t src, uint32_t dest,size_t data_size){
    //获取src,dest,result的符号位前需要进行符号扩展，否则会得到错误的结果
    switch(data_size)
    {
        case 8:{
            result = sign_ext(result & 0xFF,8);//截取低八位
            src = sign_ext(src & 0xFF,8);
            dest = sign_ext(dest & 0xFF,8);
            break;
        }
        case 16:{
            result = sign_ext(result & 0xFFFF,16);
            src = sign_ext(src & 0xFFFF,16);
            dest = sign_ext(dest & 0xFFFF,16);
            break;
        }
        default:break;
    }
    if(sign(src)==sign(dest))
    {
        if(sign(src)!=sign(result))
            cpu.eflags.OF = 1;
        else 
            cpu.eflags.OF = 0;
    }
    else
        cpu.eflags.OF = 0;//正负相加一定不会溢出
}
void set_PF(uint32_t result){
    int count = 0;
    for(int i=0;i<8;i++)
    {
        if((result&0x1)==0x1)
            count++;
        result=(result>>1);
    }
    cpu.eflags.PF = (count%2==0);//偶数为0，奇数位1
}
uint32_t alu_add(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_add(src, dest, data_size);
#else
	uint32_t res = 0;//返回的结果
	res = src + dest;
    //接下来进行标志位的判断
    set_CF_add(res,src,data_size);
    set_ZF(res,data_size);
    set_OF_add(res,src,dest,data_size);
    set_SF(res,data_size);
    set_PF(res);
	return res&(0xFFFFFFFF>>(32-data_size));//32位无符号加法
#endif
}
void set_CF_adc(uint32_t res, uint32_t src, size_t data_size){
    res = sign_ext(res&(0xFFFFFFFF>>(32-data_size)),data_size);
    src = sign_ext(src&(0xFFFFFFFF>>(32-data_size)),data_size);
    if(cpu.eflags.CF == 1)
        cpu.eflags.CF = (res <= src);
    else 
        cpu.eflags.CF = (res < src);
}
//bug
uint32_t alu_adc(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_adc(src, dest, data_size);
#else
	uint32_t res = 0;
	res = src + dest + cpu.eflags.CF;
	set_CF_adc(res,src,data_size);
    set_ZF(res,data_size);
    set_OF_add(res,src,dest,data_size);
    set_SF(res,data_size);
    set_PF(res);
	return res&(0xFFFFFFFF>>(32-data_size));
#endif
}
void set_CF_sub(uint32_t dest, uint32_t src, size_t data_size){
    dest = sign_ext(dest&(0xFFFFFFFF>>(32-data_size)),data_size);
    src = sign_ext(src&(0xFFFFFFFF>>(32-data_size)),data_size);
    cpu.eflags.CF = (dest < src);
    
}
void set_OF_sub(uint32_t result,uint32_t dest,uint32_t src,size_t data_size){
    switch(data_size)
    {
        case 8:{
            result = sign_ext(result & 0xFF,8);//截取低八位
            src = sign_ext(src & 0xFF,8);
            dest = sign_ext(dest & 0xFF,8);
            break;
        }
        case 16:{
            result = sign_ext(result & 0xFFFF,16);
            src = sign_ext(src & 0xFFFF,16);
            dest = sign_ext(dest & 0xFFFF,16);
            break;
        }
        default:break;
    }
    if(sign(dest)!=sign(src))
    {
        if(sign(result)!=sign(dest))
            cpu.eflags.OF = 1;
        else
            cpu.eflags.OF = 0;
    }
    else
        cpu.eflags.OF = 0;
}
uint32_t alu_sub(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sub(src, dest, data_size);
#else
	uint32_t res=0;
// 	src = sign_ext(src);
// 	dest = sign_ext(dest);
	res = dest - src;
	set_PF(res);
	set_SF(res,data_size);
	set_ZF(res,data_size);
	set_CF_sub(dest,src,data_size);
	set_OF_sub(res,dest,src,data_size);
	return res & (0xFFFFFFFF>>(32-data_size));
#endif
}

void set_CF_sbb(uint32_t dest,uint32_t src,size_t data_size){
    dest = sign_ext(dest&(0xFFFFFFFF>>(32-data_size)),data_size);
    src = sign_ext(src&(0xFFFFFFFF>>(32-data_size)),data_size);
    if(cpu.eflags.CF==1)
        cpu.eflags.CF = (dest <= src);
    else
        cpu.eflags.CF = (dest < src);
}
uint32_t alu_sbb(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sbb(src, dest, data_size);
#else
	uint32_t res=0;
	res = dest - src - cpu.eflags.CF;
	set_PF(res);
	set_SF(res,data_size);
	set_ZF(res,data_size);
	set_CF_sbb(dest,src,data_size);
	set_OF_sub(res,dest,src,data_size);
	return res & (0xFFFFFFFF>>(32-data_size));
#endif
}

uint64_t alu_mul(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_mul(src, dest, data_size);
#else
	printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;
#endif
}

int64_t alu_imul(int32_t src, int32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_imul(src, dest, data_size);
#else
	printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;
#endif
}

// need to implement alu_mod before testing
uint32_t alu_div(uint64_t src, uint64_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_div(src, dest, data_size);
#else
	printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;
#endif
}

// need to implement alu_imod before testing
int32_t alu_idiv(int64_t src, int64_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_idiv(src, dest, data_size);
#else
	printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;
#endif
}

uint32_t alu_mod(uint64_t src, uint64_t dest)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_mod(src, dest);
#else
	printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;
#endif
}

int32_t alu_imod(int64_t src, int64_t dest)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_imod(src, dest);
#else
	printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;
#endif
}

uint32_t alu_and(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_and(src, dest, data_size);
#else
	uint32_t result;
	result = src & dest;
	set_ZF(result,data_size);
	set_SF(result,data_size);
	set_PF(result);
	cpu.eflags.OF=0;
	cpu.eflags.CF=0;
	return result &(0xFFFFFFFF>>(32-data_size));
#endif
}

uint32_t alu_xor(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_xor(src, dest, data_size);
#else
    uint32_t result;
	result = src ^ dest;
	set_ZF(result,data_size);
	set_SF(result,data_size);
	set_PF(result);
	cpu.eflags.OF=0;
	cpu.eflags.CF=0;
	return result &(0xFFFFFFFF>>(32-data_size));
#endif
}

uint32_t alu_or(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_or(src, dest, data_size);
#else
	uint32_t result;
	result = src | dest;
	set_ZF(result,data_size);
	set_SF(result,data_size);
	set_PF(result);
	cpu.eflags.OF=0;
	cpu.eflags.CF=0;
	return result &(0xFFFFFFFF>>(32-data_size));
#endif
}

uint32_t alu_shl(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_shl(src, dest, data_size);
#else
	//uint32_t b=dest&(0xFFFFFFFF<<data_size);//保留高32-data_size位
	uint32_t temp = src;
	while(temp!=0)//移动src位
	{
	    cpu.eflags.CF = sign(sign_ext(dest&(0xFFFFFFFF>>(32-data_size)),data_size));
	    //只取低data_size位
	    dest = dest*2;
	    dest = dest&(0xFFFFFFFF>>(32-data_size));//保留低data_size位
	    temp--;
	}
	if(src==1)
	{
	    cpu.eflags.OF= sign(dest!=cpu.eflags.CF);
	}
	set_PF(dest);
	set_SF(dest,data_size);
	set_ZF(dest,data_size);
	return dest ;//高位清0
#endif
}

uint32_t alu_shr(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_shr(src, dest, data_size);
#else
	uint32_t temp = src;
	dest = dest &(0xFFFFFFFF>>(32-data_size));
	while(temp!=0)//移动src位
	{
	    cpu.eflags.CF = dest &0x1;
	    //最后一位作为CF标志
	    dest = dest/2;
	    dest = dest&(0xFFFFFFFF>>(32-data_size));//保留低data_size位，高位清0
	    temp--;
	}
	if(src==1)
	{
	    cpu.eflags.OF= sign(dest!=cpu.eflags.CF);
	}
	set_PF(dest);
	set_SF(dest,data_size);
	set_ZF(dest,data_size);
	return dest ;//高位清0
#endif
}

uint32_t alu_sar(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sar(src, dest, data_size);
#else
	printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;
#endif
}

uint32_t alu_sal(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sal(src, dest, data_size);
#else
	printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;
#endif
}
