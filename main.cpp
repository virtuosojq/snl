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
 int error=0;//��־λ���жϴʷ������Ƕ��Ǵ�

string rword[21]={ "begin","integer","char","program","array","of","record","end","var","procedure"
		,"if","then","else","fi","while","do","endwh","read","write","return","type" };
//�����ֱ�

struct token
{
    token(int a,string b,string d,int c)//û�й��캯���ᱨ��
    {
        this->line=a;
        this->lex=b;
        this->pos=c;
        this->uplex=d;
    }
    int line;//����
    string lex;//�ʷ���Ϣ
    string uplex;//��д�ʷ���Ϣ��־
    int pos;//������Ϣ
};//�����ֵ�posΪ-1,�ֽ����pos=-2,�����posΪ-3,ע��Ϊ-4

vector<token> tokenlist;//�������е�token����
vector<string> idTable;//��ʶ����
vector<string> intcTable;//������
 int line=1;//������ʼ��Ϊ1

 vector<string>data;//�������ݣ�ȫΪ�ռ���
 vector <int> row;//��ʾ�����������﷨�����еĴ�����ʾ
 stack<string>sem;//����ջ
 map< int,vector<string> >Predicts;//����predict��
 map< string,map<string,int> >LL1;//ll1��������

set<string> Vn,Vt;  //�����ռ����ͷ��ռ���
vector<string> Left;  //�������ʽ����
vector< vector<string> >Right;  //�������ʽ���Ҳ�
 //�ʷ�����
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
	string str = "";//��ʼΪ�մ�
	token * t = NULL;

	ch = fgetc(f);
    if(ch!=EOF)
    {
        while(ch==' '||ch == '\t'||ch=='\r'||ch=='\n')//�����ո�
        {
            if(ch=='\n')
            {
                line++;
            }
            ch=fgetc(f);
        }


        if(isLetter(ch))//��һ������ĸ
        {
         str+=ch;
         ch=fgetc(f);//����
         while(isLetter(ch)||isDigit(ch))
         {
             str+=ch;
             ch=fgetc(f);
         }
         if(isOther(ch))
         {
             ungetc(ch,f);//����
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
			||ch == ')' || ch == ';' || ch == '[' || ch == ']' || ch == '=')//���ֽ��
			//��ȥ.��:
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
                    printf("��%d�У�error  %s",line,str.c_str());
            }
        }

/*        if(ch=='\'')//������,��ֹ������
        {
            str+=ch;
            ch=fgetc(f);
            if(isLetter(ch))
            {
                ch=fgetc(f)
                    if(ch=='\'')
            }
        }*/
        if(ch=='{')//ע�ͻ�����ʱ������
            {
             ch=fgetc(f);
             while(ch!='}')
                    {
                        str+=ch;
                        ch=fgetc(f);
                    }
            // printf("ע�ͣ�%s",str.c_str());
             t=new token(line,str,str,-4);

             return t;
            }
            str+=ch;
           t=new token(line,str," ",-3);//������������һ���������ϣ����ش���

            return t;
        }
        else
        {
            return NULL;
        }
}
//�﷨����
//**
vector<string> split(const string& str, const string& delim) {//��ȡ�ļ��е�����
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
            data.push_back(tokenlist[i].uplex);//��token���뵽����������
            row.push_back(tokenlist[i].line);
        }
    }
    data.pop_back();
    data.push_back("#");//����ռ���
}

void ReadChan()//��ȡ����ʽ
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


void ReadPre()//��ȡpredict����
{
ifstream file;
file.open("F:\\predict.txt");
if(!file.is_open())
{
 printf("could not open predict");
 exit(-1);
}
string str;
int i=0;//predict��ǰ������
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
          if(!Vn.count(Right[i][j]))//���г������󲿵ķ��Ŷ�Ϊ���ռ���
            Vt.insert(Right[i][j]);
      Vt.insert("#");
}

//����predict������LL1������
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


//LL1��������
int driver(){
  sem.push("#");
  sem.push(Left[0]);
  int top=0;
  while(data[top]!="#"||sem.top()!="#"){
    if(Vt.count(sem.top())&&data[top]==sem.top()){
      top++;//����������ƶ���λ
      sem.pop();//��ջ
    }
    else  if(Vt.count(sem.top())&&data[top]!=sem.top()){
      cout<<"��"<<row[top]<<"��"<<" "<<data[top]<<"�����﷨����"<<endl;
      return 0;
    }
    else if(Vn.count(sem.top())){
      int b;
      if((b=LL1[sem.top()][data[top]])!=-1){
        sem.pop();
        if(Right[b][0]!="e"){
          for(int i=Right[b].size()-1;i>=0;i--)
            sem.push(Right[b][i]);//���Ҳ�Ԫ���������
        }
      }
      else{
       cout<<"��"<<row[top]<<"��"<<" "<<data[top]<<"�����﷨����"<<endl;
        return 0;
      }
    }
  }
  cout<<"�﷨�������";
  return 1;

}

void cifa()//�Դʷ��������еķ�װ
{
    cout<<"�ʷ����������"<<endl;
  for(int i=0;i<tokenlist.size();i++)
    {
        if(tokenlist[i].pos==-3)
        {
            printf("��%d�г��ִʷ�����%s\n",tokenlist[i].line,tokenlist[i].uplex.c_str());
            error=1;//�÷���λ
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

void yufa()//���﷨�������еķ�װ
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
		printf("�ļ���ʧ��\n");
		exit(1);
	}

	token *temp=NULL;//��ʱtoken�ڵ�
	while((temp=scan(fp))!=NULL)
    {
        tokenlist.push_back(*temp);//�����tΪȫ�ֱ���
        delete temp;
        temp=NULL;
    }
    fclose(fp);//�ر��ļ�
    cifa();
    cout<<endl;
    if(!error)
    {
        yufa();
    }
    else
    {
        cout<<"�ʷ��������ִ���󲻽����﷨����"<<endl;
    }
//���ʷ������Ѿ��������⣬���ܽ����﷨����
    return 0;
}


