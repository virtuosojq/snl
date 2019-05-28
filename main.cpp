#include<cstdio>
#include<cstdlib>
#include<string>
#include<iostream>
#include<fstream>
#include<vector>
#include<set>
#include<stack>
#include<map>
#include<algorithm>
using namespace std;
 int error=0;//标志位，判断词法分析是对是错

string rword[21]={ "begin","integer","char","program","array","of","record","end","var","procedure"
		,"if","then","else","fi","while","do","endwh","read","write","return","type" };
//保留字表

struct token
{
    token(int a,string b,string d,int c)//没有构造函数会报错
    {
        this->line=a;
        this->lex=b;
        this->pos=c;
        this->uplex=d;
    }
    int line;//行数
    string lex;//词法信息
    string uplex;//大写词法信息标志
    int pos;//语义信息
};//保留字的pos为-1,分界符的pos=-2,错误的pos为-3,注释为-4

vector<token> tokenlist;//保存所有的token序列
vector<string> idTable;//标识符表
vector<string> intcTable;//整数表
 int line=1;//行数初始化为1

 vector<string>data;//输入数据，全为终极符
 vector <int> row;//表示行数，用于语法分析中的错误显示
 stack<string>sem;//分析栈
 map< int,vector<string> >Predicts;//保存predict集
 map< string,map<string,int> >LL1;//ll1分析表，由

set<string> Vn,Vt;  //保存终极符和非终极符
vector<string> Left;  //保存产生式的左部
vector< vector<string> >Right;  //保存产生式的右部
 //词法分析
 //****
bool isLetter(char ch)
{
    if(ch>='A'&&ch<='Z') return true;
    else if(ch>='a'&&ch<='z') return true;
    else  return false;
}

bool isDigit(char ch)
{
    if(ch>='0'&&ch<='9') return true;
    else return false;
}

bool isOther(char ch)
{
   	if (ch >= 'A'&&ch <= 'Z')
		return false;
	if (ch >= 'a'&& ch <= 'z')
		return false;
	if (ch >= '0'&& ch <= '9')
		return false;
	return true;
}

bool reservedLookup(string str)
{
    for(int i=0;i<21;i++)
    {
        if(str==rword[i])
        {
            return true;
        }
    }
    return false;
}

int addId(string str)
{
    for(int i=0;i<idTable.size();i++)
    {
        if(str==idTable[i])
        {
            return i;
        }
    }
    idTable.push_back(str);
    return idTable.size()-1;
}

int addIn(string str)
{
    for(int i=0;i<intcTable.size();i++)
    {
        if(str==intcTable[i])
        {
            return i;
        }
    }
    intcTable.push_back(str);
    return intcTable.size()-1;
}

token * scan(FILE *f)
{
  	char ch;
	string str = "";//初始为空串
	token * t = NULL;

	ch = fgetc(f);
    if(ch!=EOF)
    {
        while(ch==' '||ch == '\t'||ch=='\r'||ch=='\n')//消除空格
        {
            if(ch=='\n')
            {
                line++;
            }
            ch=fgetc(f);
        }


        if(isLetter(ch))//第一个是字母
        {
         str+=ch;
         ch=fgetc(f);//往后看
         while(isLetter(ch)||isDigit(ch))
         {
             str+=ch;
             ch=fgetc(f);
         }
         if(isOther(ch))
         {
             ungetc(ch,f);//回退
             if(reservedLookup(str))
             {
                 string strA=str;
                 transform(strA.begin(), strA.end(), strA.begin(), ::toupper);
                 t=new token(line,str,strA,-1);
                 return t;
             }
             else
             {
                 int id=addId(str);
                 string temp="ID";
                 t=new token(line,str,temp,id);
                 return t;
             }
         }
        }


        if(isDigit(ch))
        {
            str+=ch;
            ch=fgetc(f);
            while(isDigit(ch))
            {
                str+=ch;
                ch=fgetc(f);
            }
            if(!isDigit(ch))
            {
                ungetc(ch,f);
                int id=addIn(str);
                string temp="INTC";
                t=new token(line,str,temp,id);
                return t;
            }
        }


        if(ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '(' ||ch=='<'||ch==','
			||ch == ')' || ch == ';' || ch == '[' || ch == ']' || ch == '=')//单分界符
			//除去.和:
			{
			    str+=ch;
			    t=new token(line,str,str,-2);
			    return t;
			}

        if(ch=='.')
        {
            str+=ch;
            ch=fgetc(f);
            if(ch=='.')
            {
                str+=ch;
                t=new token(line,str,str,-2);
                return t;
            }
            else
            {
                ungetc(ch,f);
                t=new token(line,str,str,-2);
                return t;
            }
        }

        if(ch==':')
        {
            str+=ch;
            ch=fgetc(f);
            if(ch=='=')
            {
                str+=ch;
                t=new token(line,str,str,-2);
                return t;
            }
            else
            {
                    ungetc(ch,f);
                    t=new token(line,str,str,-3);
                    printf("第%d行：error  %s",line,str.c_str());
            }
        }

/*        if(ch=='\'')//单引号,防止二义性
        {
            str+=ch;
            ch=fgetc(f);
            if(isLetter(ch))
            {
                ch=fgetc(f)
                    if(ch=='\'')
            }
        }*/
        if(ch=='{')//注释换行暂时不考虑
            {
             ch=fgetc(f);
             while(ch!='}')
                    {
                        str+=ch;
                        ch=fgetc(f);
                    }
            // printf("注释：%s",str.c_str());
             t=new token(line,str,str,-4);

             return t;
            }
            str+=ch;
           t=new token(line,str," ",-3);//假设以上条件一个都不符合，返回错误

            return t;
        }
        else
        {
            return NULL;
        }
}
//语法分析
//**
vector<string> split(const string& str, const string& delim) {//提取文件中的语素
vector<string> res;
	int n = str.length();
	int last = 0;
	for (int i = 0; i < n;  )
	{
		int index = str.find(delim,i);
		//res.push_back(str.substr(last,index))
		if (index == -1)
		{
			res.push_back(str.substr(last));
			break;
		}
		else
		{
			res.push_back(str.substr(last, index-last));
			last = index + delim.length();
			i = last;
		}
	}
	return res;
}

void Shift()
{
    for(int i=0;i<tokenlist.size();i++)
    {
        if(tokenlist[i].pos!=-3&&tokenlist[i].pos!=-4)
        {
            data.push_back(tokenlist[i].uplex);//把token加入到输入数据中
            row.push_back(tokenlist[i].line);
        }
    }
    data.pop_back();
    data.push_back("#");//添加终极符
}

void ReadChan()//读取产生式
{
ifstream file;
file.open("F:\\css.txt");
if(!file.is_open())
{
 printf("could not open css");
 exit(-1);
}
string str;
while(!file.eof())
{
    getline(file,str);
    string temp;
    int k;
    string temp1=str.substr(0,str.find("::="));
    Left.push_back(temp1);//::zuobiandefugei
    k=str.find("::=");
    if(k>0){
    string temp2=str.substr(k+3,str.length()-1);
    vector<string> temp3=split(temp2," ");
		  Right.push_back(temp3);
     }

  }
  Left.pop_back();
 // cout<<Left.size()<<"  "<<Right.size()<<endl;
  file.close();
}


void ReadPre()//读取predict集到
{
ifstream file;
file.open("F:\\predict.txt");
if(!file.is_open())
{
 printf("could not open predict");
 exit(-1);
}
string str;
int i=0;//predict集前面的序号
while(!file.eof())
{
 getline(file,str);
  vector<string> temp=split(str," ");
  Predicts[i]=temp;
  i++;
}
//cout<<Predicts.size()<<endl;
file.close();
}

void wenF(){
    for(int i=0;i<Left.size();i++)
      Vn.insert(Left[i]);
    for(int i=0;i<Right.size();i++)
      for(int j=0;j<Right[i].size();j++)
          if(!Vn.count(Right[i][j]))//所有出现在左部的符号都为非终极符
            Vt.insert(Right[i][j]);
      Vt.insert("#");
}

//利用predict集产生LL1分析表
void GenLL1(){
  set<string>::iterator it;
  set<string>::iterator it1;
  for(it=Vn.begin();it!=Vn.end();it++)
    for(it1=Vt.begin();it1!=Vt.end();it1++)
    LL1[*it][*it1]=-1;
    for(int i=0;i<Left.size();i++){
      vector<string>::iterator it;
      for(it=Predicts[i].begin();it!=Predicts[i].end();it++)
        LL1[Left[i]][*it]=i;
    }
}


//LL1驱动程序
int driver(){
  sem.push("#");
  sem.push(Left[0]);
  int top=0;
  while(data[top]!="#"||sem.top()!="#"){
    if(Vt.count(sem.top())&&data[top]==sem.top()){
      top++;//输入流向后移动移位
      sem.pop();//弹栈
    }
    else  if(Vt.count(sem.top())&&data[top]!=sem.top()){
      cout<<"第"<<row[top]<<"行"<<" "<<data[top]<<"处有语法错误"<<endl;
      return 0;
    }
    else if(Vn.count(sem.top())){
      int b;
      if((b=LL1[sem.top()][data[top]])!=-1){
        sem.pop();
        if(Right[b][0]!="e"){
          for(int i=Right[b].size()-1;i>=0;i--)
            sem.push(Right[b][i]);//将右部元素逆序插入
        }
      }
      else{
       cout<<"第"<<row[top]<<"行"<<" "<<data[top]<<"处有语法错误"<<endl;
        return 0;
      }
    }
  }
  cout<<"语法分析完成";
  return 1;

}

void cifa()//对词法分析进行的封装
{
    cout<<"词法分析结果："<<endl;
  for(int i=0;i<tokenlist.size();i++)
    {
        if(tokenlist[i].pos==-3)
        {
            printf("第%d行出现词法错误%s\n",tokenlist[i].line,tokenlist[i].uplex.c_str());
            error=1;//置符号位
        }
        if(tokenlist[i].pos==-1||tokenlist[i].pos==-2)
        {
            printf("<%d %s ->\n",tokenlist[i].line,tokenlist[i].uplex.c_str());
        }
        if(tokenlist[i].pos>=0)
        {
            printf("<%d %s %s>\n",tokenlist[i].line,tokenlist[i].uplex.c_str(),tokenlist[i].lex.c_str());
        }

    }

}

void yufa()//对语法操作进行的封装
{
    Shift();
    ReadChan();
    wenF();
    ReadPre();
    GenLL1();
    driver();
}

int main()
{
    FILE *fp;
    	if ((fp = fopen("F:\\test1.txt", "r")) == NULL)
	{
		printf("文件打开失败\n");
		exit(1);
	}

	token *temp=NULL;//临时token节点
	while((temp=scan(fp))!=NULL)
    {
        tokenlist.push_back(*temp);//这里的t为全局变量
        delete temp;
        temp=NULL;
    }
    fclose(fp);//关闭文件
    cifa();
    cout<<endl;
    if(!error)
    {
        yufa();
    }
    else
    {
        cout<<"词法分析出现错误后不进行语法分析"<<endl;
    }
//若词法分析已经出现问题，则不能进行语法分析
    return 0;
}


