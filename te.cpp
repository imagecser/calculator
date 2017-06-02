
/*---------------------------------------
功能：
目前提供了10多个常用数学函数:
    ⑴正弦sin
    ⑵余弦cos
    ⑶正切tan
    ⑷开平方sqrt
    ⑸反正弦arcsin
    ⑹反余弦arccos
    ⑺反正切arctan
    ⑻常用对数lg
    ⑼自然对数ln
    ⑽ｅ指数exp
    ⑾乘幂函数∧
----------------------------------------*/

#include <string>
#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <sstream>
#include <cmath>
#include <cstdio>
using namespace std;
#define MAGENTA "\033[35m"      /* Magenta */
const char Tab=0x9;
const int  DIGIT=1;
const int MAXLEN=16384;

struct State {
	string ex;
	double res;
	string cmt;
}pn;
char s[MAXLEN],*endss;
bool back;
int state = 10;
int pcs=15;
double ans=0;



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
            if (c!=' '&& c!=Tab)
                *pf++ = c;
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
        } else if (isalpha(c)) {
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
                ST[ist++]=ans;
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
           	ST[ist++]=strtod(pp,&rexp);
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
	cout.setf(ios::fixed);
	cout.precision(8);
    if (argc<=1) {
        // if (GetConsoleOutputCP()!=936) system("chcp 936>NUL");//中文代码页
        cout << "计算函数表达式的值。"<<endl<<"支持(),+,-,*,/,^,pi,e,sin,cos,tan,sqrt,arcsin,arccos,arctan,lg,ln,exp"<<endl;
        while (1) {
			back = false;
            cout << "请输入表达式：";
			cin.getline(s, MAXLEN);
            if (s[0]==0) break;//
			if (!strcmp((const char*)s, "history")) {
				if(states.size() == 0)
					cout << "null" << endl;
				else
					for(unsigned int i = 0; i < states.size(); ++i) 
						cout << i << '\t' << states[i].ex << " = " << states[i].res << '\t' << states[i].cmt << endl;
				continue;
			}
			if(!strncmp((const char*)s, "!", 1)) {
				char p = *(s+1);
				unsigned int hisindex = atoi(s+1);
				pn.ex = s;
				if(hisindex >= states.size() || p < '0' || p > '9')
					cout << "index error." << endl;
				else {
				// pn.res = states[hisindex].res;
				cout << s << " = " << pn.res << endl;
				ans = pn.res;
				states.push_back(pn);
				}
				continue;
			}
			if(!strncmp((const char*)s, "radix", 5)){
				stringstream ss;
				ss << s;
				ss.seekg(5, ios::beg);
				int rad;
				ss >> rad;
				if(rad < 2) cout << "radix index fault." << endl;
				continue;	
			}
            cout << s <<" = ";
			pn.ex = s;
			double res = calc(s, &endss);
			stringstream ss; ss.str("");
			ss << res << endl;
			ss >> pn.res;
			if(back)  
				pn.cmt = "非法输入参数";
			else {
				pn.cmt.clear();
				cout << res << endl;
			}
			ans = pn.res;
			states.push_back(pn);
        }
    } else {
        strncpy(s,argv[1],MAXLEN-1);s[MAXLEN-1]=0;
        if (argc>=3) {
            pcs=atoi(argv[2]);
        } else {
            printf("%.8lg\n",calc(s,&endss));
        }
    }
    return 0;
}
