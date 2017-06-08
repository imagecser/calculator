#include <readline/readline.h>
#include <readline/history.h>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <sstream>
#include <cmath>
#include <cstdio>
using namespace std;
#define BLUE "\e[1;4;34m"      /* Magenta */
#define NUNL "\e[24m"
#define CYAN "\e[1;36m"
#define RESET "\e[0m"

#define KRESET "\001\e[0m\002"
#define KRED "\001\e[1;31m\002"
const char Tab=0x9;
const int  DIGIT=1;
const int MAXLEN=16384;

//auto
static char** my_completion(const char*, int ,int);
char* my_generator(const char*,int);
char * dupstr (char*);
void *xmalloc (int);
 
char* cmd [] ={ (char*)"sin", (char*)"cos", (char*)"tan" ,(char*)"sqrt", (char*)"arcsin", (char*)"arccos", (char*)"arctan", (char*)"lg", (char*)"ln", 
    (char*)"ans", (char*)"history", (char*)"radix", (char*)" " };
//completion



struct State {
	string ex;
	double res;
    string display;
	string cmt;
    State() {
        res = 0;
    }
}pn;
char s[MAXLEN],*endss;
bool back;
int state = 10;
int pcs=15;
State ans;

long convert(char **p, int state){
    string num;
    long part;
    for(; **p && (((**p >= '0' && **p <= '9') || (**p >= 'A' && **p <= 'Z'))); ++*p)
        num.push_back(**p);
    part = strtol(num.c_str(), NULL, state);
    return part;
}

double strtodec(char *str, char **p, int radix){
    *p = str;
    double intpart = 0, decpart = 0;
    intpart = convert(p, radix);
    if(**p == '.') {
        ++*p;
        decpart = convert(p, radix);
        for(;decpart > 1; decpart /= radix);
    }
    return intpart + decpart;
}


vector<char> chs;
string radixfint(int decpart, int radix) {
    string res; 
	for(int i = '0'; i <= '9'; ++i)
		chs.push_back(i);
	for(int i = 'A'; i <= 'Z'; ++i)
		chs.push_back(i);
    vector<char> decchs;
    while(decpart > 0) {
        decchs.push_back(chs[decpart%radix]);
        decpart /= radix;
    }
    for(char i = decchs.size() - 1; i >= 0; --i)
        res.push_back(decchs[i]);
    return res;
}

string radixfnint(double nint, int radix) {
    string res;
    vector<char> chs;
	for(int i = '0'; i <= '9'; ++i)
		chs.push_back(i);
	for(int i = 'A'; i <= 'Z'; ++i)
		chs.push_back(i);
    for(int i = 0; i < 8; ++i) {
        nint *= radix;
        res.push_back(chs[(int)nint]);
        nint -= (int)nint;
    }    
    return res;
}

string radixfdec(double num, int radix) {
    bool ism = false;
    if(num < 0) {
        ism = true;
        num = -num;
    }
    //整数部分
    string rtn;
    rtn += radixfint((int)num, radix);
    num -= (int)num;
    if(rtn.size() == 0) rtn += "0";
    rtn += ".";
    rtn += radixfnint(num, radix);
    for(int i = rtn.size() - 1; i >= 0 && rtn[i] == '0'; --i) 
        rtn.pop_back();
    if(rtn[rtn.size() - 1] == '.') rtn.pop_back();
    return ism? "-"+rtn : rtn;
} 

double fun(double x,char op[],int *iop) {
    while (op[*iop-1]<32) //本行使得函数嵌套调用时不必加括号,如 arc sin(sin(1.234)) 只需键入arc sin sin 1.234<Enter>
        switch (op[*iop-1]) {
        case  7: x=sin(x);  (*iop)--;break;
        case  8: x=cos(x);  (*iop)--;break;
        case  9: x=tan(x);  (*iop)--;break;
        case 10: x=sqrt(x); (*iop)--;break;
        case 11: x=asin(x); (*iop)--;break;
        case 12: x=acos(x); (*iop)--;break;
        case 13: x=atan(x); (*iop)--;break;
        case 14: x=log10(x);(*iop)--;break;
        case 15: x=log(x);  (*iop)--;break;
        case 16: x=exp(x);  (*iop)--;break;
        }
    return x;
}

double calc(char *expr,char **addr) {
    static int deep; //递归深度
    static const char *fname[]={ "sin","cos","tan","sqrt","arcsin","arccos","arctan","lg","ln","exp",NULL};
    double ST[10]={0.0}; //数字栈
    char op[10]={'+'}; //运算符栈
    char c,*rexp,*pp,*pf;
    int ist=1,iop=1,last,i;
    if (!deep) {
        pp=pf=expr;
        do {
            c = *pp++;
            if (c!=' '&& c!=Tab){
                if(c >= 'A' && c <= 'Z')
                    if((c - 'A') >= state - 10) {
                        cout << "超出进制范围" << endl;
                        back = true;
                        return 0;
                    }
                *pf++ = c;
            }
        } while (c!='\0');
    }
    pp=expr;
    if ((c=*pp)=='-'||c=='+') {
        op[0] = c;
        pp++;
    }
    last = !DIGIT;
    while ((c=*pp)!='\0') {
        if (c=='(') {//左圆括弧
            deep++;
            ST[ist++]=calc(++pp,addr);
            deep--;
			if(back == true) return 0;
            ST[ist-1]=fun(ST[ist-1],op,&iop);
            pp = *addr;
            last = DIGIT;
            if ((*pp == '('||isalpha(*pp)) && strncmp(pp,"Pi",2)) {//目的是：当右圆括弧的右恻为左圆括弧或函数名字时，默认其为乘法
                op[iop++]='*';
                last = !DIGIT;
                c = op[--iop];
                goto operate ;
            }
        }
        else if (c==')') {//右圆括弧
            pp++;
            break;
        } else if (islower(c)) {
            if (!strncmp(pp,"pi",2)) {
                if (last==DIGIT) {
                    cout<< "π左侧遇）" <<endl; back = true; return 0;
                }
                ST[ist++]=4*atan(1);
                ST[ist-1]=fun(ST[ist-1],op,&iop);
                pp += 2;
                last = DIGIT;
                if (!strncmp(pp,"pi",2)) {
                    cout<< "两个π相连" <<endl; back = true; return 0;
                }
                if (*pp=='(') {
                    cout<< "π右侧遇（" <<endl;back = true; return 0;
                }
            }
			else if(!strncmp(pp,"ans",3)) {
                if (last==DIGIT) {
                    cout<< "ans左侧遇）" <<endl; back = true; return 0;
                }
                ST[ist++]=ans.res;
                ST[ist-1]=fun(ST[ist-1],op,&iop);
                pp += 3;
                last = DIGIT;
                if (!strncmp(pp,"ans",3)) {
                    cout<< "两个ans相连" <<endl; back = true; return 0;
                }
                if (*pp=='(') {
                    cout<< "ans右侧遇（" <<endl;back = true; return 0;
                }
			}
			else if(!strncmp(pp,"e",1)) {
                if (last==DIGIT) {
                    cout<< "e左侧遇）" <<endl; back = true; return 0;
                }
                ST[ist++]=2.718281828459;
                ST[ist-1]=fun(ST[ist-1],op,&iop);
                pp += 1;
                last = DIGIT;
                if (!strncmp(pp,"ANS",1)) {
                    cout<< "两个e相连" <<endl; back = true; return 0;
                }
                if (*pp=='(') {
                    cout<< "e右侧遇（" <<endl;back = true; return 0;
                }
			}

			else {
                for (i=0; (pf=(char *)fname[i])!=NULL; i++)
                    if (!strncmp(pp,pf,strlen(pf))) break;
                if (pf!=NULL) {
                    op[iop++] = 07+i;
                    pp += strlen(pf);
                } else {
                    cout<< "陌生函数名" <<endl;back = true; return 0;
                }
            }
        } else if (c=='+'||c=='-'||c=='*'||c=='/'||c=='^') {
            char cc;
            if (last != DIGIT) {
                cout<< "运算符粘连" <<endl;back = true; return 0;
            }
            pp++;
            if (c=='+'||c=='-') {
                do {
                    cc = op[--iop];
                    --ist;
                    switch (cc) {
                    case '+':  ST[ist-1] += ST[ist];break;
                    case '-':  ST[ist-1] -= ST[ist];break;
                    case '*':  ST[ist-1] *= ST[ist];break;
                    case '/':  ST[ist-1] /= ST[ist];break;
                    case '^':  ST[ist-1] = pow(ST[ist-1],ST[ist]);break;
                    }
                } while (iop);
                op[iop++] = c;
            } else if (c=='*'||c=='/') {
operate:        cc = op[iop-1];
                if (cc=='+'||cc=='-') {
                    op[iop++] = c;
                } else {
                    --ist;
                    op[iop-1] = c;
                    switch (cc) {
                    case '*':  ST[ist-1] *= ST[ist];break;
                    case '/':  ST[ist-1] /= ST[ist];break;
                    case '^':  ST[ist-1] = pow(ST[ist-1],ST[ist]);break;
                    }
                }
            } else {
                cc = op[iop-1];
                if (cc=='^') {
                    cout<< "乘幂符连用" <<endl;back = true; return 0;
                }
                op[iop++] = c;
            }
            last = !DIGIT;
        } else {
            if (last == DIGIT) {
                cout<< "两数字粘连" <<endl;back = true; return 0;
            }
           	ST[ist++]=strtodec(pp,&rexp, state);
            ST[ist-1]=fun(ST[ist-1],op,&iop);
            if (pp == rexp) {
                cout<< "非法字符" <<endl;back = true; return 0;
            }
            pp = rexp;
            last = DIGIT;
            if (*pp == '('||isalpha(*pp)) {
                op[iop++]='*';
                last = !DIGIT;
                c = op[--iop];
                goto operate ;
            }
        }
    }
    *addr=pp;
    if (iop>=ist) {
        cout<< "表达式有误" <<endl;back = true; return 0;
    }
    while (iop) {
        --ist;
        switch (op[--iop]) {
        case '+':  ST[ist-1] += ST[ist];break;
        case '-':  ST[ist-1] -= ST[ist];break;
        case '*':  ST[ist-1] *= ST[ist];break;
        case '/':  ST[ist-1] /= ST[ist];break;
        case '^':  ST[ist-1] = pow(ST[ist-1],ST[ist]);break;
        }
    }
    return ST[0];
}

vector<State> states;

int main(int argc,char **argv) {
    rl_attempted_completion_function = my_completion;
	cout.setf(ios::fixed);
	cout.precision(8);
    if (argc<=1) {
        char *buf;
        // if (GetConsoleOutputCP()!=936) system("chcp 936>NUL");//中文代码页
        //cout << "计算函数表达式的值。"<<endl<<"支持(),+,-,*,/,^,pi,e,sin,cos,tan,sqrt,arcsin,arccos,arctan,lg,ln,exp"<<endl;
        while (true) {
            cout << RESET;
            buf = readline(KRED "calc>" KRESET);
            add_history(buf);
		    rl_bind_key('\t',rl_complete);
		    back = false;
            memset(s, 0, sizeof(s)/sizeof(char));
            strncpy(s, buf, strlen(buf));
		//cin.getline(s, MAXLEN);
            if(s[0] == '\0') break;
			if (!strcmp((const char*)s, "history")) {
				if(states.size() == 0)
					cout << "null" << endl;
				else
					for(unsigned int i = 0; i < states.size(); ++i) 
						cout << CYAN << i << '\t' <<  BLUE << states[i].ex << NUNL << " = " << RESET << CYAN << states[i].display << RESET << '\t' << states[i].cmt << endl;
				continue;
			}
			if(!strncmp((const char*)s, "!", 1)) {
				char p = *(s+1);
				unsigned int hisindex = atoi(s+1);
				pn.ex = s;
				if(hisindex >= states.size() || p < '0' || p > '9')
					cout << "index error." << endl;
				else {
				pn.res = states[hisindex].res;
				cout << BLUE << s << NUNL << " = " << RESET << CYAN << pn.res << RESET << endl;
				ans = pn;
				states.push_back(pn);
				}
				continue;
			}
			if(!strncmp((const char*)s, "radix", 5)){
				stringstream ss;
				ss << s;
				ss.seekg(5, ios::beg);
				if(ss.str().size() > 5) {
				int rad;
				ss >> rad;
				if(rad < 2 || rad > 36) cout << "radix index fault." << endl;
				else state = rad;
				}
				else cout << BLUE << NUNL << "radix: " << RESET << CYAN << state << endl;
				continue;	
			}
            cout << BLUE  << s << NUNL <<" = " << RESET;
			pn.ex = s;
			double res = calc(s, &endss);
			stringstream ss; ss.str("");
			ss <<  res << endl;
			ss >> pn.res;
            pn.display = radixfdec(res, state);
			if(back)  
				pn.cmt = "非法输入参数";
			else {
				pn.cmt.clear();
				cout << CYAN << pn.display << RESET << endl;
			}
			ans = pn;
			states.push_back(pn);
        }
        free(buf);
    } else {
        if(strcmp(argv[1], "help") == 0) {
            fstream fhelp("/bin/calchelp");
            while (true) {
                string shelp;
                getline(fhelp, shelp);
                if(!shelp.size()) break;
                cout << shelp << endl;
            }
            fhelp.close();
    }
    }
    return 0;
}


 
 
static char** my_completion( const char * text , int start,  int end)
{
    char **matches;
 
    matches = (char **)NULL;
 
    if (start >= 0)
        matches = rl_completion_matches ((char*)text, &my_generator);
    else
        rl_bind_key('\t',rl_abort);
 
    return (matches);
 
}
 
char* my_generator(const char* text, int state)
{
    static int list_index, len;
    char *name;
 
    if (!state) {
        list_index = 0;
        len = strlen (text);
    }
   
    while (list_index < 13 && (name = cmd[list_index])) {
        list_index++;
 
        if (strncmp (name, text, len) == 0)
            return (dupstr(name));
    }
 
    /* If no names matched, then return NULL. */
    return ((char *)NULL);
 
}
 
char * dupstr (char* s) {
  char *r;
 
  r = (char*) xmalloc ((strlen (s) + 1));
  strcpy (r, s);
  return (r);
}
 
void * xmalloc (int size)
{
    void *buf;
 
    buf = malloc (size);
    if (!buf) {
        fprintf (stderr, "Error: Out of memory. Exiting.'n");
        exit (1);
    }
 
    return buf;
}
