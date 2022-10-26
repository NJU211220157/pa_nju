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
	{"[0-9]{1,10}",NUM},
	{"\\+",'+'},
	{"-",'-'},
	{"\\*",'*'},
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

				printf("match regex[%d] at position %d with len %d: %.*s\n", i, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. 
				 * Add codes to perform some actions with this token.
				 */

				switch (rules[i].token_type)
				{
    				case 0:{//空格
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
        if(tokens[p].type==NUM)
            return atoi(tokens[p].str);
        else
            return -1;
    }
    else if(check_parentheses(p,q) == true){
        return eval(p+1,q-1);
    }
    else{
        uint32_t op=0;
        char op_type='0';
        
        uint32_t src=p,end=q;
        uint32_t left_parentheses=0;
        
        while(src<=end){
            switch(tokens[src].type){
                case '(':
                    left_parentheses++;
                    break;
                case ')':
                    left_parentheses--;
                    break;
                case '+':
                    op_type='+';op=src;
                    break;
                case '-':
                    op_type='-';op=src;
                    break;
                case '*':{
                    if(op_type=='0'||op_type=='*'||op_type=='/'){
                        op_type='*';op=src;
                    }
                    break;
                }
                case '/':{
                    if(op_type=='0'||op_type=='*'||op_type=='/'){
                        op_type='/';op=src;
                    }
                    break;
                }
                default:
                    break;
            }
            src++;
        }
        
        uint32_t val1 = eval(p,op-1);
        uint32_t val2 = eval(op+1,q);
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

uint32_t check_parentheses(uint32_t p,uint32_t q){
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
    
    uint32_t ans = eval(0,nr_token);
    
	return ans;
}






