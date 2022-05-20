#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_HEX,  TK_DECI, TK_REG, TK_LEFTPA, TK_RIGHTPA, TK_EQ, TK_UNEQ, TK_AND

  /* TODO: Add more token types */

};

static int priorities[512];

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\b0x[0-9]+", TK_HEX},	// hexadecimal numer
  {"\\b[0-9]+", TK_DECI},	// decimal number
  {"(\\$\\$0)|(\\$[a-z0-9]{1,3})", TK_REG},	// reg_name
 	{"\\(", TK_LEFTPA},		// left paren
	{"\\)", TK_RIGHTPA},	// right paren
  {"\\+", '+'},         // plus
  {"-", '-'},						// sub
  {"\\*", '*'},					// multiple
  {"\\/", '/'},					// divide	
  {"==", TK_EQ},        // equal
  {"!=", TK_UNEQ},			// unequl
  {"&&", TK_AND},				// and
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
	priorities['+'] = 4;
	priorities['-'] = 4;
	priorities['*'] = 3;
	priorities['/'] = 3;
	priorities[TK_EQ] = 7;
	priorities[TK_UNEQ] = 7;
	priorities[TK_AND] = 11;
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
	for (int i = 0; i < 32; i ++) {
		tokens[i].type = 0;
		memset(tokens[i].str, 0, strlen(tokens[i].str));
	}
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    // Try all rules one by one. 
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        // TODO: Now a new token is recognized with rules[i]. Add codes
        // to record the token in the array `tokens'. For certain types
        // of tokens, some extra actions should be performed.
        //

        switch (rules[i].token_type) {
        	case TK_DECI: 
        		tokens[nr_token].type = rules[i].token_type;
        		strncpy(tokens[nr_token].str, substr_start, substr_len);
        		nr_token ++;
        		break;
        	case TK_HEX:
        		tokens[nr_token].type = rules[i].token_type;
        		strncpy(tokens[nr_token].str, substr_start+2, substr_len-2);
        		nr_token ++;
        		break;
        	case TK_REG:
        		tokens[nr_token].type = rules[i].token_type;
        		strncpy(tokens[nr_token].str, substr_start+1, substr_len-1);
        		nr_token ++;
        		break;
        	case TK_NOTYPE:break;
          default: tokens[nr_token].type = rules[i].token_type; nr_token ++; break;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

static bool check_parentheses(int p, int q) {
	if (tokens[p].type != TK_LEFTPA || tokens[q].type != TK_RIGHTPA) {return false;}
	int leftpa_num = 0;
	int i;
	for (i = p+1; i < q; i ++) {
		switch (tokens[i].type) {
			case TK_LEFTPA: leftpa_num ++; break;
			case TK_RIGHTPA: leftpa_num --; break;
			default: break;
		}
		if (leftpa_num < 0) {break;}
	}
	bool result = leftpa_num == 0 && i == q;
	return result;
}


static int main_op(int p, int q) {
	int priority = -1;
	int operator = -1;
	int i = p;
	while (i <= q) {
		switch (tokens[i].type) {
			case TK_RIGHTPA:
				assert(0);
			case TK_LEFTPA:
			{
				int leftpa_num = 1;
				int j;
				for (j = i+1; j <= q; j ++) {
					switch (tokens[j].type) {
						case TK_LEFTPA: leftpa_num ++; break;
						case TK_RIGHTPA: leftpa_num --; break;
						default: break;
					}
					if (leftpa_num == 0) {j ++; break;}
				}
				if (leftpa_num != 0) { assert(0);}
				i = j;
				break;
			}
			case TK_AND:
			case TK_EQ:
			case TK_UNEQ:
			case '+':
			case '-':
			case '*':
			case '/':
				if (priority <= priorities[tokens[i].type]) {
					priority = priorities[tokens[i].type];
					operator = i;
				}
				i ++;
				break;
			default:
				i ++;
				break;
		}
	}
	if (operator < 0) {assert(0); }
	return operator;
}
				
		
static word_t eval(int p, int q) {
  if (p > q) {
    assert(0);
  }
  else if (p == q) {
  	// single token should be num
    switch (tokens[p].type) {
    	case TK_DECI:
    	{
    		unsigned decimal = atoi(tokens[p].str);
    		return decimal;
    	}
    	case TK_HEX:
    	{
    		char *endptr = NULL;
    		unsigned hex = (unsigned) strtol(tokens[p].str, &endptr, 16);
    		return hex;
    	}
    	case TK_REG:
    	{
    		bool b_false = false;
    		bool *read_suc = &b_false;
    		word_t reg_value = isa_reg_str2val(tokens[p].str, read_suc);
    		if (*read_suc) { return reg_value;}
    		else { return 0;}
    	}
    	default: assert(0);
  	}
  }
  else if (check_parentheses(p, q) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    return eval(p + 1, q - 1);
  }
  else {
    int op = main_op(p, q);
    int val1 = eval(p, op - 1);
    int val2 = eval(op + 1, q);

    switch (tokens[op].type) {
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/': return val1 / val2;
      case TK_EQ: return val1 == val2;
      case TK_UNEQ: return val1 != val2;
      case TK_AND: return val1 && val2;
      default: assert(0);
    }
  }
}


word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
   	return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  word_t result = eval(0, nr_token-1);
  *success = true;

  return result;
}
