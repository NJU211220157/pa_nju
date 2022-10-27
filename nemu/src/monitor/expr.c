#include "nemu.h"
#include "cpu/reg.h"
#include "memory/memory.h"
#include "cpu/cpu.h"

#include <stdlib.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum
{
	NOTYPE = 256,
	NUM,
	REG,
	SYMB,//符号
	/* TODO: Add more token types */
    HEX,//16进制数
    
    DEREF,//*
    NEG,// -
    NOT,// !
    
    DIV,// /
    MUL,// *
    ADD,// +
    SUB,// -
    
	EQ,//==
	NEQ,//!=
	
	AND,// &&
	OR,// ||
	
};

static struct rule
{
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +", NOTYPE}, // white space 一个或者多个空格
	
	{"\\(",'('},
	{"\\)",')'},
	
	{"0[xX][0-9a-fA-F]{1,10}",HEX},//十六进制数字 应该放在NUM之前
	{"[0-9]{1,10}",NUM},
	{"\\$[a-z]{2,3}",REG},
	
	{"\\+",ADD},
	{"-",SUB},
	{"\\*",MUL},
	{"\\/",DIV},
	
	{"\\!=",NEQ},
	{"\\!",NOT},
	
	{"\\==",EQ},
	{"\\&&",AND},
	{"\\||",OR}
	
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]))

static regex_t re[NR_REGEX];

/* Rules are used for more times.
 * Therefore we compile them only once before any usage.
 */
void init_regex()
{
	int i;
	char error_msg[128];
	int ret;

	for (i = 0; i < NR_REGEX; i++)
	{
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if (ret != 0)
		{
			regerror(ret, &re[i], error_msg, 128);
			assert(ret != 0);
		}
	}
}

typedef struct token
{
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e)
{
	int position = 0;
	int i;
	regmatch_t pmatch;

	nr_token = 0;//将索引初始化为0

	while (e[position] != '\0')
	{
		/* Try all rules one by one. */
		for (i = 0; i < NR_REGEX; i++)
		{
			if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0)
			{
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				//printf("match regex[%d] at position %d with len %d: %.*s\n", i, position, substr_len, substr_len, substr_start);
				position += substr_len;

				switch (rules[i].token_type)
				{
				    case NOTYPE:break;
    				case REG:{
    				    tokens[nr_token].type=rules[i].token_type;
                        for(int j=0;j<substr_len;j++){
                            tokens[nr_token].str[j]=substr_start[j];
                        }
                        nr_token++;
                        break;
    				}
    				case HEX:{
    				    tokens[nr_token].type=rules[i].token_type;
                        for(int j=0;j<substr_len;j++){
                            tokens[nr_token].str[j]=substr_start[j];
                        }
                        nr_token++;
                        break;
    				}
                    case NUM:{// NUM
                        tokens[nr_token].type=rules[i].token_type;
                        for(int j=0;j<substr_len;j++){
                            tokens[nr_token].str[j]=substr_start[j];
                        }
                        nr_token++;
                        break;
                    }
				default:
					tokens[nr_token].type = rules[i].token_type;
					nr_token++;
				}

				break;
			}
		}

		if (i == NR_REGEX)
		{
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true;
}

uint32_t check_parentheses(uint32_t p,uint32_t q);

uint32_t eval(uint32_t p,uint32_t q){
    if(p > q){
        return 0;//返回一个不会对结果造成影响的值
    }
    else if(p == q){
        if(tokens[p].type == NUM)
            return atoi(tokens[p].str);
        else if(tokens[p].type == HEX){
            uint32_t val;
            sscanf(tokens[p].str,"%x",&val);
            return val;
        }
        else if(tokens[p].type == REG){
            if     ( strcmp(tokens[p].str,"$eax") == 0 ) return cpu.gpr[0].val;
            else if( strcmp(tokens[p].str,"$ax" ) == 0 ) return cpu.gpr[0]._16;
            else if( strcmp(tokens[p].str,"$al" ) == 0 ) return cpu.gpr[0]._8[0];
            
            else if( strcmp(tokens[p].str,"$ecx") == 0 ) return cpu.gpr[1].val;
            else if( strcmp(tokens[p].str,"$cx" ) == 0 ) return cpu.gpr[1]._16;
            else if( strcmp(tokens[p].str,"$cl" ) == 0 ) return cpu.gpr[1]._8[0];
            
            else if( strcmp(tokens[p].str,"$edx") == 0 ) return cpu.gpr[2].val;
            else if( strcmp(tokens[p].str,"$dx" ) == 0 ) return cpu.gpr[2]._16;
            else if( strcmp(tokens[p].str,"$dl" ) == 0 ) return cpu.gpr[2]._8[0];
            
            else if( strcmp(tokens[p].str,"$ebx") == 0 ) return cpu.gpr[3].val;
            else if( strcmp(tokens[p].str,"$bx" ) == 0 ) return cpu.gpr[3]._16;
            else if( strcmp(tokens[p].str,"$bl" ) == 0 ) return cpu.gpr[3]._8[0];
            
            else if( strcmp(tokens[p].str,"$esp") == 0 ) return cpu.gpr[4].val;
            else if( strcmp(tokens[p].str,"$ebp") == 0 ) return cpu.gpr[5].val;
            else if( strcmp(tokens[p].str,"$esi") == 0 ) return cpu.gpr[6].val;
            else if( strcmp(tokens[p].str,"$edi") == 0 ) return cpu.gpr[7].val;
            
            else { printf("fifa registers expr!\n"); return -1; }
        }
        printf("fifa expr in line190!\n");
        return -1;
    }
    else if(check_parentheses(p,q) == 1){
        return eval(p + 1, q - 1);
    }
    else if(check_parentheses(p,q) == -1){
        printf("fifa expr in line197!\n");
        return -1;
    }
    else{
        uint32_t op=0;
        int op_type=0;;
        
        uint32_t src=p,end=q;
        uint32_t left_parentheses=0;
        
        while(src<=end){
            switch(tokens[src].type){
                case '(':{
                    left_parentheses++;
                    break;
                }
                case ')':{
                    left_parentheses--;
                    break;
                }
                case ADD:{
                    if(op_type == 0 || op_type <= SUB){
                        if(left_parentheses<=0){
                            op_type = ADD;op = src;
                        }
                    }
                    break;
                }
                case SUB:{
                    if(op_type == 0 || op_type <= SUB){
                        if(left_parentheses<=0){
                            op_type = SUB;op = src;
                        }
                    }
                    break;
                }
                case MUL:{
                    if(op_type == 0||op_type <= MUL){
                        if(left_parentheses<=0){
                            op_type = MUL;op = src;
                        }
                    }
                    break;
                }
                case DIV:{
                    if(op_type == 0||op_type <= MUL){
                        if(left_parentheses<=0){
                            op_type = DIV;op = src;
                        }
                    }
                    break;
                }
                case EQ:{
                    if(op_type == 0||op_type <= NEQ){
                        if(left_parentheses<=0){
                            op_type = EQ;op = src;
                        }
                    }
                    break;
                }
                case NEQ:{
                    if(op_type == 0||op_type <= NEQ){
                        if(left_parentheses<=0){
                            op_type = NEQ;op = src;
                        }
                    }
                    break;
                }
                case AND:{
                    if(op_type == 0||op_type <= AND){
                        if(left_parentheses<=0){
                            op_type = AND;op = src;
                        }
                    }
                    break;
                }
                case OR:{
                    if(op_type == 0||op_type <= OR){
                        if(left_parentheses<=0){
                            op_type = OR;op = src;
                        }
                    }
                    break;
                }
                case DEREF:{
                    /*uint32_t addr = eval(op + 1,q);
                    uint32_t val  = vaddr_read(addr, uint8_t sreg, 32);*/
                    break;
                }
                case NEG:{
                    if(op_type == 0||op_type <= NEG){
                        if(left_parentheses<=0){
                            op_type =NEG;op = src;
                        }
                    }
                    break;
                }
                case NOT:{
                    if(op_type == 0||op_type <= NOT){
                        if(left_parentheses<=0){
                            op_type = NOT;op = src;
                        }
                    }
                    break;
                }
                default:break;
            }
            src++;
        }
        

        switch(op_type){
            case DEREF:{
                /*uint32_t addr = eval(op + 1,q);
                uint32_t val  = vaddr_read(addr, uint8_t sreg, 32);*/
                break;
            }
            case NEG:   return 0-(eval(op + 1,q));
            case NOT:   return !eval(op + 1,q);
            default:break;
        }
        uint32_t val1 = eval(p, op - 1);
        uint32_t val2 = eval(op + 1, q);
        
        switch(op_type){
            case ADD:  return val1 + val2;
            case SUB:  return val1 - val2;
            case MUL:  return val1 * val2;
            case DIV:  return val1 / val2;
            case EQ:   return val1 == val2;
            case NEQ:  return val1 != val2;
            case AND:  return val1 && val2;
            case OR:   return val1 || val2;
            default:assert(0);
        }
    }
    return 0;
}


//如何处理括号匹配的问题？需要能够识别出错误的表达式

// 1+(2+3)
uint32_t check_parentheses(uint32_t p,uint32_t q){
    int index_parentheses[32] = { -1 };
    int top=0;uint32_t i=p;
    while(p <= q){
        if(p == q){//最后一个
            if(tokens[p].type == ')'&& top == 1 && index_parentheses[top-1] == i)
                return 1;
        }
        if(tokens[p].type == '('){
            index_parentheses[top++] = p;//入栈
        }
        else if(tokens[p].type == ')'){
            if(top == 0)
                return -1;//FIFA
            else{
                top--;//出栈
            }
        }
        p++;
    }
    if(top > 0)
        return -1;
    return 0;
}

uint32_t expr(char *e, bool *success)
{
	if (!make_token(e))//make_token()函数以e为参数，修改全局变量tokens
	{
		*success = false;
		return 0;
	}

    *success = true;
    
    //printf("nr_token = %d\n",nr_token);
    
    bool last_oper = 0 ;
    
    for(int i = 0;i < nr_token; i++){
        if(tokens[i].type == '-' && (i == 0 || (tokens[i].type >= DIV && tokens[i].type <= OR)  )){
            tokens[i].type = NEG;
        }
        else if(tokens[i].type == '*' && (i == 0 || (tokens[i].type >= DIV && tokens[i].type <= OR)  )){
            tokens[i].type = DEREF;
        }
    }
    
    
    
    for(int i = 0;i<nr_token;i++){
        if(tokens[i].type >= DIV && tokens[i].type <= OR){
            if(last_oper == 1){
                printf("fifa expr!\n");
                return -1;
            }
            last_oper = 1;
        }
        else{
            last_oper = 0;
        }
        if(i >= 1 && tokens[i-1].type == tokens[i].type&&tokens[i].type == NUM){
            printf("fifa expr!\n");
            return -1;
        }
    }
    
    uint32_t ans = eval(0,nr_token-1);
    
	return ans;
}






