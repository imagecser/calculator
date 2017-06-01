
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
用法：
如果要求2的32次幂，可以打入2^32<回车>
如果要求30度角的正切可键入tan(Pi/6)<回车>
注意不能打入：tan(30)<Enter>
如果要求1.23弧度的正弦，有几种方法都有效：
sin(1.23)<Enter>
sin 1.23 <Enter>
sin1.23  <Enter>
如果验证正余弦的平方和公式,可打入sin(1.23)^2+cos(1.23)^2 <Enter>或sin1.23^2+cos1.23^2 <Enter>
此外两函数表达式连在一起,自动理解为相乘如：sin1.23cos0.77+cos1.23sin0.77就等价于sin(1.23)*cos(0.77)+cos(1.23)*sin(0.77)
当然你还可以依据三角变换，再用sin(1.23+0.77)也即sin2验证一下。
本计算器充分考虑了运算符的优先级因此诸如：2+3*4^2 实际上相当于：2+(3*(4*4))
另外函数名前面如果是数字,那么自动认为二者相乘.
同理，如果某数的右侧是左括号，则自动认为该数与括弧项之间隐含一乘号。
如：3sin1.2^2+5cos2.1^2 相当于3*sin2(1.2)+5*cos2(2.1)
又如：4(3-2(sqrt5-1)+ln2)+lg5 相当于4*(3-2*(√5 -1)+loge(2))+log10(5)
此外，本计算器提供了圆周率 Pi键入字母时不区分大小写,以方便使用。
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
            if (!strncmp(pp,"Pi",2)) {
                if (last==DIGIT) {
                    cout<< "π左侧遇）" <<endl; back = true; return 0;
                }
                ST[ist++]=3.14159265358979323846264338328;
                ST[ist-1]=fun(ST[ist-1],op,&iop);
                pp += 2;
                last = DIGIT;
                if (!strncmp(pp,"Pi",2)) {
                    cout<< "两个π相连" <<endl; back = true; return 0;
                }
                if (*pp=='(') {
                    cout<< "π右侧遇（" <<endl;back = true; return 0;
                }
            }
			else if(!strncmp(pp,"ANS",3)) {
                if (last==DIGIT) {
                    cout<< "ANS左侧遇）" <<endl; back = true; return 0;
                }
                ST[ist++]=ans;
                ST[ist-1]=fun(ST[ist-1],op,&iop);
                pp += 3;
                last = DIGIT;
                if (!strncmp(pp,"ANS",3)) {
                    cout<< "两个ANS相连" <<endl; back = true; return 0;
                }
                if (*pp=='(') {
                    cout<< "ANS右侧遇（" <<endl;back = true; return 0;
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
    if (argc<=1) {
        // if (GetConsoleOutputCP()!=936) system("chcp 936>NUL");//中文代码页
        cout << "计算函数表达式的值。"<<endl<<"支持(),+,-,*,/,^,Pi,sin,cos,tan,sqrt,arcsin,arccos,arctan,lg,ln,exp"<<endl;
        while (1) {
			back = false;
            cout << "请输入表达式：";
            // gets(s);
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
				cout << s << " = " << setprecision(15) << pn.res << endl;
				ans = pn.res;
				states.push_back(pn);
				}
				continue;
			}	
            cout << s <<" = ";
			pn.ex = s;
			double res = calc(s, &endss);
			stringstream ss; ss.str("");
			ss << setprecision(15) << res << endl;
			ss >> pn.res;
			if(back)  
				pn.cmt = "非法输入参数";
			else {
				pn.cmt.clear();
				cout << setprecision(15) << res << endl;
			}
			ans = pn.res;
			states.push_back(pn);
        }
    } else {
        strncpy(s,argv[1],MAXLEN-1);s[MAXLEN-1]=0;
        if (argc>=3) {
            pcs=atoi(argv[2]);
        } else {
            printf("%.15lg\n",calc(s,&endss));
        }
    }
    return 0;
}
