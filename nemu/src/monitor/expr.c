#include "nemu.h"
#include "cpu/reg.h"
#include "memory/memory.h"

#include <stdlib.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum
{
	NOTYPE = 256,
	EQ,
	NUM,
	REG,
	SYMB,
	HEX
	/* TODO: Add more token types */

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
	{"0[xX][0-9a-fA-F]{1,10}",HEX},//十六进制数字 应该放在NUM之前
	{"[0-9]{1,10}",NUM},
	{"$[a-z]{2,3}",REG},
	{"\\+",'+'},
	{"-",'-'},
	{"\\*",'*'},
	{"\\/",'/'},
	{"\\(",'('},
	{"\\)",')'},
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

				/* TODO: Now a new token is recognized with rules[i]. 
				 * Add codes to perform some actions with this token.
				 */
                if(i == 0)//是空格则退出
                    break;
				switch (rules[i].token_type)
				{
    				case 0:{//空格
                        break;    				    
    				}
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
            uint32_t index;
            if     ( strcmp(tokens[p].str,"$eax") == 0 ) index = 0;
            else if( strcmp(tokens[p].str,"$ecx") == 0 ) index = 1;
            else if( strcmp(tokens[p].str,"$edx") == 0 ) index = 2;
            else if( strcmp(tokens[p].str,"$ebx") == 0 ) index = 3;
            else if( strcmp(tokens[p].str,"$esp") == 0 ) index = 4;
            else if( strcmp(tokens[p].str,"$ebp") == 0 ) index = 5;
            else if( strcmp(tokens[p].str,"$esi") == 0 ) index = 6;
            else if( strcmp(tokens[p].str,"$edi") == 0 ) index = 7;
            else { printf("fifa expr!\n"); return -1; }
            index = cpu.gpr[index].val;
            return index;
        }
        printf("fifa expr!\n");
        return -1;
    }
    else if(check_parentheses(p,q) == 1){
        return eval(p + 1, q - 1);
    }
    else if(check_parentheses(p,q) == -1){
        printf("fifa expr!\n");
        return -1;
    }
    else{
        uint32_t op=0;
        char op_type='0';
        
        uint32_t src=p,end=q;
        uint32_t left_parentheses=0;
        
        while(src<=end){//  1 + (2 + 3) 段错误
            switch(tokens[src].type){
                case '(':{
                    left_parentheses++;
                    break;
                }
                case ')':{
                    left_parentheses--;
                    break;
                }
                case '+':{
                    if(left_parentheses<=0){
                        op_type='+';op=src;
                    }
                    break;
                }
                case '-':{
                    if(left_parentheses<=0){
                        op_type='-';op=src;
                    }
                    break;
                }
                case '*':{
                    if(op_type=='0'||op_type=='*'||op_type=='/'){
                        if(left_parentheses<=0){
                            op_type='*';op=src;
                        }
                    }
                    break;
                }
                case '/':{
                    if(op_type=='0'||op_type=='*'||op_type=='/'){
                        if(left_parentheses<=0){
                            op_type='/';op=src;
                        }
                    }
                    break;
                }
                default:break;
            }
            src++;
        }
        
        uint32_t val1 = eval(p, op - 1);
        uint32_t val2 = eval(op + 1, q);
        
        switch(op_type){
            case '+':return val1 + val2;
            case '-':return val1 - val2;
            case '*':return val1 * val2;
            case '/':return val1 / val2;
            default:assert(0);
        }
    }
    return 0;
}


//如何处理括号匹配的问题？需要能够识别出错误的表达式
uint32_t check_parentheses(uint32_t p,uint32_t q){
    int index_parentheses[32] = { -1 };
    int top=0;uint32_t i=p;
    while(p <= q){
        if(p == q){//最后一个
            if(tokens[p].type == ')'&& top == 1 && index_parentheses[top-1] == i)
                return 1;
        }
        else if(tokens[p].type == '('){
            index_parentheses[top++]=p;//入栈
        }
        else if(tokens[p].type == ')'){
            if(top==0)
                return -1;//FIFA
            else{
                top--;//出栈
            }
        }
        p++;
    }
    if(top>0)
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
    
    uint32_t ans = eval(0,nr_token-1);
    
	return ans;
}






