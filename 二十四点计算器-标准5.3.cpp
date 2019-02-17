/*
分治算法：无论以何种方式达到24，一定有一步运算是在最后做的
枚举一下这是哪种运算，枚举一下它的位置，然后在它的左边和右边再继续做
如果左边或者右边有不止一个数字，那么还要添加括号
1.首先，通过枚举，把这四个数按照一定的次序排好
  用一个字符串陪着深搜，一开始，字符串为‘1234’表示塞四个编号的数字，中间没有运算符
2.然后，开始深搜——dfs(i,j,c)表示在第i个到第j个数之间，需要添加运算符,运算符为c 
  （1） 弄一个k，表示在第k个数和第k+1个数之间需要一个运算符，把k从i到j-1枚举
  （2）找定一个位置后，把自己加入字符串，然后数一数自己左边有多少个数字，如果数量大于1，那么加个括号，然后搜左边
       如果数量等于1，那么什么也不做
  （3）右边以此类推
3.通过深搜我们得到了一个表达式，然后运用表达式计算大法，看看结果是不是24，如果是，输出表达式，如果不是，清空再来

1.0版诞生：1.这是一个伟大的时刻 
2.0版更新：1.算法大改，修复近九成的bug 
2.5版更新：1.去除了很多重复的答案2.分类讨论更加全面，不会漏掉3.可以连续计算4.可以统计答案数量5.有提示语
2.7版更新：1.去除了重复的括号（这是一个大更新） 2.强制修复了双块生成式末尾莫名其妙的括号，从而去除更多重复答案 
2.9版更新：1.可以有中括号和大括号 2.加上了某些被误删的括号 
3.1版更新：1.专业治愈强迫症，所有答案按长度大小排列
3.5版更新：1.修改了字体颜色 2.新增格式错误检测系统 3.新增封面设计 
3.6版更新：1.提升兼容性，修复了窗口不匹配的bug,操作更加人性化 
4.6版更新：1.新增出题模式 2.界面修改，解决了答案较多时上面的答案看不到的bug 
4.7版更新：1.修复了出题模式中无法判别数字重复、数字多余的bug 
4.8版更新：1.修复了出题模式中某些正确的答案被误判的bug 2.修复了出题模式中无法判断四个数字是否都被使用完的bug 
4.9版更新：1.出题模式新增计分系统 2.出题模式可以显示答案的难度（虽然说对难度的判断并不是很严谨） 
5.1版更新：1.出题模式可以选择难度 2.各种界面美化 
5.2版更新：1.修复了一个有关括号的bug 2.修复了出题模式一个有关空格的bug 3.难度提升
5.3版更新：1.出题模式可以记录时间并显示平均用时 
*/ 
#include<bits/stdc++.h>
#include<windows.h>
using namespace std;				
int a[5],data[5];//data存四个数，a存枚举状态的4个数 
char d[501][101],b[101];  //d存目前为止的答案，防止重复(dnum为生成表达式中的数字）,b存生成的表达式 
string ans;
int topd=0,topb=0,dnum[501][5],dlength[501];
void gaikuohao(){//修改括号样式 
	//方法很简单，具体不说了
	for(int i=0;i<=topb;i++)
	  if(b[i]=='('){
	  	  int rpair,anotherpair=0,otherpair=0;
	  	  for(rpair=i+1;rpair<=topb;rpair++){
	  	  	  if(b[rpair]=='(') anotherpair++;
	  	  	  if(b[rpair]==')') anotherpair--;
	  	  	  otherpair=max(otherpair,anotherpair);
	  	  	  if(anotherpair<0) break;
	  	  }
	  	  if(otherpair==1) b[i]='[',b[rpair]=']';
	  	  if(otherpair>=2) b[i]='{',b[rpair]='}';
	  }
	return;
}
void qukuohao(){//去除括号操作 
	/*无论一个括号有多么复杂，它里面一定只包含最多两种东西：1.一个运算符  2.别的括号
	而它的外面一定有且只有一个运算符——要么在它前面，要么在它后面
	把括号内的运算符和括号外的等级进行对比，如果括号外的等级小于等于括号内的，就可以把这个括号删掉*/
	int rk[200];
	rk['+']=rk['-']=1;
	rk['*']=rk['/']=2;
	for(int i=0;i<=topb;i++)//注意：一定要先删左边的括号再删右边的括号，不然会导致混乱 
	  if(b[i]=='('){
	  	  int rpair,anotherpair=0;//rpair记录和我这个左括号匹配的右括号的位置,anotherpair可以防止别的括号的影响 
	  	  for(rpair=i+1;rpair<=topb;rpair++){
	  	  	  if(b[rpair]=='(') anotherpair++;
	  	  	  if(b[rpair]==')') anotherpair--;
			  if(anotherpair<0) break;
	  	  }
	  	  int symout,symin;//分别记录外面和里面的符号 
	  	  if(i>0 && (b[i-1]=='+' || b[i-1]=='-' || b[i-1]=='*' || b[i-1]=='/')) symout=i-1;
	  	  if(rpair<topb && (b[rpair+1]=='+' || b[rpair+1]=='-' || b[rpair+1]=='*' || b[rpair+1]=='/')) symout=rpair+1;
	  	  anotherpair=0;
	  	  for(symin=i+1;symin<rpair;symin++){
	  	  	  if(b[symin]=='(') anotherpair++;
	  	  	  if(b[symin]==')') anotherpair--;
	  	  	  if(anotherpair==0 &&(b[symin]=='+' || b[symin]=='-' || b[symin]=='*' || b[symin]=='/')) break;
	  	  }
	  	  if(rk[b[symin]]>=rk[b[symout]] && !(symout<i && (b[symout]=='/' || b[symout]=='-' && b[symin]!='*')))
	  	  	b[i]=' ',b[rpair]=' '; 
	  }
	for(int i=0;i<=topb;i++)
	  while(b[i]==' '){
	  	for(int j=i;j<99;j++) b[j]=b[j+1];
	  	    topb--;
	  	    if(i>topb) break;
	  }
	return;
}
double calc(double x,double y,char z){
		if(z=='+') return x+y;
		if(z=='-') return x-y;
		if(z=='*') return x*y;
		if(z=='/') return x/y;
}
void jisuan(){//表达式计算大法（顺便搞定输出） 
	double num[5];//数字栈 
	char sym[1001];//符号栈 
	int topnum=0,topsym=0;
	int rk[200];//存运算符的等级
	rk['+']=rk['-']=1;
	rk['*']=rk['/']=2;
	rk['(']=0;
	memset(num,0,sizeof(num));
	memset(sym,0,sizeof(sym));//必不可少的初始化 
	for(int i=0;i<=topb;i++){
		if(b[i]>='1' && b[i]<='4') num[++topnum]=(double)a[b[i]-48];
			else{
			    if(b[i]=='(') sym[++topsym]=b[i];//左括号直接入栈 
			    else if(b[i]==')'){//右括号一直弹到左括号 
				    while(topnum && topsym && sym[topsym]!='('){
				    	num[topnum-1]=calc(num[topnum-1],num[topnum],sym[topsym]);
					    topnum--,topsym--;
				    } 
				    topsym--;//把左括号也弹掉 
			    }
			         else{
			     	      while(topnum && topsym && rk[b[i]]<=rk[sym[topsym]]){
			     	          num[topnum-1]=calc(num[topnum-1],num[topnum],sym[topsym]);
			     	          topnum--,topsym--;
			     	      }
			     	     sym[++topsym]=b[i];
			         }
		    }
	}
	while(topsym){
		num[topnum-1]=calc(num[topnum-1],num[topnum],sym[topsym]);
		topnum--,topsym--;
	}
	if(abs(num[1]-24)<0.000001){//复杂的输出操作 
	    qukuohao();//去除多余的括号
	    gaikuohao();//修改括号样式 
		//接下来是去重环节 
		topd++;
		for(int i=1;i<=4;i++) dnum[topd][i]=a[i];
		for(int i=0;i<=100;i++) d[topd][i]=b[i];
		dlength[topd]=topb;
		bool f=false;
		for(int i=1;i<topd;i++){
			if(f) break;
			for(int j=0;j<=100;j++){
				if(d[i][j]>='1' && d[i][j]<='4' && b[j]>='1' && b[j]<='4'){
					if(dnum[i][d[i][j]-48]!=a[b[j]-48]) break; 
				}
		  	    else if(d[i][j]!=b[j]) break;
		  	    if(j==100){
		  	  	    f=true;
		  	  	    topd--;
		  	    }
		    }
		}
		if(f) return;
    }
	return;
}
bool jisuan2(){//出题模式检测答案的表达式计算 
    ans=ans+')';
    ans='('+ans;
    int len=ans.size();
    double num[5];//数字栈 
	char sym[1001];//符号栈 
	int topnum=0,topsym=0;
	int rk[200];//存运算符的等级
	rk['+']=rk['-']=1;
	rk['*']=rk['/']=2;
	rk['(']=0;
	memset(num,0,sizeof(num));
	memset(sym,0,sizeof(sym));//必不可少的初始化 
	double k=0;
	bool vis[5]={0};
	for(int i=0;i<len;i++){
	    if(ans[i]>='0' && ans[i]<='9') k=k*10+(ans[i]-48); 
	    else{
	        if(i>0 && ans[i-1]>='0' && ans[i-1]<='9') num[++topnum]=k;
	        if(i>0 && ans[i-1]>='0' && ans[i-1]<='9'){
	        	bool cant=true;
	            for(int j=1;j<=4;j++){
	        	    if(k==data[j] && !vis[j]){
	          		    cant=false;
	          			vis[j]=true;
	          			break;
	          		}
	       		}
	       		if(cant) return false;
	        }
            k=0;
            if(ans[i]=='(') sym[++topsym]=ans[i];//左括号直接入栈 
	        else if(ans[i]==')'){//右括号一直弹到左括号 
	            while(topnum && topsym && sym[topsym]!='('){
    	            num[topnum-1]=calc(num[topnum-1],num[topnum],sym[topsym]);
	                topnum--,topsym--;
                } 
                topsym--;//把左括号也弹掉 
	        }
            else{
                while(topnum && topsym && rk[ans[i]]<=rk[sym[topsym]]){
 	                num[topnum-1]=calc(num[topnum-1],num[topnum],sym[topsym]);
	                topnum--,topsym--;
     	        }
   	            sym[++topsym]=ans[i];
            }
        }
    }
    while(topsym){
		num[topnum-1]=calc(num[topnum-1],num[topnum],sym[topsym]);
		topnum--,topsym--;
	}
	for(int i=1;i<=4;i++) if(!vis[i]) return false; 
    if(abs(num[1]-24)<0.000001) return true;
    return false;
}
void dfs(int x,int y,char c){
	//开始寻找运算符应该放的位置 
	for(int k=x;k<=y-1;k++){//e[k]+1为当前需要加符号的位置 
	   int e[5];
	   for(int i=x;i<=y;i++)
	     for(int j=0;j<=topb;j++)
	       if(b[j]==i+48){//寻找从x到y所有编号的数的位置 
	    	   e[i]=j;
	           break;
	       }
	    char g[101];
	    for(int i=0;i<=100;i++) g[i]=b[i];//回退回来的时候进行复原
	    int topg=topb;
		//放运算符 
	    topb++;
	    for(int i=topb;i>=e[k]+2;i--) b[i]=b[i-1];//留出一个位置来放运算符
		b[e[k]+1]=c;
		for(int i=y;i>k;i--) e[i]++;//因为加了一个运算符进去，所以后面的数的位置要变动 
		if(k-x+1>1){//如果左边还有的多
		    //在左边添加一对括号 
			topb++;
			for(int i=topb;i>=e[x]+1;i--) b[i]=b[i-1];
			b[e[x]]='(';
			for(int i=x;i<=y;i++) e[i]++;//因为加了一个左括号进去，所以大家的位置都要变动了 
			topb++;
			for(int i=topb;i>=e[k]+2;i--) b[i]=b[i-1];
			b[e[k]+1]=')';
			for(int i=k+1;i<=y;i++) e[i]++;
			int g2[100];
			for(int i=0;i<=100;i++) g2[i]=b[i];
			int topg2=topb;
			for(char p='*';p<='/';p++){//枚举左边应该添加哪种运算符（在+-*/四个运算中，*的ascii最小，/最大 
				if(!(p=='+' || p=='-' || p=='*' || p=='/')) continue;//这四个运算符并不是连续的，中间还夹了很多杂七杂八的符号 
				dfs(x,k,p);
				if(y-k>1){//如果在左边有的多的情况下，右边也有的多 
					for(int i=x;i<=y;i++)
				      for(int j=0;j<=topb;j++)
				        if(b[j]==i+48) e[i]=j;//经过了前面的dfs，所有的东西都要变动
				    //在右边添加一对括号  注意：由于已经在左边添加过一对括号，所以此时运算符的位置是e[k]+2 
				    topb++;
			        for(int i=topb;i>=e[k]+4;i--) b[i]=b[i-1];
			        b[e[k]+3]='(';
			        for(int i=k+1;i<=y;i++) e[i]++;
			        topb++;
			        for(int i=topb;i>=e[y]+2;i--) b[i]=b[i-1];
			        b[e[y]+1]=')';
			        int g3[100];
			        for(int i=0;i<=100;i++) g3[i]=b[i];
			        int topg3=topb;
			        for(char p2='*';p2<='/';p2++){
			        	if(!(p2=='+' || p2=='-' || p2=='*' || p2=='/')) continue;
			        	b[100]=0;//这是为了强制处理那个莫名其妙的bug 
			        	dfs(k+1,y,p2);
			        	for(int i=0;i<=100;i++) b[i]=g3[i];
						topb=topg3;//复原，准备下一次循环 
			        }
				}
				for(int i=0;i<=100;i++) b[i]=g2[i];
				topb=topg2;//复原，准备下一次循环	 
			}
		} 
		if(y-k>1){//如果仅仅右边有的多 
		    //在右边添加一对括号 
			topb++;
			for(int i=topb;i>=e[k]+3;i--) b[i]=b[i-1];
			b[e[k]+2]='(';
			for(int i=k+1;i<=y;i++) e[i]++;
			topb++;
			for(int i=topb;i>=e[y]+2;i--) b[i]=b[i-1];
			b[e[y]+1]=')';
			char g2[100];
			for(int i=0;i<=100;i++) g2[i]=b[i];
			int topg2=topb;
			for(char p='*';p<='/';p++){
				if(!(p=='+' || p=='-' || p=='*' || p=='/')) continue;
				dfs(k+1,y,p);
				for(int i=0;i<=100;i++) b[i]=g2[i];
				topb=topg2;//复原 
			}
		}
	    if(k-x+1==1 && y-k==1){//如果左右两边都没得多，那么说明可能已经有一个表达式成功生成了 
	    	bool really=true;//判断这是不是真的有表达式生成的依据：每两个数之间是否都已经有了运算符 
	    	for(int i=1;i<=4;i++)//这次得把全部的数字的位置都求出来 
	    	  for(int j=0;j<=topb;j++)
	    	    if(b[j]==i+48){
	    	    	e[i]=j;
	    	    	break;
	    	    }
	    	for(int i=1;i<4;i++){
	    		bool really2=false;
	    		for(int j=e[i];j<=e[i+1];j++)
	    			if(b[j]=='+' || b[j]=='-' || b[j]=='*' || b[j]=='/'){
	    	      	  really2=true;
	    	      	  break;
	    		    }
	    	    if(really2==false) really=false;
	    	}
	    	if(really) jisuan();//算24点去喽 
	    }
	    if(k-x+1>1 || y-k>1){//两边只有一个的时候不用复原 
	    	for(int i=0;i<=100;i++) b[i]=g[i];
	        topb=topg;//复原，准备下一次循环 
	    }
	}
}
void px(){//总指挥官 
	memset(b,0,sizeof(b));
	b[0]='1',b[1]='2',b[2]='3',b[3]='4',topb=3;//数字为几代表这里塞的是几号数字 
	dfs(1,4,'+');
	memset(b,0,sizeof(b));
	b[0]='1',b[1]='2',b[2]='3',b[3]='4',topb=3;
	dfs(1,4,'-');
	memset(b,0,sizeof(b));
	b[0]='1',b[1]='2',b[2]='3',b[3]='4',topb=3;
	dfs(1,4,'*');
	memset(b,0,sizeof(b));
	b[0]='1',b[1]='2',b[2]='3',b[3]='4',topb=3;
	dfs(1,4,'/');
}
bool check(string s){
	for(int i=0;i<s.size();i++) 
	  if(s[i]<'0' || s[i]>'9') return false;
	return true;
}
bool checkans(){//出题模式中检查输入的答案的格式 
    for(int i=0;i<ans.size();i++){
        if(ans[i]=='[' || ans[i]=='{') ans[i]='(';//把所有括号统一换成小括号 
        if(ans[i]==']' || ans[i]=='}') ans[i]=')';
        if(ans[i]==' '){  
            if(i>0 && i<ans.size()-1 && ans[i-1]>='0' && ans[i-1]<='9' && ans[i+1]>='0' && ans[i-1]<='9') return false;//不予许数字中间出现空格 
	 	    ans.erase(i,1);i--;//删除空格 
	    }
    }
    for(int i=0;i<ans.size();i++){
        if((ans[i]<'0' || ans[i]>'9') && ans[i]!='+' && ans[i]!='-' && ans[i]!='*' && ans[i]!='/' && ans[i]!='(' && ans[i]!=')') return false;//杂七杂八的字符
        if(ans[i]=='0' && (i==0 || ans[i-1]<'0' || ans[i-1]>'9') && i<ans.size()-1 && ans[i+1]>='0' && ans[i+1]<='9') return false;//前导零 
        if((ans[i]=='+' || ans[i]=='-' || ans[i]=='*' || ans[i]=='/') && (i==0 || ans[i-1]=='+' || ans[i-1]=='-' || ans[i-1]=='*' || ans[i-1]=='/')) return false;//符号重复
        if(i>0 && ans[i]=='(' && ans[i-1]==')') return false;//玄学错误 
        if(i<ans.size()-1 && ans[i]=='(' && (ans[i+1]=='+' || ans[i+1]=='-' || ans[i+1]=='*' || ans[i+1]=='/')) return false;//左括号的右边有运算符 
        if(i>0 && ans[i]==')' && (ans[i-1]=='+' || ans[i-1]=='-' || ans[i-1]=='*' || ans[i-1]=='/')) return false;//右括号的左边有运算符 
    }
    int top=0;
    for(int i=0;i<ans.size();i++){//检测括号是否匹配 
        if(ans[i]=='(') top++;
        if(ans[i]==')'){
            if(top>0) top--;
            else return false;
        }
	}
	if(top) return false;
    return true;
}
int zh(string s){
	int sum=0;
	for(int i=0;i<s.size();i++)
	  sum=(sum<<1)+(sum<<3)+(s[i]^48);
	return sum;
}
int main(){
	About_first:
	char CH;
	goto first_page;
	About:
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
	system("cls");
	puts("24_Points Calculator was first invented by Wang Zhijian on 2018.10.20.");
	puts("");
	puts("Name:                     24-Points Calculator STD-5.3");
	puts(""); 
	puts("Main functions:           Questioning and calculating");
	puts("");
	puts("Developers:               Wang Zhijian and Xu Xing");
	puts("");
    puts("Developing environment:   Dev-C++ 5.6.1  g++ 4.9.8  Windows 10");
    puts("");
	puts("Visions:                  STD-1.0 STD-2.0 STD-2.5 STD-2.7 STD-2.9 STD-3.1 STD-3.5 STD-3.6 SIM-3.6");
    puts("                          STD-4.6 STD-4.7 STD-4.8 STD-4.9 STD-5.1 STD-5.2 STD-5.3");
    puts(""); 
    puts("Suggested processor:      Intel Core i5-6200U CPU @ 2.30GHZ 2.40GHZ, 64-bit");
    puts("");
    puts("Suggested memory(RAM):    >=4.00GB");
    puts("");
    puts("Suggested display size:   1366*768");
    puts("");
    puts("Suggested system:         Windows 10");
	puts("");
	puts("");
	puts("copyright(c) 2018 Wang Zhijian Xu Xing. All rights reserved.");
	puts(""); 
	puts("");
	puts("");
    puts("Press Enter to continue:");
    CH=getchar();
    while(CH!='\n') CH=getchar();
	first_page:
	system("cls");
	system("mode con cols=120 lines=45");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
    puts("Ver:STD-5.3");
	puts("  ____________                                                                                                        ");
	puts("            ||   ||        ||         	  ||======                                                                    ");
	puts("            ||   ||        ||             ||      ==                                                                  ");
	puts("            ||   ||        ||             ||       ==        ====          ==                                         ");
	puts("            ||   ||        ||             ||        ==     ==    ==        ==                         ||              ");
	puts("            ||   ||        ||             ||       ==     ==      ==                ==========     ========    =====  ");
	puts(" ____________    ___________              ||      ==     ==        ==      ||       ||      ||        ||      ||      ");
	puts(" ||                        ||   =======   ||======      ==          ==     ||       ||      ||        ||      ||      ");
	puts(" ||                        ||      	  ||             ==        ==      ||       ||      ||        ||      ======  ");
	puts(" ||                        ||             ||              ==      ==       ||       ||      ||        ||           || ");
	puts(" ||                        ||             ||               ==    ==        ||       ||      ||        ||           || ");
	puts(" ____________              ||             ||                 ====          ||       ||      ||        ||      ======  ");
	puts("");
	puts("");
	puts("                                             ");
	puts("");
	puts("");
	puts("");
	puts("   									-------Developed by Wang Zhijian & Xu Xing");
	puts("");
	puts("");
	puts("");
	puts("");
	puts("");
	puts("");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_GREEN);
	puts("You can input 'About' to view the information about the software.");
	puts("You can also input 's' to start your game.");
	first_input:
    string input_first;
    getline(cin,input_first,'\n');
    if(input_first=="s") goto pat;
    else if(input_first=="About") goto About;
    else goto first_input;
	pat:
    system("cls");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
	puts("Which pattern would you like to play?");
	puts("Please input 'a' for problem set pattern, and 'c' for calculating pattern");
	format:
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	string pattern;
	getline(cin,pattern,'\n');
	while(pattern=="") getline(cin,pattern,'\n');
	if(pattern=="return") goto first_page;
	if(pattern!="c" && pattern!="a"){
	    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_RED);
	    puts("Format Error! Please input again!");
	    goto format;
    }
    if(pattern=="c")
        while(1){
        	system("cls");
        	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_GREEN);
        	cout<<"Input your four numbers:"<<endl;
        	puts("(If you don't want to calculate, you can input 'return' to choose the pattern again)");
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
        	topd=0,topb=0;
        	string s1,s2,s3,s4;
        	cin>>s1;
        	if(s1=="return") goto pat;
        	cin>>s2;
        	if(s2=="return") goto pat;
        	cin>>s3;
        	if(s3=="return") goto pat;
        	cin>>s4;
        	if(s4=="return") goto pat;
        	if(!check(s1) || !check(s2) || !check(s3) || !check(s4)){
        		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_RED);
        		puts("Format Error! Please input again!");
        		Sleep(1500);
        		continue;
        	}
        	data[1]=zh(s1),data[2]=zh(s2),data[3]=zh(s3),data[4]=zh(s4);
            for(int i=1;i<=4;i++)
              for(int j=1;j<=4;j++)
                for(int k=1;k<=4;k++)
                  for(int t=1;t<=4;t++)
                    if(i!=j && i!=k && i!=t && j!=k && j!=t && k!=t){
                    	a[1]=data[i],a[2]=data[j],a[3]=data[k],a[4]=data[t];
                	    px();
                    }
            if(!topd){
            	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_RED);
            	cout<<"No Answer"<<endl<<endl;
            }
            else{
          	    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            	cout<<endl;
            	for(int i=1;i<topd;i++)
            	  for(int j=i+1;j<=topd;j++)
            	    if(dlength[i]>dlength[j]){
            	    	swap(d[i],d[j]);//swap可以直接交换数组，非常强大 
    					swap(dnum[i],dnum[j]);
            	    	swap(dlength[i],dlength[j]);
            	    }
    	        int t=0;
    			for(int i=1;i<=topd;i++){
    			    t++;
    				for(int j=0;j<=20;j++)
    			  	  if(d[i][j]>='1' && d[i][j]<='4') cout<<dnum[i][d[i][j]-48];
    			      else cout<<d[i][j];
    			    cout<<" ";
    			    if(t%4==0) cout<<endl;
    			}
   			    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_BLUE);
                cout<<endl<<"There are "<<topd<<" answers."<<endl<<endl;
    			if(topd>=100){
    				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
    				cout<<"That's awful!"<<endl<<endl;
    			}
            }
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_GREEN);
            puts("Press Enter to continue...");
            char c=getchar();
            c=getchar();
    		while(c!='\n') c=getchar();
        }
        else{
            srand(time(0));
            input_difficulty_again:
            system("cls");
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_GREEN);
            puts("Are you ready? The computer is going to challenge you...");
            puts("(If you don't want to play, please input 'return' to choose the pattern again.)");
            puts("");
            puts("");
            puts("Please input your difficulty: (Simple/Medium/Hard/Random)");
            input_difficulty:
            int ac=0,wa=0,giveup=0,sum=0;
            double totaltime=0;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_GREEN);
            string difficulty;
            getline(cin,difficulty,'\n');
            if(difficulty=="return") goto pat;
            if(difficulty!="Simple" && difficulty!="Medium" && difficulty!="Hard" && difficulty!="Random"){
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_RED);
                puts("Format Error! Please input again:");
                goto input_difficulty;
            }
            while(1){
                system("cls");
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_GREEN);
                problem:
                data[1]=rand()%13+1,data[2]=rand()%13+1,data[3]=rand()%13+1,data[4]=rand()%13+1;
                memset(d,0,sizeof(d));
                memset(b,0,sizeof(b));
                memset(a,0,sizeof(a));
                topd=0;
                for(int i=1;i<=4;i++)
                  for(int j=1;j<=4;j++)
                    for(int k=1;k<=4;k++)
                      for(int t=1;t<=4;t++)
                        if(i!=j && i!=k && i!=t && j!=k && j!=t && k!=t){
                    	    a[1]=data[i],a[2]=data[j],a[3]=data[k],a[4]=data[t];
                	        px();
                        }
                if(!topd) goto problem;
                input:
				clock_t start=clock();
				input_without_stop:
                system("cls");
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                string tdifficulty;
                if(topd<=5) tdifficulty="Hard";
                else if(topd<=20) tdifficulty="Medium";
                else tdifficulty="Simple";
                if(data[1]+data[2]+data[3]+data[4]==24) tdifficulty="Simple";
                if(tdifficulty!=difficulty && difficulty!="Random") goto problem;
                sum++;
                printf("%d %d %d %d\n",data[1],data[2],data[3],data[4]);
                cout<<"Difficulty:"<<tdifficulty<<endl;
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_GREEN);
                printf("Please calculate and input your answer:                                            Correct:%d  Wrong:%d\n",ac,wa);
                printf("You can input 'I failed' to view the answers.                                               Give up:%d\n",giveup);
                if(totaltime==0) printf("You can input 'return' to choose the difficulty again.                                 Average time:N/A\n");
                else printf("You can input 'return' to choose the difficulty again.                                 Average time:%.2lfs\n",totaltime/(double)(sum-1));
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
                getline(cin,ans,'\n');
                if(ans=="return") goto input_difficulty_again;
                if(ans=="I failed"){
                	 giveup++;sum--;
                     SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
               	     cout<<endl;
               	     for(int i=1;i<topd;i++)
            	       for(int j=i+1;j<=topd;j++)
            	         if(dlength[i]>dlength[j]){
    	    	             swap(d[i],d[j]);//swap可以直接交换数组，非常强大 
    					     swap(dnum[i],dnum[j]);
            	    	     swap(dlength[i],dlength[j]);
            	         }
               	     int t=0;
    			     for(int i=1;i<=topd;i++){
			             t++;
    				     for(int j=0;j<=20;j++)
                           if(d[i][j]>='1' && d[i][j]<='4') cout<<dnum[i][d[i][j]-48];
    			           else cout<<d[i][j];
    			         cout<<" ";
    			         if(t%4==0) cout<<endl;
    			     }
   			        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_BLUE);
                    cout<<endl<<"There are "<<topd<<" answers."<<endl<<endl;
    			    if(topd>=100){
    				    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
    				    cout<<"That's awful!"<<endl<<endl;
    			    }
          			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_GREEN);
          			puts("Make persistent efforts!");
          			puts("Press enter to continue...");
          			char c=getchar();
          			while(c!='\n') c=getchar();
          			goto problem;
                }
                if(!checkans()){
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_RED);
            		puts("Format Error! Please input again!");
            		Sleep(1500);
            		goto input_without_stop;
                }
                if(jisuan2()){
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_GREEN);
                    puts("Congratulations! The answer is correct.");
                    double time=(clock()-start)*1000.0/CLOCKS_PER_SEC/1000.0;
                    printf("\ntime: %.2lfs\n",time);
                    if(time<=10) puts("So fast!");
                    puts("");
                    ac++;totaltime+=time;
                    puts("Press anykey to continue...");
                    char ch=getchar();
                }
                else{
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_RED);
                    puts("Wrang answer! Please input again!");
                    wa++;
                    Sleep(1500);
                    goto input_without_stop;
                }
            }
        }	
    return 0;
} 
