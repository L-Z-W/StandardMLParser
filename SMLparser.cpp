#include<iostream>
#include<fstream>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define SCANNER_MAX_INPUT_SIZE 32768
//#define debug 1
using namespace std;

struct token{
	int	pro=0;//属性 
	char name[256];
}tok[2048];

//tokens pros
	//con
static const int t_char=1;
static const int t_str=2;
static const int t_word=3;
static const int t_float=4;
static const int t_int=5;
static const int t_int_or_lab=6;
	//identifiers
static const int t_id=8;//lab included 
static const int t_var=9;
static const int t_longid=10;
	//keyword
static const int t_kword=12;
	//symbol
static const int t_sym=13;
	//op 
static const int t_sinop=15; 
static const int t_binop=16;
static const int t_binop_nop=17;
static const int t_binop_low=18;
static const int t_binop_mid=19;
static const int t_binop_high=20; 



struct ASTNODE{
	int pro=0;//属性 
	char name[256];
	struct ASTNODE* sons[256]; 
}; 

unsigned int deep=0;//用来存AST树的深度 

token* curtok=&tok[0];//token[]的遍历指针 

string c[256];//用于比较的暂存字符串 

//ASTNODE pros
	//type
static const int ast_type=-1;
static const int ast_typrow=-2;
	//patterns
static const int ast_pat=-3;
static const int ast_patrow=-4;
	//expression
static const int ast_expression=-5;
static const int ast_exprow=-6;
static const int ast_match=-7;
	//declaration
static const int ast_declaration=-8;
static const int ast_valbind=-9;
static const int ast_funbind=-10;
static const int ast_funmatch=-11;
static const int ast_typbind=-12;
static const int ast_datbind=-13;
static const int ast_conbind=-14;
static const int ast_exnbind=-15;
	//structure
static const int ast_structure=-16;
static const int ast_strbind=-17;
	//signature
static const int ast_signature=-18;
static const int ast_typrefin=-19;
static const int ast_spec=-20;
static const int ast_valdesc=-21;
static const int ast_typdesc=-22;
static const int ast_datdesc=-23;
static const int ast_condesc=-24;
static const int ast_exndesc=-25;
static const int ast_strdesc=-26;
	//program
static const int ast_program=-27;
static const int ast_fctbind=-28;
static const int ast_sigbind=-29;






//底层函数
 
string* keycmp(char* name){//字符串比较 
	
	c[0]=name[0];
	for(int i=1;i<256;i++){
		if(name[i]=='\0')
		break;
		c[0]+=name[i];
	}
	return c; 
}


void err(const string b){//显示错误信息 

 	cout<<"在"<<curtok->name<<"处"<<" "<<b<<endl;
}
	

ASTNODE* newnode(int p)//新建节点并填入属性 
{
	ASTNODE* T;
	T=(ASTNODE*)malloc(sizeof(ASTNODE));
	T->pro=p;
	for(int i=0;i<256;i++)
	{
		T->sons[i]=NULL;
	}
	return T;
} 


void tokend(ASTNODE* node,token* endtoken)//终结符填充AST节点 
{
	int	i=0;
	node->pro=endtoken->pro;
	while(i<256)
	{
		node->name[i]=endtoken->name[i];
    	i++;
	}
	curtok++;
}


ASTNODE* toksonend(token* endtoken)//终结符添加到AST子节点 
{
	ASTNODE* node=newnode(endtoken->pro); 
	tokend(node,endtoken);
	return node;
}


void printast(ASTNODE* node)//打印AST 
{
	for(int i=0;i<=deep;i++)
	{
		cout<<"      ";
	}
	
	cout<<deep<<"属性 "<<node->pro<<" 名称$" <<node->name<<"$" <<endl;
	deep++;
	for(int j=0;j<256;j++)
	{
		if(node->sons[j]!=NULL)
		{	
			printast(node->sons[j]);
		}
	}
	deep--;
}




//解析函数声明 
//type
ASTNODE* parse_type();
ASTNODE* parse_typrow();
//partten
ASTNODE* parse_pattern();
ASTNODE*parse_patrow();
// expression
ASTNODE* parse_expression();
ASTNODE* parse_exprow();
ASTNODE* parse_match();
//declaration
ASTNODE* parse_declaration();
ASTNODE* parse_valbind();
ASTNODE* parse_funbind();
ASTNODE* parse_funmatch();
ASTNODE* parse_typbind();
ASTNODE* parse_datbind();
ASTNODE* parse_conbind();
ASTNODE* parse_exnbind();
//structure
ASTNODE* parse_structure();
ASTNODE* parse_strbind();
//signature
ASTNODE* parse_signature();
ASTNODE* parse_typrefin();
ASTNODE* parse_spec();
ASTNODE* parse_valdesc();
ASTNODE* parse_typdesc();
ASTNODE* parse_datdesc();
ASTNODE* parse_condesc();
ASTNODE* parse_exndesc();
ASTNODE* parse_strdesc();
//program
ASTNODE* parse_program();
ASTNODE* parse_fctbind();
ASTNODE* parse_sigbind();


//解析函数定义
 
ASTNODE* parse_typrow()// typrow 
{

	ASTNODE* T=newnode(ast_typrow);
	if((curtok->pro==t_int_or_lab||curtok->pro==t_id)&&*keycmp((curtok+1)->name)==":")
	{
		T->sons[0]=toksonend(curtok);
		T->sons[1]=toksonend(curtok);
		T->sons[2]=parse_type();

		if(*keycmp(curtok->name)==",")
		{
			T->sons[3]=toksonend(curtok);
			T->sons[4]=parse_typrow();
		}
		return T;
	}
	else
	{
		err("typrow error!");
		return NULL;
	}
	
} 

 
ASTNODE* parse_type()//type解析
{ 
	int i=0;
	ASTNODE* T=newnode(ast_type);
	
	switch(curtok->pro)
	{
		case t_var:{
			T->sons[0]=toksonend(curtok);
			break;
		}
		
		case t_sym:{
			if(*keycmp(curtok->name)=="(")//parentheses
			{
				
				T->sons[0]=toksonend(curtok);//eat (
				
				T->sons[1]=parse_type();
				
				if(*keycmp(curtok->name)==")")
				{
					T->sons[2]=toksonend(curtok);//eat )
				}
				else if(*keycmp(curtok->name)==",")//constructor
				{
					i=3;
					while(*keycmp(curtok->name)!=")"&&curtok->pro!=0&&*keycmp(curtok->name)==",")
					{
						T->sons[i]=toksonend(curtok);
						T->sons[i+1]=parse_type();
						i+=2;
					}
					
					if(*keycmp(curtok->name)==")")
					{
						T->sons[i]=toksonend(curtok);i++;//eat )
						
						if(curtok->pro==t_longid||curtok->pro==t_id)
						{
							T->sons[i]=toksonend(curtok);
						}
						else
						{
							err("expected longid");
							return NULL;
						}
					}
					else
					{
						err("expected \')\'");
						return NULL;
					}
				}
				
				else
				{
					err("expected \')\'");
					return NULL;
				}
			
			}
			else if(*keycmp(curtok->name)=="{")//record
			{
				T->sons[0]=toksonend(curtok);//eat {
				
				if(*keycmp(curtok->name)!="}")
				{
					T->sons[1]=parse_typrow();

					if(*keycmp(curtok->name)=="}")
					{
						T->sons[2]=toksonend(curtok);
					}
					else
					{	
						err("expected \'}\'");
						return NULL;
					}
				}
				else if(*keycmp(curtok->name)=="}")
				{
					T->sons[1]=toksonend(curtok);
				}
				else
				{
					err("expected \'}\'");
					return NULL;
				}
			}
			else
			{
				err("expected \'(\' or '\{\'");
				return NULL;
			}
			
			break;
		}
		
		case t_longid:{//constructor
			T->sons[0]=toksonend(curtok);
			break;
		}
		
		case t_id:{//constructor
			T->sons[0]=toksonend(curtok);
			break;
		}
		
		default:{
			err("type error!");
			return NULL; 
			break;
		}
		
	}
	i=0;
	
	
	if(*keycmp(curtok->name)=="->")//function
	{
		ASTNODE* FT=newnode(ast_type);
		FT->sons[0]=T;
		FT->sons[1]=toksonend(curtok);
		FT->sons[2]=parse_type();
		cout<<"parsed:type"<<endl;
		#ifdef debug
			printast(FT);
		#endif
		return FT;
	}
	else if(*keycmp(curtok->name)=="*")//tuple
	{
		i=0;
		ASTNODE* FT=newnode(ast_type);
		FT->sons[i]=T;i++;
		
		while(*keycmp(curtok->name)=="*")
		{
			FT->sons[i]=toksonend(curtok);
			FT->sons[i+1]=parse_type();
			i+=2;	
		}
		cout<<"parsed type"<<endl;
		#ifdef debug
			printast(FT);
		#endif
		return FT;
	}
	else if(curtok->pro==t_id||curtok->pro==t_longid)
	{
		ASTNODE* FT=newnode(ast_type);
		FT->sons[0]=T;
		FT->sons[1]=toksonend(curtok);
		if(curtok->pro==t_sym&&*keycmp(curtok->name)=="->")//function
		{
			ASTNODE* FFT=newnode(ast_type);
			FFT->sons[0]=FT;
			FFT->sons[1]=toksonend(curtok);
			FFT->sons[2]=parse_type();
			cout<<"parsed type"<<endl;
			#ifdef debug
				printast(FFT);
			#endif
			return FFT;
		}
		else if(*keycmp(curtok->name)=="*")//tuple
		{
			i=0;
			ASTNODE* FFT=newnode(ast_type);
			FFT->sons[i]=FT;i++;
		
			while(*keycmp(curtok->name)=="*")
			{
				FFT->sons[i]=toksonend(curtok);
				FFT->sons[i+1]=parse_type();
				i+=2;	
			}
			cout<<"parsed type"<<endl;
			#ifdef debug
				printast(FFT);
			#endif
			return FFT;
		}
		else
		{
			cout<<"parsed type"<<endl;
			#ifdef debug
				printast(FT);
			#endif
			return FT;	
		}
	}	
	else
	{
		cout<<"parsed:type"<<endl;
		#ifdef debug
			printast(T);
		#endif
		return T;
	}
	
	
	
}


ASTNODE*parse_patrow()//patrow 
{
	int i=0;
	ASTNODE* T=newnode(ast_patrow);
	if(*keycmp(curtok->name)=="...")//wildcard
	{
		T->sons[0]=toksonend(curtok);
	}
	else if(curtok->pro==t_id&&(*keycmp((curtok+1)->name)=="=")||curtok->pro==t_int_or_lab&&(*keycmp((curtok+1)->name)=="="))//pattern
	{
		T->sons[0]=toksonend(curtok);
		T->sons[1]=toksonend(curtok);
		T->sons[2]=parse_pattern();
		if(*keycmp(curtok->name)==",")
		{
			T->sons[3]=toksonend(curtok);
			T->sons[4]=parse_patrow();
		}
		return T;
	}
	else if(curtok->pro==t_id)//variable
	{
		T->sons[0]=toksonend(curtok);
		i=1;
		if(*keycmp(curtok->name)==":")
		{
			T->sons[i]=toksonend(curtok);
			T->sons[i+1]=parse_type();
			i+=2;
		}
		if(*keycmp(curtok->name)=="as")
		{
			T->sons[i]=toksonend(curtok);
			T->sons[i+1]=parse_pattern();
			i+=2;
		}
		if(*keycmp(curtok->name)==",")
		{
			T->sons[i]=toksonend(curtok);
			T->sons[i+1]=parse_patrow();
			i+=2;
		}
	}
	else
	{
		err("patrow error!");
	}
}


ASTNODE* parse_pattern()//pattern解析
{
	int i=0;
	ASTNODE* T=newnode(ast_pat);
	if((curtok->pro>0)&&(curtok->pro<8))//con
	{
		T->sons[0]=toksonend(curtok);
	}
	else if(curtok->pro>14&&((curtok+1)->pro==t_id||(curtok+1)->pro==t_longid))//op longid
	{
		T->sons[0]=toksonend(curtok);
		if(curtok->pro==t_id)
		{
			T->sons[1]=toksonend(curtok);//variable
			
			if(*keycmp(curtok->name)==":"||*keycmp(curtok->name)=="as")
			{
				i=2;
				if(*keycmp(curtok->name)==":")
				{
						T->sons[i]=toksonend(curtok);i++;
						T->sons[i]=parse_type();i++;

				}
				if(*keycmp(curtok->name)=="as")//layered
				{
					T->sons[i]=toksonend(curtok);i++;
					T->sons[i]=parse_pattern();i++;
				}
				i=0;
			}
			else if(*keycmp(curtok->name)=="("||*keycmp(curtok->name)=="["||*keycmp(curtok->name)=="{"||*keycmp(curtok->name)=="_"||
			((curtok->pro>0)&&(curtok->pro<8))||curtok->pro>14||curtok->pro==t_id||curtok->pro==t_longid)//constrction
			{
				T->sons[2]=parse_pattern();	
			}	
		}
		else if(curtok->pro==t_longid)//construction
		{
			T->sons[1]=toksonend(curtok);
			if(*keycmp(curtok->name)=="("||*keycmp(curtok->name)=="["||*keycmp(curtok->name)=="{"||*keycmp(curtok->name)=="_"||
			((curtok->pro>0)&&(curtok->pro<8))||curtok->pro>14||curtok->pro==t_id||curtok->pro==t_longid)
			{
				T->sons[2]=parse_pattern();	
			} 
		}
		else
		{
			err("expected id!");
			return NULL;
		}
	}
	else if(curtok->pro>14&&(*keycmp((curtok+1)->name)=="("||*keycmp((curtok+1)->name)=="["||*keycmp((curtok+1)->name)=="{"||*keycmp((curtok+1)->name)=="_"||
			(((curtok+1)->pro>0)&&((curtok+1)->pro<8))||(curtok+1)->pro>14||(curtok+1)->pro==t_id||(curtok+1)->pro==t_longid))
	{
		T->sons[0]=toksonend(curtok);
		T->sons[1]=parse_pattern();
	}
	else
	{
	
		switch(curtok->pro)
		{
			case t_sym:{
				if(*keycmp(curtok->name)=="(")
				{
					T->sons[0]=toksonend(curtok);
					T->sons[1]=parse_pattern();
					if(*keycmp(curtok->name)==")")//parentheses
					{
						T->sons[2]=toksonend(curtok);
					}
					else if(*keycmp(curtok->name)==",")//tuple
					{
						i=2;
						while(*keycmp(curtok->name)==","&&i<=253)
						{
							T->sons[i]=toksonend(curtok);
							T->sons[i+1]=parse_pattern();
							i+=2;
						}
						
						if(*keycmp(curtok->name)==")")
						{
							T->sons[i]=toksonend(curtok);
							i=0;
						}
						else
						{
							err("expected \')\'");
							return NULL;
						}
					}
					else
					{
						err("expected \')\'");
						return NULL;
					}
				}
				else if(*keycmp(curtok->name)=="[")//list
				{
					T->sons[0]=toksonend(curtok);
					
					if(*keycmp(curtok->name)=="]")
					{//empty
						T->sons[1]=toksonend(curtok);
					}
					else 
					{
						T->sons[1]=parse_pattern();
						if(*keycmp(curtok->name)=="]")
						{
							T->sons[2]=toksonend(curtok);
						}
						else if(*keycmp(curtok->name)==",")
						{
							i=2;
							while(*keycmp(curtok->name)==","&&i<=253)
							{
								T->sons[i]=toksonend(curtok);
								T->sons[i+1]=parse_pattern();
								i+=2;
							}
							
							if(*keycmp(curtok->name)=="]")
							{
								T->sons[i]=toksonend(curtok);
								i=0;
							}
							else
							{
								err("expected \']\'");
								return NULL;
							}
						}
						else
						{
							err("expected \']\'");
							return NULL;
						}
							
					}
					
				}
				else if(*keycmp(curtok->name)=="{")//record
				{
					T->sons[0]=toksonend(curtok);//eat {
					
					if(*keycmp(curtok->name)!="}")
					{
						T->sons[1]=parse_patrow();

						if(*keycmp(curtok->name)=="}")
						{
							T->sons[2]=toksonend(curtok);
						}
						else
						{	
							err("expected \'}\'");
							return NULL;
						}
					}
					else if(*keycmp(curtok->name)=="}")
					{
						T->sons[1]=toksonend(curtok);
					}
					else
					{
						err("expected \'}\'");
						return NULL;
					}
				}
				else if(*keycmp(curtok->name)=="_")//wildcard	
				{
					T->sons[0]=toksonend(curtok);
				}
				else
				{
					err("symbol mistake");
					return NULL;	
				}	
				break;
			} 
			
			case t_id:{
			
				T->sons[0]=toksonend(curtok);//variable
				if(*keycmp(curtok->name)==":"||*keycmp(curtok->name)=="as")
				{
					i=2;
					if(*keycmp(curtok->name)==":")
					{
							T->sons[i]=toksonend(curtok);i++;
							T->sons[i]=parse_type();i++;

					}
					if(*keycmp(curtok->name)=="as")//layered
					{
						T->sons[i]=toksonend(curtok);i++;
						T->sons[i]=parse_pattern();i++;
					}
					i=0;
				}
				else if(*keycmp(curtok->name)=="("||*keycmp(curtok->name)=="["||*keycmp(curtok->name)=="{"||*keycmp(curtok->name)=="_"||
				((curtok->pro>0)&&(curtok->pro<8))||curtok->pro>14||curtok->pro==t_id||curtok->pro==t_longid)//constrction
				{
					T->sons[2]=parse_pattern();	
				}
				break;
			} 
			
			case t_longid:{//construction
			
				T->sons[0]=toksonend(curtok);
				if(*keycmp(curtok->name)=="("||*keycmp(curtok->name)=="["||*keycmp(curtok->name)=="{"||*keycmp(curtok->name)=="_"||
				((curtok->pro>0)&&(curtok->pro<8))||curtok->pro>14||curtok->pro==t_id||curtok->pro==t_longid)
				{
					T->sons[1]=parse_pattern();	
				} 
				break;
			} 
			
			default:{
				err("pattern error!!");
				return NULL;
				break;
			}
		
		}
	}
	
	/*
	if(curtok->pro==t_id)//infix construction
	{
		ASTNODE* FT=newnode(ast_pat);
		FT->sons[0]=T;
		FT->sons[1]=toksonend(curtok);
		FT->sons[2]=parse_pattern();
		cout<<"parsed pattern"<<endl;
		#ifdef debug
			printast(FT);
		#endif
		return FT;
	}
	else */
	 if(*keycmp(curtok->name)==":")//type annotation
	{
		ASTNODE* FT=newnode(ast_pat);
		FT->sons[0]=T;
		FT->sons[1]=toksonend(curtok);
		FT->sons[2]=parse_type();
		cout<<"parsed pattern"<<endl;
		#ifdef debug
			printast(FT);
		#endif
		return FT;
	}
	else if(curtok->pro>14&&(*keycmp((curtok+1)->name)=="("||*keycmp((curtok+1)->name)=="["||*keycmp((curtok+1)->name)=="{"||*keycmp((curtok+1)->name)=="_"||
				(((curtok+1)->pro>0)&&((curtok+1)->pro<8))||(curtok+1)->pro>14||(curtok+1)->pro==t_id||(curtok+1)->pro==t_longid))
	{
		ASTNODE* FT=newnode(ast_pat);
		FT->sons[0]=T;
		FT->sons[1]=parse_pattern();
		#ifdef debug
			printast(FT);
		#endif
		return FT;
	}
	else if(*keycmp(curtok->name)=="("||*keycmp(curtok->name)=="["||*keycmp(curtok->name)=="{"||*keycmp(curtok->name)=="_"||
			((curtok->pro>0)&&(curtok->pro<8))||curtok->pro>14||curtok->pro==t_id||curtok->pro==t_longid)
	{
		ASTNODE* FT=newnode(ast_pat);
		FT->sons[0]=T;
		FT->sons[1]=parse_pattern();
		#ifdef debug
			printast(FT);
		#endif
		return FT;
	}
	else
	{
		cout<<"parsed pattern"<<endl;
		#ifdef debug
			printast(T);
		#endif
		return T;
	}
	
	
} 


ASTNODE* parse_exprow()//exprow 
{

	ASTNODE* T=newnode(ast_exprow);
	if((curtok->pro==t_int_or_lab||curtok->pro==t_id)&&*keycmp((curtok+1)->name)=="=")
	{
		T->sons[0]=toksonend(curtok);
		T->sons[1]=toksonend(curtok);
		T->sons[2]=parse_expression();
		
		if(*keycmp(curtok->name)==",")
		{
			T->sons[3]=toksonend(curtok);
			T->sons[4]=parse_exprow();
		}
		return T;

	}
	else
	{
		err("exprow error!");
		return NULL;
	}
	
}


ASTNODE* parse_match()//match 
{
	ASTNODE* T=newnode(ast_match);
	
	T->sons[0]=parse_pattern();
	if(*keycmp(curtok->name)=="=>")
	{
		T->sons[1]=toksonend(curtok);
		T->sons[2]=parse_expression();
		if(*keycmp(curtok->name)=="|")
		{
			T->sons[3]=toksonend(curtok);
			T->sons[4]=parse_match();
		}
		return T;
	}
	else
	{
		err("match error!");
		return NULL;
	}
	
}


ASTNODE* parse_expression()//expression 解析 
{
	int i=0;
	ASTNODE* T=newnode(ast_expression);
	if(curtok->pro>14&&(curtok+1)->pro==t_id||curtok->pro>14&&(curtok+1)->pro==t_longid)//op
	{
		T->sons[0]=toksonend(curtok);
		T->sons[1]=toksonend(curtok);
	}
	else if(curtok->pro>0&&curtok->pro<8)//con
	{
		T->sons[0]=toksonend(curtok);
	}
	else if(curtok->pro==t_id||curtok->pro==t_longid)//value or constructor identifier
	{
		T->sons[0]=toksonend(curtok);
	}
	else
	{
		switch(curtok->pro)
		{
			case t_sym:{
				if(*keycmp(curtok->name)=="#")//record selector
				{
					T->sons[0]=toksonend(curtok);
					if(curtok->pro==t_id||curtok->pro==t_int_or_lab)
					{
						T->sons[1]=toksonend(curtok);
					}
					else
					{
						err("expected lab");
						return NULL;
					}
					
				}
				else if(*keycmp(curtok->name)=="(")
				{
					T->sons[0]=toksonend(curtok);
					T->sons[1]=parse_expression();
					if(*keycmp(curtok->name)==")")//parentheses
					{
						T->sons[2]=toksonend(curtok);
					}
					else if(*keycmp(curtok->name)==",")//tuple
					{
						i=2;
						while(*keycmp(curtok->name)==",")
						{
							T->sons[i]=toksonend(curtok);
							T->sons[i+1]=parse_expression();
							i+=2;
						}
						
						if(*keycmp(curtok->name)==")")
						{
							T->sons[i]=toksonend(curtok);
							i=0;
						}
						else
						{
							err("expected \')\'");
							return NULL;
						}
					}
					else if(*keycmp(curtok->name)==";")//sequence
					{
						i=2;
						while(*keycmp(curtok->name)==";")
						{
							T->sons[i]=toksonend(curtok);
							T->sons[i+1]=parse_expression();
							i+=2;
						}
						
						if(*keycmp(curtok->name)==")")
						{
							T->sons[i]=toksonend(curtok);
							i=0;
						}
						else
						{
							err("expected \')\'");
							return NULL;
						}
					}
					else
					{
						err("expected \')\'");
						return NULL;
					}
				}
				else if(*keycmp(curtok->name)=="[")//list
				{
					T->sons[0]=toksonend(curtok);
					
					if(*keycmp(curtok->name)=="]")
					{//empty
						T->sons[1]=toksonend(curtok);
					}
					else 
					{
						T->sons[1]=parse_expression();
						if(*keycmp(curtok->name)=="]")
						{
							T->sons[2]=toksonend(curtok);
						}
						else if(*keycmp(curtok->name)==",")
						{
							i=2;
							while(*keycmp(curtok->name)==",")
							{
								T->sons[i]=toksonend(curtok);
								T->sons[i+1]=parse_expression();
								i+=2;
							}
							
							if(*keycmp(curtok->name)=="]")
							{
								T->sons[i]=toksonend(curtok);
								i=0;
							}
							else
							{
								err("expected \']\'");
								return NULL;
							}
						}
						else
						{
							err("expected \']\'");
							return NULL;
						}
							
					}
					
				}
				else if(*keycmp(curtok->name)=="{")//record
				{
					T->sons[0]=toksonend(curtok);//eat {
				
					if(*keycmp(curtok->name)!="}")
					{
						T->sons[1]=parse_exprow();

						if(*keycmp(curtok->name)=="}")
						{
							T->sons[2]=toksonend(curtok);
						}
						else
						{	
							err("expected \'}\'");
							return NULL;
						}
					}
					else if(*keycmp(curtok->name)=="}")
					{
						T->sons[1]=toksonend(curtok);
					}
					else
					{
						err("expected \'}\'");
						return NULL;
					}
				}
				else
				{
					err("symbol mistake!!"); 
					return NULL;
				}
				break;
			}
			
			case t_kword:{
				if(*keycmp(curtok->name)=="if")//conditional
				{
					
					T->sons[0]=toksonend(curtok);
					T->sons[1]=parse_expression();
					if(*keycmp(curtok->name)=="then")
					{
						T->sons[2]=toksonend(curtok);
						T->sons[3]=parse_expression();
						if(*keycmp(curtok->name)=="else")
						{
							T->sons[4]=toksonend(curtok);
							T->sons[5]=parse_expression();
						}
						else
						{
							err("expected \'else\'");
							return NULL;
						}
					}
					else
					{
						err("expected \'then\'");
						return NULL;
					}
				}
				else if(*keycmp(curtok->name)=="raise")//exception raising
				{
					T->sons[0]=toksonend(curtok);
					T->sons[1]=parse_expression();
				}
				else if(*keycmp(curtok->name)=="fn")//function
				{
					T->sons[0]=toksonend(curtok);
					T->sons[1]=parse_match();
				}
				else if(*keycmp(curtok->name)=="while")//iteration
				{
					T->sons[0]=toksonend(curtok);
					T->sons[1]=parse_expression();
					if(*keycmp(curtok->name)=="do")
					{
						T->sons[2]=toksonend(curtok);
						T->sons[3]=parse_expression();
					}
					else
					{
						err("expected \'do\'");
						return NULL;
					}
				}
				else if(*keycmp(curtok->name)=="case")//case analysis
				{
					T->sons[0]=toksonend(curtok);
					T->sons[1]=parse_expression();
					if(*keycmp(curtok->name)=="of")
					{
						T->sons[2]=toksonend(curtok);
						T->sons[3]=parse_match();
					}
					else
					{
						err("expected \'of\'");
						return NULL;
					}
				}
				else if(*keycmp(curtok->name)=="let")//local declaration
				{
					T->sons[0]=toksonend(curtok);
					T->sons[1]=parse_declaration();
					if(*keycmp(curtok->name)=="in")
					{
						T->sons[2]=toksonend(curtok);
						T->sons[3]=parse_expression();
						i=4;
						if(*keycmp(curtok->name)==";")
						{
							while(*keycmp(curtok->name)==";")
							{
								T->sons[i]=toksonend(curtok);
								T->sons[i+1]=parse_expression();
								i+=2;
							}
						}
						if(*keycmp(curtok->name)=="end")
						{
							T->sons[i]=toksonend(curtok);
							i=0;
						}
						else
						{
							err("expected \'end\'");
							return NULL;
						}
						
					}
					else
					{
						err("expected \'in\'");
						return NULL;
					}
				}
				else
				{
					err("keyword mistake!");
					return NULL;
				}
				break;
			}
			default:{
				err("expression error!");
				return NULL;
				break;
			}
		}
	}
	
	
	if((curtok->pro==t_id||curtok->pro>14)&&(*keycmp((curtok+1)->name)=="#"||*keycmp((curtok+1)->name)=="("||*keycmp((curtok+1)->name)=="{"||*keycmp((curtok+1)->name)=="["||
			//*keycmp((curtok+1)->name)=="if"||*keycmp((curtok+1)->name)=="fn"||*keycmp((curtok+1)->name)=="let"||*keycmp((curtok+1)->name)=="case"||*keycmp((curtok+1)->name)=="let"||
			//*keycmp((curtok+1)->name)=="raise"||*keycmp((curtok+1)->name)=="while"||*keycmp((curtok+1)->name)=="#"||
			((curtok+1)->pro>14&&((curtok+1)+1)->pro==t_id||(curtok+1)->pro>14&&((curtok+1)+1)->pro==t_longid)||(curtok+1)->pro>0&&(curtok+1)->pro<8||((curtok+1)->pro==t_id||(curtok+1)->pro==t_longid)))//infix application
	{
		ASTNODE* FT=newnode(ast_expression);
		FT->sons[0]=T;
		FT->sons[1]=toksonend(curtok);
		FT->sons[2]=parse_expression();
		cout<<"parsed expression"<<endl;
		#ifdef debug
			printast(FT);
		#endif
		return FT;
		
	}
	else if(curtok->pro==t_kword&&*keycmp(curtok->name)=="handle")//exception handling
	{
		ASTNODE* FT=newnode(ast_expression);
			FT->sons[0]=T;
			FT->sons[1]=toksonend(curtok);
			FT->sons[2]=parse_match();
			cout<<"parsed expression"<<endl;
			#ifdef debug
			printast(FT);
			#endif
			return FT;
	}
	else if(curtok->pro==t_kword&&*keycmp(curtok->name)=="andalso")//conjunction
	{
		ASTNODE* FT=newnode(ast_expression);
			FT->sons[0]=T;
			FT->sons[1]=toksonend(curtok);
			FT->sons[2]=parse_expression();
			cout<<"parsed expression"<<endl;
			#ifdef debug
			printast(FT);
			#endif
			return FT;
	}
	else if(curtok->pro==t_kword&&*keycmp(curtok->name)=="orelse")//disjunction
	{
		ASTNODE* FT=newnode(ast_expression);
			FT->sons[0]=T;
			FT->sons[1]=toksonend(curtok);
			FT->sons[2]=parse_expression();
			cout<<"parsed expression"<<endl;
			#ifdef debug
			printast(FT);
			#endif
			return FT;
	}
	else if(*keycmp(curtok->name)==":")//type annotation
	{
		ASTNODE* FT=newnode(ast_expression);
		FT->sons[0]=T;
		FT->sons[1]=toksonend(curtok);
		FT->sons[2]=parse_type();
		cout<<"parsed expression"<<endl;
		#ifdef debug
			printast(FT);
		#endif
		return FT;
	}
	else if(*keycmp(curtok->name)=="#"||*keycmp(curtok->name)=="("||*keycmp(curtok->name)=="{"||*keycmp(curtok->name)=="["||*keycmp(curtok->name)=="="||
			//*keycmp(curtok->name)=="if"||*keycmp(curtok->name)=="fn"||*keycmp(curtok->name)=="let"||*keycmp(curtok->name)=="case"||*keycmp(curtok->name)=="let"||
			//*keycmp(curtok->name)=="raise"||*keycmp(curtok->name)=="while"||*keycmp(curtok->name)=="#"||
			(curtok->pro>14&&(curtok+1)->pro==t_id||curtok->pro>14&&(curtok+1)->pro==t_longid)||curtok->pro>0&&curtok->pro<8||(curtok->pro==t_id||curtok->pro==t_longid))//application
	{
		ASTNODE* FT=newnode(ast_expression);
		FT->sons[0]=T;
		if(*keycmp(curtok->name)=="=")
		{
			FT->sons[1]=toksonend(curtok);
			FT->sons[2]=parse_expression();
		}
		else
		{
			FT->sons[1]=parse_expression();
		}
		
		cout<<"parsed expression"<<endl;
		
		#ifdef debug
			printast(FT);
		#endif
		return FT;
	}
	else{
		
		cout<<"parsed expression"<<endl;
		#ifdef debug
			printast(T);
		#endif
		return T;
	}
	
} 


ASTNODE* parse_declaration()//declaration 
{
	int i=0;
	ASTNODE* T=newnode(ast_declaration);
	
		if(*keycmp(curtok->name)=="val")//value
		{
			T->sons[0]=toksonend(curtok); 
			i=1;
			if(curtok->pro==t_var)
			{
				T->sons[i]=toksonend(curtok);i++;	
			} 
			else if(*keycmp(curtok->name)=="("&&(curtok+1)->pro==t_var)
			{
				T->sons[i]=toksonend(curtok);i++;
				T->sons[i]=toksonend(curtok);i++;
				if(*keycmp(curtok->name)==")")
				{
					T->sons[i]=toksonend(curtok);i++;//eat )
				}
				else if(*keycmp(curtok->name)==",")//constructor
				{
					while(*keycmp(curtok->name)==","&&(curtok+1)->pro==t_var)
					{
						T->sons[i]=toksonend(curtok);
						T->sons[i+1]=toksonend(curtok);
						i+=2;
					}
					
					if(*keycmp(curtok->name)==")")
					{
						T->sons[i]=toksonend(curtok);i++;//eat )			
					}
					else
					{
						err("expected \')\'");
						return NULL;
					}
				}
				else
				{
					err("expected \')\'");
					return NULL;
				}
			}
			
			
			T->sons[i]=parse_valbind();
			
		}
		else if(*keycmp(curtok->name)=="fun")//function
		{
			T->sons[0]=toksonend(curtok); 
			i=1;
			if(curtok->pro==t_var)
			{
				T->sons[i]=toksonend(curtok);i++;	
			} 
			else if(*keycmp(curtok->name)=="("&&(curtok+1)->pro==t_var)
			{
				T->sons[i]=toksonend(curtok);i++;
				T->sons[i]=toksonend(curtok);i++;
				if(*keycmp(curtok->name)==")")
				{
					T->sons[i]=toksonend(curtok);i++;//eat )
				}
				else if(*keycmp(curtok->name)==",")//constructor
				{
					while(*keycmp(curtok->name)==","&&(curtok+1)->pro==t_var)
					{
						T->sons[i]=toksonend(curtok);
						T->sons[i+1]=toksonend(curtok);
						i+=2;
					}
					
					if(*keycmp(curtok->name)==")")
					{
						T->sons[i]=toksonend(curtok);i++;//eat )
					}
					else
					{
						err("expected \')\'");
						return NULL;
					}
				}
				else
				{
					err("expected \')\'");
					return NULL;
				}
			}
			
			T->sons[i]=parse_funbind();			
		}
		else if(*keycmp(curtok->name)=="type")//type
		{
			T->sons[0]=toksonend(curtok);
			T->sons[1]=parse_typbind();
		}
		else if(*keycmp(curtok->name)=="datatype")
		{
			T->sons[0]=toksonend(curtok);
			if(curtok->pro==t_id&&*keycmp((curtok+1)->name)=="="&&*keycmp((curtok+2)->name)=="datatype")//data type replication
			{
				T->sons[1]=toksonend(curtok);
				T->sons[2]=toksonend(curtok);
				T->sons[3]=toksonend(curtok);
				if(curtok->pro==t_id||curtok->pro==t_longid)
				{
					T->sons[4]=toksonend(curtok);
				}
				else
				{
					err("expected longid");
					return NULL;
				}
			}
			else
			{
				T->sons[1]=parse_datbind();//data type
				if(*keycmp(curtok->name)=="withtype")
				{
					T->sons[2]=parse_typbind();
				}
			}
		}
		else if(*keycmp(curtok->name)=="abstype")//abstract type
		{
			i=0;
			T->sons[i]=toksonend(curtok);i++;
			T->sons[i]=parse_datbind();i++;
			
			if(*keycmp(curtok->name)=="withtype")
			{
				T->sons[i]=toksonend(curtok);i++;
				T->sons[i]=parse_typbind();i++;
			}
			
			if(*keycmp(curtok->name)=="with")
			{
				T->sons[i]=toksonend(curtok);i++;
				T->sons[i]=parse_declaration();i++;
			}
			else
			{
				err("expected \'with\'");
				return NULL;
			}
			
			if(*keycmp(curtok->name)=="end")
			{
				T->sons[i]=toksonend(curtok);i++;
			}
			else
			{
				err("expected \'with\'");
				return NULL;
			}
 
		}
		else if(*keycmp(curtok->name)=="exception")//exception
		{
			T->sons[0]=toksonend(curtok);
			T->sons[1]=parse_exnbind();
		}
		else if(*keycmp(curtok->name)=="structure")//structure (not allowed inside expressions)
		{
			T->sons[0]=toksonend(curtok);
			T->sons[1]=parse_strbind();
		}
		else if(*keycmp(curtok->name)=="local")//local
		{
			i=0;
			T->sons[i]=toksonend(curtok);i++;
			
			
			T->sons[i]=parse_declaration();i++;
			
			
			if(*keycmp(curtok->name)=="in")
			{
				T->sons[i]=toksonend(curtok);i++;
			}
			else
			{
				err("expected \'in\'");
				return NULL;
			}
			
			T->sons[i]=parse_declaration();i++;
			
			if(*keycmp(curtok->name)=="end")
			{
				T->sons[i]=toksonend(curtok);i++;
			}
			else
			{
				err("expected \'end\'");
				return NULL;
			}
		}
		else if(*keycmp(curtok->name)=="open")//inclusion (n ≥ 1)
		{
			i=0;
			T->sons[i]=toksonend(curtok);i++;
			if(curtok->pro==t_id||curtok->pro==t_longid)
			{
				while(curtok->pro==t_id||curtok->pro==t_longid)
				{
					T->sons[i]=toksonend(curtok);i++;
				}
			} 
			else
			{
				err("expected longid");
				return NULL;
			}
		}
		else if(*keycmp(curtok->name)=="nonfix")//nonfix (n ≥ 1)
		{
			i=0;
			T->sons[i]=toksonend(curtok);i++;
			if(curtok->pro==t_id)
			{
				while(curtok->pro==t_id)
				{
					T->sons[i]=toksonend(curtok);i++;
				}
			}
			else
			{
				err("expected id");
				return NULL;
			} 
		}
		else if(*keycmp(curtok->name)=="infix")//left-associative infix (n ≥ 1)
		{
			i=0;
			T->sons[i]=toksonend(curtok);i++;
			if(curtok->pro==t_int_or_lab&&curtok->name[2]=='\0')
			{
				T->sons[i]=toksonend(curtok);i++;
			}
			
			if(curtok->pro==t_id)
			{
				while(curtok->pro==t_id)
				{
					T->sons[i]=toksonend(curtok);i++;
				}
			}
			else
			{
				err("expected id");
				return NULL;
			} 
		}
		else if(*keycmp(curtok->name)=="infixr")//right-associative infix (n ≥ 1)
		{
			i=0;
			T->sons[i]=toksonend(curtok);i++;
			if(curtok->pro==t_int_or_lab&&curtok->name[2]=='\0')
			{
				T->sons[i]=toksonend(curtok);i++;
			}
			if(curtok->pro==t_id)
			{
				while(curtok->pro==t_id)
				{
					T->sons[i]=toksonend(curtok);i++;
				}
			}
			else
			{
				err("expected id");
				return NULL;
			} 
		}
		else
		{
			err("dec keyword mistake!");
			return NULL;
		}
	
	
	
	
	if(*keycmp(curtok->name)==";"||*keycmp(curtok->name)=="val"||*keycmp(curtok->name)=="fun"||*keycmp(curtok->name)=="type"||*keycmp(curtok->name)=="datatype"||*keycmp(curtok->name)=="abstype"||
		*keycmp(curtok->name)=="exception"||*keycmp(curtok->name)=="structure"||*keycmp(curtok->name)=="local"||*keycmp(curtok->name)=="open"||
		*keycmp(curtok->name)=="nonfix"||*keycmp(curtok->name)=="infix"||*keycmp(curtok->name)=="infixr")//sequence
	{
		ASTNODE* FT=newnode(ast_declaration);
		FT->sons[0]=T;
		if(*keycmp(curtok->name)==";")
		{
			FT->sons[1]=toksonend(curtok);
			FT->sons[2]=parse_declaration();
		}
		else
		{
			FT->sons[1]=parse_declaration();
		}
		
		cout<<"parsed declaration"<<endl;
		#ifdef debug
			printast(FT);
		#endif
		return FT;
	}
	else
	{
		cout<<"parsed declaration"<<endl;
		#ifdef debug
			printast(T);
		#endif
		return T;
	}
}


ASTNODE* parse_valbind()
{
	ASTNODE* T=newnode(ast_valbind);
	if(*keycmp(curtok->name)=="rec")//recursive
	{
		T->sons[0]=toksonend(curtok);
		T->sons[1]=parse_valbind();
	}
	else if(*keycmp(curtok->name)=="("||*keycmp(curtok->name)=="["||*keycmp(curtok->name)=="{"||*keycmp(curtok->name)=="_"||
		((curtok->pro>0)&&(curtok->pro<8))||curtok->pro>14||curtok->pro==t_id||curtok->pro==t_longid)//	destructuring
	{
		T->sons[0]=parse_pattern();
		if(*keycmp(curtok->name)=="=")
		{
			T->sons[1]=toksonend(curtok);
			T->sons[2]=parse_expression();
			if(*keycmp(curtok->name)=="and")
			{
				T->sons[3]=toksonend(curtok);
				T->sons[4]=parse_valbind();
			}			
		} 
		else
		{
			err("expected \'=\'");
			return NULL;
		}
	}
	else
	{
		err("valbind error!");
		return NULL;
	}
	return T;
}


ASTNODE* parse_funbind()
{
	ASTNODE* T=newnode(ast_funbind);
	if(*keycmp(curtok->name)=="("||*keycmp(curtok->name)=="["||*keycmp(curtok->name)=="{"||*keycmp(curtok->name)=="_"||
		((curtok->pro>0)&&(curtok->pro<8))||curtok->pro>14||curtok->pro==t_id||curtok->pro==t_longid)//clausal function
	{
		T->sons[0]=parse_funmatch();
		if(*keycmp(curtok->name)=="and")
		{
			T->sons[1]=toksonend(curtok);
			T->sons[2]=parse_funbind();
		}
	}
	else
	{
		err("funbind error!");
		return NULL;
	}
	return T;
}


ASTNODE* parse_funmatch()
{
	int i=0;
	ASTNODE* T=newnode(ast_funmatch);
	
	if(curtok->pro>14||curtok->pro==t_id)//nonfix (n ≥ 1)
	{
		i=0;
		if(curtok->pro==t_id)
		{
			T->sons[i]=toksonend(curtok);i++;
		}
		else
		{
			T->sons[i]=toksonend(curtok);i++;
			T->sons[i]=toksonend(curtok);i++;
		}
		if(*keycmp(curtok->name)=="("||*keycmp(curtok->name)=="["||*keycmp(curtok->name)=="{"||*keycmp(curtok->name)=="_"||
			((curtok->pro>0)&&(curtok->pro<8))||curtok->pro>14||curtok->pro==t_id||curtok->pro==t_longid)
		{
			while(!(*keycmp(curtok->name)==":"||*keycmp(curtok->name)=="=")&&i<=252)
			{
				T->sons[i]=parse_pattern();i++;
			}
		}
		
		
		if(*keycmp(curtok->name)==":")
		{
			T->sons[i]=toksonend(curtok);i++;
			T->sons[i]=parse_type();i++;
		}
		
		if(*keycmp(curtok->name)=="=")
		{
			T->sons[i]=toksonend(curtok);i++;
			T->sons[i]=parse_expression();i++;
			
		}
		else
		{
			err("expected \'=\'");
			return NULL;
		}
		
		if(*keycmp(curtok->name)=="|")
		{
			T->sons[i]=toksonend(curtok);i++;
			T->sons[i]=parse_funmatch();i++;
		}
		
	}
	else if(*keycmp(curtok->name)=="("&&(*keycmp((curtok+1)->name)=="("||*keycmp((curtok+1)->name)=="["||*keycmp((curtok+1)->name)=="{"||*keycmp((curtok+1)->name)=="_"||
		(((curtok+1)->pro>0)&&((curtok+1)->pro<8))||(curtok+1)->pro>14||(curtok+1)->pro==t_id||(curtok+1)->pro==t_longid))//infix (n ≥ 0)
	{
		i=0;
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=parse_pattern();i++;
		if(curtok->pro==t_id)
		{
			T->sons[i]=toksonend(curtok);i++;
			T->sons[i]=parse_pattern();i++;	
		}
		else
		{
			err("expected id");
			return NULL;
		}
		
		if(*keycmp(curtok->name)==")")
		{
			T->sons[i]=toksonend(curtok);i++;
		}
		else
		{
			err("expected \')\'");
			return NULL;
		}
		
		while(*keycmp(curtok->name)=="("||*keycmp(curtok->name)=="["||*keycmp(curtok->name)=="{"||*keycmp(curtok->name)=="_"||
		((curtok->pro>0)&&(curtok->pro<8))||/*curtok->pro>14||curtok->pro==t_id||*/curtok->pro==t_longid)
		{
			T->sons[i]=parse_pattern();i++;
		}
		
		if(*keycmp(curtok->name)==":")
		{
			T->sons[i]=toksonend(curtok);i++;
			T->sons[i]=parse_type();i++;
		}
		
		if(*keycmp(curtok->name)=="=")
		{
			T->sons[i]=toksonend(curtok);i++;
			T->sons[i]=parse_expression();i++;
			if(*keycmp(curtok->name)=="|")
			{
				T->sons[i]=toksonend(curtok);i++;
				T->sons[i]=parse_funmatch();i++;
			}
		}
		else
		{
			err("expected \'=\'");
			return NULL;
		}
		
	}
	else if(*keycmp(curtok->name)=="("||*keycmp(curtok->name)=="["||*keycmp(curtok->name)=="{"||*keycmp(curtok->name)=="_"||
		((curtok->pro>0)&&(curtok->pro<8))||curtok->pro>14||curtok->pro==t_id||curtok->pro==t_longid)//infix
	{
		i=0;
		T->sons[i]=parse_pattern();i++;
		
		if(curtok->pro==t_id)
		{
			T->sons[i]=toksonend(curtok);i++;
		}
		else
		{
			err("expected id");
			return NULL;
		}
		
		T->sons[i]=parse_pattern();i++;
		
		
		
		if(*keycmp(curtok->name)==":")
		{
			T->sons[i]=toksonend(curtok);i++;
			T->sons[i]=parse_type();i++;
		}
		
		if(*keycmp(curtok->name)=="=")
		{
			T->sons[i]=toksonend(curtok);i++;
			T->sons[i]=parse_expression();i++;
			if(*keycmp(curtok->name)=="|")
			{
				T->sons[i]=toksonend(curtok);i++;
				T->sons[i]=parse_funmatch();i++;
			}
		}
		else
		{
			err("expected \'=\'");
			return NULL;
		}
		
	}
	else
	{
		err("funmatch error!");
		return NULL;
	}
	return T; 
}


ASTNODE* parse_typbind()
{
	int i=0;
	ASTNODE* T=newnode(ast_typbind);
	
	if(curtok->pro==t_id) //abbreviation
	{
		T->sons[i]=toksonend(curtok);i++;
	}
	else if(curtok->pro==t_var&&(curtok+1)->pro==t_id)
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=toksonend(curtok);i++;	
	} 
	else if(*keycmp(curtok->name)=="("&&(curtok+1)->pro==t_var)
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=toksonend(curtok);i++;
		if(*keycmp(curtok->name)==")")
		{
			T->sons[i]=toksonend(curtok);i++;//eat )
			if(curtok->pro==t_id)
			{
				T->sons[i]=toksonend(curtok);i++;//eat id	
			}
			else
			{
				err("expected id");
				return NULL;
			}
		}
		else if(*keycmp(curtok->name)==",")//constructor
		{
			while(*keycmp(curtok->name)==","&&(curtok+1)->pro==t_var)
			{
				T->sons[i]=toksonend(curtok);
				T->sons[i+1]=toksonend(curtok);
				i+=2;
			}			
			
			if(*keycmp(curtok->name)==")")
			{
				T->sons[i]=toksonend(curtok);i++;//eat )
				if(curtok->pro==t_id)
				{
					T->sons[i]=toksonend(curtok);i++;//eat id	
				}
				else
				{
					err("expected id");
					return NULL;
				}					
			}
			else
			{
				err("expected \')\'");
				return NULL;
			}
		}
		else
		{
			err("expected \')\'");
			return NULL;
		}
	}
	else
	{
		err("typbind error!");
		return NULL;
	}
	
	if(*keycmp(curtok->name)=="=")
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=parse_type();i++;
	}
	else
	{
		err("expected \'=\'");
		return NULL;
	}
	

	if(*keycmp(curtok->name)=="and")
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=parse_typbind();i++;
	}
	
	return T;
}


ASTNODE* parse_datbind()
{
	int i=0;
	ASTNODE* T=newnode(ast_datbind);//data type
	
	if(curtok->pro==t_id) 
	{
		T->sons[i]=toksonend(curtok);i++;
	}
	else if(curtok->pro==t_var&&(curtok+1)->pro==t_id)
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=toksonend(curtok);i++;	
	} 
	else if(*keycmp(curtok->name)=="("&&(curtok+1)->pro==t_var)
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=toksonend(curtok);i++;
		if(*keycmp(curtok->name)==")")
		{
			T->sons[i]=toksonend(curtok);i++;//eat )
			if(curtok->pro==t_id)
			{
				T->sons[i]=toksonend(curtok);i++;//eat id	
			}
			else
			{
				err("expected id");
				return NULL;
			}
		}
		else if(*keycmp(curtok->name)==",")//constructor
		{
			while(*keycmp(curtok->name)==","&&(curtok+1)->pro==t_var)
			{
				T->sons[i]=toksonend(curtok);
				T->sons[i+1]=toksonend(curtok);
				i+=2;
			}			
			
			if(*keycmp(curtok->name)==")")
			{
				T->sons[i]=toksonend(curtok);i++;//eat )
				if(curtok->pro==t_id)
				{
					T->sons[i]=toksonend(curtok);i++;//eat id	
				}
				else
				{
					err("expected id");
					return NULL;
				}					
			}
			else
			{
				err("expected \')\'");
				return NULL;
			}
		}
		else
		{
			err("expected \')\'");
			return NULL;
		}
	}
	else
	{
		err("typbind error!");
		return NULL;
	}
	
	if(*keycmp(curtok->name)=="=")
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=parse_conbind();i++;
	}
	else
	{
		err("expected \'=\'");
		return NULL;
	}
	

	if(*keycmp(curtok->name)=="and")
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=parse_datbind();i++;
	}
	
	return T;
}


ASTNODE* parse_conbind()
{
	int i=0;
	ASTNODE* T=newnode(ast_conbind);
	
	if(curtok->pro==t_id)//data constructor
	{
		T->sons[i]=toksonend(curtok);i++;
	}
	else
	{
		err("conbind error!");
		return NULL;
	}
	
	if(*keycmp(curtok->name)=="of")
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=parse_type();i++;
	}
	
	if(*keycmp(curtok->name)=="|")
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=parse_conbind();i++;
	}
	
	return T;
}


ASTNODE* parse_exnbind()
{
	int i=0;
	ASTNODE* T=newnode(ast_exnbind);
	
	if(curtok->pro==t_id)
	{
		T->sons[i]=toksonend(curtok);i++;
	}
	else
	{
		err("exnbind error!");
		return NULL;
	}
	
	if(*keycmp(curtok->name)=="="&&((curtok+1)->pro==t_id||(curtok+1)->pro==t_longid))//	renaming
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=toksonend(curtok);i++;
	}
	else//	generative
	{
		if(*keycmp(curtok->name)=="of")
		{
			T->sons[i]=toksonend(curtok);i++;
			T->sons[i]=parse_type();i++;
		}
	}
	
	if(*keycmp(curtok->name)=="and")
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=parse_exnbind();i++;
	}
	
	return T;
}


ASTNODE* parse_structure()//structure解析 
{
	int i=0;
	ASTNODE* T=newnode(ast_structure);
	
	if(curtok->pro==t_id&&*keycmp((curtok+1)->name)=="(")//	functor application
	{
		i=0;
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=toksonend(curtok);i++;
		
		if(curtok->pro==t_id||curtok->pro==t_longid||*keycmp(curtok->name)=="struct"||*keycmp(curtok->name)=="let")
		{
			T->sons[i]=parse_structure();i++;
		}
		else
		{
			T->sons[i]=parse_declaration();i++;
		}
		
		if(*keycmp(curtok->name)==")")
		{
			T->sons[i]=toksonend(curtok);i++;
		}
		else
		{
			err("expected \')\'");
			return NULL;
		}
		
	}
	else if(*keycmp(curtok->name)=="struct")//	structure
	{
		i=0;
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=parse_declaration();i++;
		if(*keycmp(curtok->name)=="end")
		{
			T->sons[i]=toksonend(curtok);i++;
		}
		else
		{
			err("expected \'end\'");
			return NULL;
		}
	}
	else if(*keycmp(curtok->name)=="let")//local declaration
	{
		i=0;
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=parse_declaration();i++;
		
		if(*keycmp(curtok->name)=="in")
		{
			T->sons[i]=toksonend(curtok);i++;
		}
		else
		{
			err("expected \'in\'");
			return NULL;
		}
		
		T->sons[i]=parse_structure();i++;
		
		if(*keycmp(curtok->name)=="end")
		{
			T->sons[i]=toksonend(curtok);i++;
		}
		else
		{
			err("expected \'end\'");
			return NULL;
		}
		
	}
	else if(curtok->pro==t_id||curtok->pro==t_longid)//	identifier
	{
		T->sons[0]=toksonend(curtok);
	}
	else
	{
		err("structure error!");
		return NULL;
	}
	
	if(*keycmp(curtok->name)==":")
	{
		i=0;
		ASTNODE* FT=newnode(ast_structure);
		FT->sons[i]=T;i++;
		FT->sons[i]=toksonend(curtok);i++;
		if(*keycmp(curtok->name)==">")//opaque annotation
		{
			FT->sons[i]=toksonend(curtok);i++;
		}
		//transparent annotation
		FT->sons[i]=parse_signature();
		cout<<"parsed structure"<<endl;
		#ifdef debug
			printast(FT);
		#endif
		return FT;
	}
	else
	{
		cout<<"parsed structure"<<endl;
		#ifdef debug
			printast(T);
		#endif
		return T;
	}
	
}


ASTNODE* parse_strbind()
{
	int i=0;
	ASTNODE* T=newnode(ast_strbind);
	
	i=0;
	if(curtok->pro==t_id)//structure
	{
		T->sons[i]=toksonend(curtok);i++;
	}
	else
	{
		err("strbind error!");
		return NULL;
	}
	
	if(*keycmp(curtok->name)==":")
	{
		T->sons[i]=toksonend(curtok);i++;
		if(*keycmp(curtok->name)==">")
		{
			T->sons[i]=toksonend(curtok);i++;
		}
		T->sons[i]=parse_signature();i++;
	}
	
	if(*keycmp(curtok->name)=="=")
	{
		T->sons[i]=toksonend(curtok);i++;

	}
	else
	{
		err("expected \'=\'");
		return NULL;
	}
	T->sons[i]=parse_structure();i++;
	
	
	if(*keycmp(curtok->name)=="and")
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=parse_strbind();i++;
	}
	
	return T;
	
}


ASTNODE* parse_signature()//signature解析 
{
	int i=0;
	ASTNODE* T=newnode(ast_signature);
	
	if(curtok->pro==t_id)//structure
	{
		i=0;
		T->sons[i]=toksonend(curtok);i++;
	}
	else if(*keycmp(curtok->name)=="sig") 
	{
		i=0;
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=parse_spec();i++;
		
		if(*keycmp(curtok->name)=="end")
		{
			T->sons[i]=toksonend(curtok);i++;
		}
		else
		{
			err("expected \'end\'");
			return NULL;
		}
	}
	else
	{
		err("signature error!");
		return NULL;
	} 
	
	if(curtok->pro==t_id||*keycmp(curtok->name)=="sig")
	{
		ASTNODE* FT=newnode(ast_signature);
		FT->sons[0]=T;
		if(*keycmp(curtok->name)=="where"&&*keycmp((curtok+1)->name)=="type")
		{
			FT->sons[1]=toksonend(curtok);
			FT->sons[2]=toksonend(curtok);
			FT->sons[3]=parse_typrefin();
		}
		else
		{
			return T;
		}
		
		cout<<"parsed signature"<<endl;
		return FT;
	}
	else
	{
		cout<<"parsed signature"<<endl;
		return T;
	}
}


ASTNODE* parse_typrefin()
{
	int i=0;
	ASTNODE* T=newnode(ast_typrefin);
	
	if(curtok->pro==t_id||curtok->pro==t_longid) //	type refinement
	{
		T->sons[i]=toksonend(curtok);i++;
	}
	else if(curtok->pro==t_var&&((curtok+1)->pro==t_id||(curtok+1)->pro==t_longid))
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=toksonend(curtok);i++;	
	} 
	else if(*keycmp(curtok->name)=="("&&(curtok+1)->pro==t_var)
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=toksonend(curtok);i++;
		if(*keycmp(curtok->name)==")")
		{
			T->sons[i]=toksonend(curtok);i++;//eat )
		}
		else if(*keycmp(curtok->name)==",")//constructor
		{
			while(*keycmp(curtok->name)==","&&(curtok+1)->pro==t_var)
			{
				T->sons[i]=toksonend(curtok);
				T->sons[i+1]=toksonend(curtok);
				i+=2;
			}			
			
			if(*keycmp(curtok->name)==")")
			{
				T->sons[i]=toksonend(curtok);i++;//eat )
				if(curtok->pro==t_id||curtok->pro==t_longid)
				{
					T->sons[i]=toksonend(curtok);i++;//eat id	
				}
				else
				{
					err("expected longid");
					return NULL;
				}					
			}
			else
			{
				err("expected \')\'");
				return NULL;
			}
		}
		else
		{
			err("expected \')\'");
			return NULL;
		}
	}
	else
	{
		err("typrefin error!");
		return NULL;
	}
	
	if(*keycmp(curtok->name)=="=")
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=parse_type();i++;
	}
	else
	{
		err("expected \'=\'");
		return NULL;
	}
	
	if(*keycmp(curtok->name)=="and"&&*keycmp((curtok+1)->name)=="type")
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=parse_typrefin();
	}
		
		
	return T;
		
}


ASTNODE* parse_spec()
{
	int i=0;
	ASTNODE* T=newnode(ast_spec);
	
	if(*keycmp(curtok->name)=="valdesc")
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=parse_valdesc();i++;
	}
	else if(*keycmp(curtok->name)=="typdesc")
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=parse_typdesc();i++;
	}
	else if(*keycmp(curtok->name)=="eqtype")
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=parse_typdesc();i++;
	}
	else if(*keycmp(curtok->name)=="type")
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=parse_typbind();i++;
	}
	else if(*keycmp(curtok->name)=="datatype")
	{
		T->sons[i]=toksonend(curtok);i++;
		
		if(curtok->pro==t_id&&(curtok+1)->name=="="&&(curtok+2)->name=="datatype")//data type replication
		{
			T->sons[i]=toksonend(curtok);i++;
			T->sons[i]=toksonend(curtok);i++;
			T->sons[i]=toksonend(curtok);i++;
			if(curtok->pro==t_id||curtok->pro==t_longid)
			{
				T->sons[i]=toksonend(curtok);i++;
			}
			else
			{
				err("expected longid");
				return NULL;
			}
		}
		else
		{
			T->sons[i]=parse_datdesc();i++;
		}
		
	}
	else if(*keycmp(curtok->name)=="exception")
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=parse_exndesc();i++;
	}
	else if(*keycmp(curtok->name)=="structure")
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=parse_strdesc();i++;
	}
	else if(*keycmp(curtok->name)=="include")//inclusion (n ≥ 1)
	{
		T->sons[i]=toksonend(curtok);i++;
		while(curtok->pro==t_id)
		{
			T->sons[i]=toksonend(curtok);i++;
		}
	}
	else
	{
		err("spec error!");
		return NULL;
	}
	
	
	if(*keycmp(curtok->name)=="valdesc"||*keycmp(curtok->name)=="typdesc"||*keycmp(curtok->name)=="eqtype"||*keycmp(curtok->name)=="type"||*keycmp(curtok->name)=="sharing"||
		*keycmp(curtok->name)=="datatype"||*keycmp(curtok->name)=="exception"||*keycmp(curtok->name)=="structure"||*keycmp(curtok->name)=="include"||*keycmp(curtok->name)==";")
	{
		i=0;
		ASTNODE* FT=newnode(ast_spec);
		FT->sons[i]=T;i++;
		if(*keycmp(curtok->name)=="sharing")//	structure sharing (n ≥ 2)
		{
			FT->sons[i]=toksonend(curtok);i++;
			
			if(*keycmp(curtok->name)=="type")//type sharing
			{
				FT->sons[i]=toksonend(curtok);i++;
			}
			
			if(curtok->pro==t_longid||curtok->pro==t_id)
			{
				FT->sons[i]=toksonend(curtok);i++;
			}
			else
			{
				err("expected longid");
				return NULL;
			}
			
			if(*keycmp(curtok->name)=="="&&((curtok+1)->pro==t_longid||(curtok+1)->pro==t_id))
			{
				while(*keycmp(curtok->name)=="="&&((curtok+1)->pro==t_longid||(curtok+1)->pro==t_id))
				{
					T->sons[i]=toksonend(curtok);
					T->sons[i+1]=toksonend(curtok);
					i+=2;
				}
			}
			else
			{
				err("expected \'=\' longid");
				return NULL;
			}
			return FT;
			
		}
		else//sequence
		{

			if(*keycmp(curtok->name)==";")
			{
				FT->sons[i]=toksonend(curtok);i++;
				FT->sons[i]=parse_spec();i++;
			}
			else
			{
				FT->sons[i]=parse_spec();i++;
			}
			return FT;
		}
		
	} 
	else
	{
		return T;
	}
}


ASTNODE* parse_valdesc()
{
	int i=0;
	ASTNODE* T=newnode(ast_valdesc);
	
	if(curtok->pro==t_id)//value
	{
		T->sons[i]=toksonend(curtok);i++;
	}
	else
	{
		err("spec error!");
		return NULL;
	}
	
	if(*keycmp(curtok->name)==":")
	{
		T->sons[i]=toksonend(curtok);i++;
	}
	else
	{
		err("expected \':\'");
		return NULL;
	}
	
	T->sons[i]=parse_type();i++;
	
	if(*keycmp(curtok->name)=="and")
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=parse_valdesc();i++;
	}
	return T;
}


ASTNODE* parse_typdesc()
{
	int i=0;
	ASTNODE* T=newnode(ast_typdesc);
	
	if(curtok->pro==t_id) 
	{
		T->sons[i]=toksonend(curtok);i++;
	}
	else if(curtok->pro==t_var&&(curtok+1)->pro==t_id)
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=toksonend(curtok);i++;	
	} 
	else if(*keycmp(curtok->name)=="("&&(curtok+1)->pro==t_var)
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=toksonend(curtok);i++;
		if(*keycmp(curtok->name)==")")
		{
			T->sons[i]=toksonend(curtok);i++;//eat )
			if(curtok->pro==t_id)
			{
				T->sons[i]=toksonend(curtok);i++;//eat id	
			}
			else
			{
				err("expected id");
				return NULL;
			}
		}
		else if(*keycmp(curtok->name)==",")//constructor
		{
			while(*keycmp(curtok->name)==","&&(curtok+1)->pro==t_var)
			{
				T->sons[i]=toksonend(curtok);
				T->sons[i+1]=toksonend(curtok);
				i+=2;
			}			
			
			if(*keycmp(curtok->name)==")")
			{
				T->sons[i]=toksonend(curtok);i++;//eat )
				if(curtok->pro==t_id)
				{
					T->sons[i]=toksonend(curtok);i++;//eat id	
				}
				else
				{
					err("expected id");
					return NULL;
				}					
			}
			else
			{
				err("expected \')\'");
				return NULL;
			}
		}
		else
		{
			err("expected \')\'");
			return NULL;
		}
	}
	else
	{
		err("typdesc error!");
		return NULL;
	}
	
	
	if(*keycmp(curtok->name)=="and")
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=parse_typdesc();i++;
	}
	
	return T;
}


ASTNODE* parse_datdesc()
{
	int i=0;
	ASTNODE* T=newnode(ast_datdesc);
	
	if(curtok->pro==t_id) 
	{
		T->sons[i]=toksonend(curtok);i++;
	}
	else if(curtok->pro==t_var&&(curtok+1)->pro==t_id)
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=toksonend(curtok);i++;	
	} 
	else if(*keycmp(curtok->name)=="("&&(curtok+1)->pro==t_var)
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=toksonend(curtok);i++;
		if(*keycmp(curtok->name)==")")
		{
			T->sons[i]=toksonend(curtok);i++;//eat )
			if(curtok->pro==t_id)
			{
				T->sons[i]=toksonend(curtok);i++;//eat id	
			}
			else
			{
				err("expected id");
				return NULL;
			}
		}
		else if(*keycmp(curtok->name)==",")//constructor
		{
			while(*keycmp(curtok->name)==","&&(curtok+1)->pro==t_var)
			{
				T->sons[i]=toksonend(curtok);
				T->sons[i+1]=toksonend(curtok);
				i+=2;
			}			
			
			if(*keycmp(curtok->name)==")")
			{
				T->sons[i]=toksonend(curtok);i++;//eat )
				if(curtok->pro==t_id)
				{
					T->sons[i]=toksonend(curtok);i++;//eat id	
				}
				else
				{
					err("expected id");
					return NULL;
				}					
			}
			else
			{
				err("expected \')\'");
				return NULL;
			}
		}
		else
		{
			err("expected \')\'");
			return NULL;
		}
	}
	else
	{
		err("typbind error!");
		return NULL;
	}
	
	if(*keycmp(curtok->name)=="=")
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=parse_condesc();i++;
	}
	else
	{
		err("expected \'=\'");
		return NULL;
	}
	

	if(*keycmp(curtok->name)=="and")
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=parse_datdesc();i++;
	}
	
	return T;
}


ASTNODE* parse_condesc()
{
	int i=0;
	ASTNODE* T=newnode(ast_condesc);
	
	if(curtok->pro==t_id)//data constructor
	{
		T->sons[i]=toksonend(curtok);i++;
	}
	else
	{
		err("conbind error!");
		return NULL;
	}
	
	if(*keycmp(curtok->name)=="of")
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=parse_type();i++;
	}
	
	if(*keycmp(curtok->name)=="|")
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=parse_condesc();i++;
	}
	
	return T;
}


ASTNODE* parse_exndesc()
{
	int i=0;
	ASTNODE* T=newnode(ast_exndesc);
	
	if(curtok->pro==t_id)
	{
		T->sons[i]=toksonend(curtok);i++;
	}
	else
	{
		err("exnbind error!");
		return NULL;
	}
	
	
	if(*keycmp(curtok->name)=="of")
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=parse_type();i++;
	}
	
	
	if(*keycmp(curtok->name)=="and")
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=parse_exndesc();i++;
	}
	
	return T;
}


ASTNODE* parse_strdesc()
{
	int i=0;
	ASTNODE* T=newnode(ast_strdesc);
	
	if(curtok->pro==t_id)
	{
		T->sons[i]=toksonend(curtok);i++;
	}
	else
	{
		err("strdesc error!");
		return NULL;
	}
	
	
	if(*keycmp(curtok->name)==":")
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=parse_signature();i++;
	}
	else
	{
		err("expected \':\'");
		return NULL;
	}
	
	if(*keycmp(curtok->name)=="and")
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=parse_strdesc();i++;
	}
	
	return T;
}

ASTNODE* parse_program()
{
	int i=0;
	ASTNODE* T=newnode(ast_program);
	
	if(*keycmp(curtok->name)=="val"||*keycmp(curtok->name)=="fun"||*keycmp(curtok->name)=="type"||*keycmp(curtok->name)=="datatype"||*keycmp(curtok->name)=="abstype"||
		*keycmp(curtok->name)=="exception"||*keycmp(curtok->name)=="structure"||*keycmp(curtok->name)=="local"||*keycmp(curtok->name)=="open"||
		*keycmp(curtok->name)=="nonfix"||*keycmp(curtok->name)=="infix"||*keycmp(curtok->name)=="infixr")
	{
		T->sons[i]=parse_declaration();
	}
	else if(*keycmp(curtok->name)=="functor")
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=parse_fctbind();
	}
	else if(*keycmp(curtok->name)=="signature")
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=parse_sigbind();
	}
	else
	{
		err("program error!");
		return NULL;
	}
	
	if(*keycmp(curtok->name)==";"||*keycmp(curtok->name)=="val"||*keycmp(curtok->name)=="fun"||*keycmp(curtok->name)=="type"||*keycmp(curtok->name)=="datatype"||*keycmp(curtok->name)=="abstype"||
		*keycmp(curtok->name)=="exception"||*keycmp(curtok->name)=="structure"||*keycmp(curtok->name)=="local"||*keycmp(curtok->name)=="open"||
		*keycmp(curtok->name)=="nonfix"||*keycmp(curtok->name)=="infix"||*keycmp(curtok->name)=="infixr"||
		*keycmp(curtok->name)=="functor"||*keycmp(curtok->name)=="signature")//sequence
	{
		ASTNODE* FT=newnode(ast_program);
		FT->sons[0]=T;
		if(*keycmp(curtok->name)==";")
		{
			FT->sons[1]=toksonend(curtok);
			FT->sons[2]=parse_program();
		}
		else
		{
			FT->sons[1]=parse_program();
		}
		
		cout<<"parsed program"<<endl;
		return FT;
	}
	else
	{
		cout<<"parsed program"<<endl;
		return T;
	}
}


ASTNODE* parse_fctbind()
{
	int i=0;
	ASTNODE* T=newnode(ast_fctbind);
	
	if(curtok->pro==t_id&&*keycmp((curtok+1)->name)=="(")
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=toksonend(curtok);i++;
		if(curtok->pro==t_id)
		{
			T->sons[i]=toksonend(curtok);i++;	
		}
		else
		{
			T->sons[i]=parse_spec();i++;
		}
		
		if(*keycmp(curtok->name)==")")
		{
			T->sons[i]=toksonend(curtok);i++;
		}
		else
		{
			err("expected \')\'");
		}
		
		if(*keycmp(curtok->name)==":")
		{
			T->sons[i]=toksonend(curtok);i++;
			if(*keycmp(curtok->name)==">")
			{
				T->sons[i]=toksonend(curtok);i++;
			}
			T->sons[i]=parse_signature();
		}
	
		if(*keycmp(curtok->name)=="=")
		{
			T->sons[i]=toksonend(curtok);i++;
			T->sons[i]=parse_structure();
		}
		else
		{
			err("expected \'=\'");
			return NULL;
		}
	
		if(*keycmp(curtok->name)=="and")
		{
			T->sons[i]=toksonend(curtok);i++;
			T->sons[i]=parse_fctbind();i++;
		}
		
		
	}
	else
	{
		err("fctbind error");
		return NULL;
	}
	return T;
}


ASTNODE* parse_sigbind()
{
	int i=0;
	ASTNODE* T=newnode(ast_sigbind);
	
	if(curtok->pro==t_id&&*keycmp((curtok+1)->name)=="=")
	{
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=toksonend(curtok);i++;
		T->sons[i]=parse_signature();i++;
		
		if(*keycmp(curtok->name)=="and")
		{
			T->sons[i]=toksonend(curtok);i++;
			T->sons[i]=parse_sigbind();i++;
		}
	}
	else
	{
		err("sigbind error");
		return NULL;
	}
	return T;
}











int main(void)  
{  
char a[SCANNER_MAX_INPUT_SIZE]; //文件缓冲区 
string b[256];  //暂存字符串 
  
int i=0;//词法 
static int j=0;
int k=0;//语法 

unsigned int row=1;//行数 
unsigned int col=1;//列数 
unsigned int hrow; 
unsigned int hcol;

/*
string scanner_infilename="";
cout<<"请输入要读取的文件："; 
cin>>scanner_infilename;
ifstream infile(scanner_infilename.c_str());*/
ifstream infile("input.txt",ios::in);//载入自己设定路径输入文件
/*
string scanner_outfilename="";
cout<<"分析结果输出为文件："; 
cin>>scanner_outfilename;
ofstream outfile(scanner_outfilename.c_str());  */ 
ofstream outfile("output.txt",ios::out);//载入自己设定路径输出文件   


for(i=0;i<SCANNER_MAX_INPUT_SIZE;i++)  
{  
 	a[i]=infile.get();//输入文件读取到数组a[]中    
}  


//scanner
for(i=0;i<SCANNER_MAX_INPUT_SIZE;i++)//开始进行数组a[]内字符识别   
{
#ifdef debug
outfile<<row<<"行"<<col<<"列 "<<" 开始识别位置 "<<i<<" "<<a[i]<<endl;
#endif

	if(a[i]==4) break;//结束符 
	
	
	if(a[i]=='('&&a[1+i]=='*')//注释 
	{
		i+=2;col+=2;
		while(!(a[i]=='*'&&a[i+1]==')')){ if(a[i]=='\n'){row+=1;col=1;}else{col++;}   i++;     if(i==SCANNER_MAX_INPUT_SIZE-1)break;}
		i+=1;col+=1;
	} 
	
	
	//常量con
	else if(a[i]=='#'&&a[i+1]==34&&(a[i+3]==34||(a[i+2]=='\\'&&(a[i+3]>='a'&&a[i+3]<='z')&&a[i+4]==34)))//字符 
	{
		hcol=col;hrow=row;
		if(a[i+3]==34)
		{
			b[j]=a[i+2];
			i+=3;col+=3;
			outfile<<hrow<<"行"<<hcol<<"列 "<<"字符 "<<b[0]<<endl;
			
			tok[k].pro=t_char;
			strcpy(tok[k].name,b[j].c_str());
			cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
			k++;
		}
		else
		{
			b[j]=a[i+2];
			b[j]+=a[i+3];
			i+=4;col+=4;
			outfile<<hrow<<"行"<<hcol<<"列 "<<"字符 "<<b[0]<<endl;
			
			tok[k].pro=t_char;
			strcpy(tok[k].name,b[j].c_str());
			cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
			k++;
		}
	}
	else if(a[i]==34)//字符串 
	{
		hrow=row;
		hcol=col;
		b[j]=a[i]; 
		i++;col++;
		while(a[i]!=34&&a[i]!='\n'){b[j]+=a[i];i++;    if(i==SCANNER_MAX_INPUT_SIZE-1)break;}
		if(a[i]=='\n') {row+=1;col=1;i++;}
		
		if(a[i]==34) 
		{	
			b[j]+=a[i]; 
			outfile<<hrow<<"行"<<hcol<<"列 "<<"字符串 "<<b[0]<<endl;//i++;col++;
			
			tok[k].pro=t_str;
			strcpy(tok[k].name,b[j].c_str());
			cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
			k++;
		}
		
	}
	else if(a[i]=='0'&&a[i+1]=='w'&&((a[i+2]=='x'&&((a[i+3]>='0'&&a[i+3]<='9')||(a[i+3]>='A'&&a[i+3]<='F')))||(a[i+2]>='0'&&a[i+2]<='9')))//字 
	{
		hrow=row;
		hcol=col;
		b[j]=a[i];
		b[j]+=a[i+1]; 
		i+=2;col+=2;
		if(a[i]!='x')
		{
			while(a[i]>='0'&&a[i]<='9'){b[j]+=a[i];i++;col++;}
			outfile<<hrow<<"行"<<hcol<<"列 "<<"字(dec) "<<b[0]<<endl;i--;col--;
			
			tok[k].pro=t_word;
			strcpy(tok[k].name,b[j].c_str());
			cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
			k++;
		}
		else
		{
			b[j]+=a[i];
			i++;col++;
			while((a[i]>='0'&&a[i]<='9')||(a[i]>='A'&&a[i]<='F')){b[j]+=a[i];i++;col++;}
			outfile<<hrow<<"行"<<hcol<<"列 "<<"字（hex） "<<b[0]<<endl;i--;col--;
			
			tok[k].pro=t_word;
			strcpy(tok[k].name,b[j].c_str());
			cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
			k++;
		}
	}
	else if(a[i]>='0'&&a[i]<='9'||a[i]=='~'&&a[i+1]>='0'&&a[i+1]<='9')//数值 
	{
		if(!(a[i]=='0'&&a[i+1]=='x'&&((a[i+2]>='0'&&a[i+2]<='9')||(a[i+2]>='A'&&a[i+2]<='F'))||a[i]=='~'&&(a[i+1]=='0'&&a[i+2]=='x'&&((a[i+3]>='0'&&a[i+3]<='9')||(a[i+3]>='A'&&a[i+3]<='F')))))
		{
			int pflag=0;//小数点出现一次的标志 
			int eflag=0;//e出现一次的标志 
			int mflag=0;//负号在第一个字符出现的标志 
			hrow=row;
			hcol=col;
			b[j]=a[i];if(a[i]=='~') mflag=1;
			i++;col++;
			while(a[i]>='0'&&a[i]<='9'||a[i]=='.')  {if(a[i]=='.'&&pflag==1)break;if(a[i]=='.')pflag=1; b[j]+=a[i];i++;col++; } 
		
			if((a[i]=='E'||a[i]=='e')&&((a[i+1]=='~'&&(a[i+2]>='0'||a[i+2]<='9'))||(a[i+1]>='0'&&a[i+1]<='9'))) 
			{
				eflag=1;
				b[j]+=a[i];b[j]+=a[i+1];
				i+=2;col+=2;
				while(a[i]>='0'&&a[i]<='9'){ b[j]+=a[i];i++;col++; }
			}
			
			if(pflag==1||eflag==1)
			{
				outfile<<hrow<<"行"<<hcol<<"列 "<<"浮点数 "<<b[0]<<endl;i--;col--;
				
				tok[k].pro=t_float;
				strcpy(tok[k].name,b[j].c_str());
				cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
				k++;
			}
			else
			{
				outfile<<hrow<<"行"<<hcol<<"列 "<<"整数(dec) "<<b[0]<<endl;i--;col--;
				if(mflag)
				{
					tok[k].pro=t_int;
					strcpy(tok[k].name,b[j].c_str());
					cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
					k++;
				}
				else
				{
					tok[k].pro=t_int_or_lab;
					strcpy(tok[k].name,b[j].c_str());
					cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
					k++;
				}
			}
		
			pflag=0;
			eflag=0;
			mflag=0; 
		}
		else
		{
			hrow=row;
			hcol=col;
			b[j]=a[i];
			b[j]+=a[i+1];
			b[j]+=a[i+2];
			i+=3;col+=3;
			while((a[i]>='0'&&a[i]<='9')||(a[i]>='A'&&a[i]<='F')){b[j]+=a[i];i++;col++;}
			outfile<<hrow<<"行"<<hcol<<"列 "<<"整数(hex) "<<b[0]<<endl;i--;col--;
				
					tok[k].pro=t_int;
					strcpy(tok[k].name,b[j].c_str());
					cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
					k++;
		}
		
	}
	
	
	
	
	//标识符id var longid
	else if(a[i]>='A'&&a[i]<='Z'||a[i]>='a'&&a[i]<='z')//标识符 id longid
	{
		int lflag=0;//长标识符标志 
		int kwflag=0;//关键字标志 
		b[j]=a[i];
		hrow=row;
		hcol=col;
		i++;col++;
		while(a[i]>='A'&&a[i]<='Z'||a[i]>='a'&&a[i]<='z'||a[i]>='0'&&a[i]<='9'||a[i]=='_'||a[i]==39) { b[j]+=a[i];i++;col++; }
		
		while(a[i]=='.'&&(a[i+1]>='A'&&a[i+1]<='Z'||a[i+1]>='a'&&a[i+1]<='z'))
		{
			b[j]+=a[i];i++;col++;
			while(a[i]>='A'&&a[i]<='Z'||a[i]>='a'&&a[i]<='z'||a[i]>='0'&&a[i]<='9'||a[i]=='_'||a[i]==39) { b[j]+=a[i];i++;col++; }
			lflag=1;
		}
		
		if(lflag==0)
		{
			if(b[j]=="val"&&kwflag==0){kwflag=1;		}
			if(b[j]=="if"&&kwflag==0){kwflag=1;			}
			if(b[j]=="then"&&kwflag==0){kwflag=1;		}
			if(b[j]=="else"&&kwflag==0){kwflag=1;		}
			if(b[j]=="fun"&&kwflag==0){kwflag=1;		}
			if(b[j]=="case"&&kwflag==0){kwflag=1;		}
			if(b[j]=="end"&&kwflag==0){kwflag=1;		}
			if(b[j]=="in"&&kwflag==0){kwflag=1;			}
			if(b[j]=="orelse"&&kwflag==0){kwflag=1;		}
			if(b[j]=="not"&&kwflag==0){kwflag=1;		}
			if(b[j]=="andalso"&&kwflag==0){kwflag=1;	}
			if(b[j]=="as"&&kwflag==0){kwflag=1;			}
			if(b[j]=="let"&&kwflag==0){kwflag=1;		}
			if(b[j]=="of"&&kwflag==0){kwflag=1;			}
			if(b[j]=="datatype"&&kwflag==0){kwflag=1;	}
			if(b[j]=="fn"&&kwflag==0){kwflag=1;			}
			if(b[j]=="raise"&&kwflag==0){kwflag=1;		}
			if(b[j]=="exception"&&kwflag==0){kwflag=1;	}
			if(b[j]=="div"&&kwflag==0){kwflag=2;		}//
			if(b[j]=="handle"&&kwflag==0){kwflag=1;		}
			if(b[j]=="structure"&&kwflag==0){kwflag=1;	}
			if(b[j]=="struct"&&kwflag==0){kwflag=1;		}
			if(b[j]=="signature"&&kwflag==0){kwflag=1;	}
			if(b[j]=="sig"&&kwflag==0){kwflag=1;		}
			if(b[j]=="functor"&&kwflag==0){kwflag=1;	}
			if(b[j]=="local"&&kwflag==0){kwflag=1;		}
			if(b[j]=="type"&&kwflag==0){kwflag=1;		}
			if(b[j]=="include"&&kwflag==0){kwflag=1;	}
			if(b[j]=="eqtype"&&kwflag==0){kwflag=1;		}
			if(b[j]=="where"&&kwflag==0){kwflag=1;		}
			if(b[j]=="sharing"&&kwflag==0){kwflag=1;	}
			if(b[j]=="with"&&kwflag==0){kwflag=1;		}
			if(b[j]=="withtype"&&kwflag==0){kwflag=1;	}
			if(b[j]=="abstype"&&kwflag==0){kwflag=1;	}
			if(b[j]=="open"&&kwflag==0){kwflag=1;		}
			if(b[j]=="infix"&&kwflag==0){kwflag=1;		}
			if(b[j]=="nonfix"&&kwflag==0){kwflag=1;		}
			if(b[j]=="infixr"&&kwflag==0){kwflag=1;		}
			if(b[j]=="hd"&&kwflag==0){kwflag=3;			}//
			if(b[j]=="tl"&&kwflag==0){kwflag=3;			}//
			if(b[j]=="ord"&&kwflag==0){kwflag=3;		}//
			if(b[j]=="chr"&&kwflag==0){kwflag=3;		}//
			if(b[j]=="explode"&&kwflag==0){kwflag=3;	}// 
			if(b[j]=="implode"&&kwflag==0){kwflag=3;	}// 
			if(b[j]=="size"&&kwflag==0){kwflag=3;		}//
			//if(b[j]=="true"&&kwflag==0){kwflag=1;		}			
			//if(b[j]=="false"&&kwflag==0){kwflag=1;	}
			if(b[j]=="while"&&kwflag==0){kwflag=1;		}
			if(b[j]=="do"&&kwflag==0){kwflag=1;			}
			if(b[j]=="rec"&&kwflag==0){kwflag=1;		}
			if(b[j]=="and"&&kwflag==0){kwflag=1;		}			
			
			if(kwflag==1) 
			{
				outfile<<hrow<<"行"<<hcol<<"列 "<<"关键字 "<<b[0]<<endl;i--;col--;
					
					tok[k].pro=t_kword;
					strcpy(tok[k].name,b[j].c_str());
					cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
					k++;
			}
			else if(kwflag==2)
			{
				outfile<<hrow<<"行"<<hcol<<"列 "<<"关键字 "<<b[0]<<endl;i--;col--;
					
					tok[k].pro=t_binop;
					strcpy(tok[k].name,b[j].c_str());
					cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
					k++;
			}
			else if(kwflag==3)
			{
				outfile<<hrow<<"行"<<hcol<<"列 "<<"关键字 "<<b[0]<<endl;i--;col--;
					
					tok[k].pro=t_sinop;
					strcpy(tok[k].name,b[j].c_str());
					cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
					k++;
			}
			else
			{
				outfile<<hrow<<"行"<<hcol<<"列 "<<"标识符 "<<b[0]<<endl;i--;col--;
					
					tok[k].pro=t_id;
					strcpy(tok[k].name,b[j].c_str());
					cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
					k++; 
			}
		}
		else
		{
			outfile<<hrow<<"行"<<hcol<<"列 "<<"长标识符 "<<b[0]<<endl;i--;col--;
					
					tok[k].pro=t_longid;
					strcpy(tok[k].name,b[j].c_str());
					cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
					k++;
		}
		kwflag=0;
		lflag=0;
	}
	else if(a[i]==39)//变量 var
	{
		b[j]=a[i];
		hrow=row;
		hcol=col;
		i++;col++;
		if(a[i]==39)
		{
			b[j]+=a[i];
			i++;col++;
			while(a[i]>='A'&&a[i]<='Z'||a[i]>='a'&&a[i]<='z'||a[i]>='0'&&a[i]<='9'||a[i]=='_'||a[i]==39) { b[j]+=a[i];i++;col++; }
			outfile<<hrow<<"行"<<hcol<<"列 "<<"变量(equ) "<<b[0]<<endl;i--;col--;
			
			
					tok[k].pro=t_var;
					strcpy(tok[k].name,b[j].c_str());
					cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
					k++;
		}
		else
		{
			while(a[i]>='A'&&a[i]<='Z'||a[i]>='a'&&a[i]<='z'||a[i]>='0'&&a[i]<='9'||a[i]=='_'||a[i]==39) { b[j]+=a[i];i++;col++; }
			outfile<<hrow<<"行"<<hcol<<"列 "<<"变量(unc) "<<b[0]<<endl;i--;col--;
			
					tok[k].pro=t_var;
					strcpy(tok[k].name,b[j].c_str());
					cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
					k++;
		}
	}
	else 
	{
		switch(a[i])//sym,op
		{
		case'.':{
			if(a[i+1]=='.'&&a[i+2]=='.')
			{
			
			b[j]=a[i];
			hrow=row;
			hcol=col;
			b[j]+=a[i+1];
			b[j]+=a[i+2];
			i+=2;col+=2;
			//i++;col++;
			outfile<<hrow<<"行"<<hcol<<"列 "<<"符号 "<<b[0]<<endl;
			
					tok[k].pro=t_sym;
					strcpy(tok[k].name,b[j].c_str());
					cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
					k++;
			}
			else;
			break;
		}
		
		case'_':{
			b[j]=a[i];
			hrow=row;
			hcol=col;
			//i++;col++;
			outfile<<hrow<<"行"<<hcol<<"列 "<<"符号 "<<b[0]<<endl;
			
					tok[k].pro=t_sym;
					strcpy(tok[k].name,b[j].c_str());
					cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
					k++;
			break;
		}
		
		case'(':{
			b[j]=a[i];
			hrow=row;
			hcol=col;
			//i++;col++;
			outfile<<hrow<<"行"<<hcol<<"列 "<<"符号 "<<b[0]<<endl;
			
					tok[k].pro=t_sym;
					strcpy(tok[k].name,b[j].c_str());
					cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
					k++;
			break;
		}
		
		case')':{
			b[j]=a[i];
			hrow=row;
			hcol=col;
			//i++;col++;
			outfile<<hrow<<"行"<<hcol<<"列 "<<"符号 "<<b[0]<<endl;
			
					tok[k].pro=t_sym;
					strcpy(tok[k].name,b[j].c_str());
					cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
					k++;
			break;
		}
		
		case'[':{
			b[j]=a[i];
			hrow=row;
			hcol=col;
			//i++;col++;
			outfile<<hrow<<"行"<<hcol<<"列 "<<"符号 "<<b[0]<<endl;
			
					tok[k].pro=t_sym;
					strcpy(tok[k].name,b[j].c_str());
					cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
					k++;
			break;
		}
		
		case']':{
			b[j]=a[i];
			hrow=row;
			hcol=col;
			//i++;col++;
			outfile<<hrow<<"行"<<hcol<<"列 "<<"符号 "<<b[0]<<endl;
			
					tok[k].pro=t_sym;
					strcpy(tok[k].name,b[j].c_str());
					cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
					k++;
			break;
		}
		
		case '{':{//
			b[j]=a[i];
			hrow=row;
			hcol=col;
			//i++;col++;
			outfile<<hrow<<"行"<<hcol<<"列 "<<"符号 "<<b[0]<<endl;
			
					tok[k].pro=t_sym;
					strcpy(tok[k].name,b[j].c_str());
					cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
					k++;
			break;
		}
		
		case '}':{//
			b[j]=a[i];
			hrow=row;
			hcol=col;
			//i++;col++;
			outfile<<hrow<<"行"<<hcol<<"列 "<<"符号 "<<b[0]<<endl;
			
					tok[k].pro=t_sym;
					strcpy(tok[k].name,b[j].c_str());
					cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
					k++;
			break;
		}
		
		case':':{
			if(a[i+1]==':')
			{
				b[j]=a[i];
				b[j]+=a[i+1]; 
				hrow=row;
				hcol=col;
				i+=1;col+=1;
				outfile<<hrow<<"行"<<hcol<<"列 "<<"符号 "<<b[0]<<endl;
				
					tok[k].pro=t_binop;
					strcpy(tok[k].name,b[j].c_str());
					cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
					k++;
			}
			else
			{
				b[j]=a[i];
				hrow=row;
				hcol=col;
				//i++;col++;
				outfile<<hrow<<"行"<<hcol<<"列 "<<"符号 "<<b[0]<<endl;
				
					tok[k].pro=t_sym;
					strcpy(tok[k].name,b[j].c_str());
					cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
					k++;
			}
			
			break;
		}
		
		case';':{
			b[j]=a[i];
			hrow=row;
			hcol=col;
			//i++;col++;
			outfile<<hrow<<"行"<<hcol<<"列 "<<"符号 "<<b[0]<<endl;
			
					tok[k].pro=t_sym;
					strcpy(tok[k].name,b[j].c_str());
					cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
					k++;
			break;
		}
		
		case'|':{
			b[j]=a[i];
			hrow=row;
			hcol=col;
			//i++;col++;
			outfile<<hrow<<"行"<<hcol<<"列 "<<"符号 "<<b[0]<<endl;
			
					tok[k].pro=t_sym;
					strcpy(tok[k].name,b[j].c_str());
					cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
					k++;
			break;
		}
		
		case',':{
			b[j]=a[i];
			hrow=row;
			hcol=col;
			//i++;col++;
			outfile<<hrow<<"行"<<hcol<<"列 "<<"符号 "<<b[0]<<endl;
			
					tok[k].pro=t_sym;
					strcpy(tok[k].name,b[j].c_str());
					cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
					k++;
			break;
		}
		
		//str
		case'^':{
			b[j]=a[i];
			hrow=row;
			hcol=col;
			//i++;col++;
			outfile<<hrow<<"行"<<hcol<<"列 "<<"符号 "<<b[0]<<endl;
			
					tok[k].pro=t_binop;
					strcpy(tok[k].name,b[j].c_str());
					cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
					k++;
			break;
		}
		//sharp
		case'#':{
			b[j]=a[i];
			hrow=row;
			hcol=col;
			//i++;col++;
			outfile<<hrow<<"行"<<hcol<<"列 "<<"符号 "<<b[0]<<endl;
			
					tok[k].pro=t_sym;
					strcpy(tok[k].name,b[j].c_str());
					cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
					k++;
			break;
		}
		//equ
		case'=':{
			if(a[i+1]=='>')
			{
				b[j]=a[i];
				b[j]+=a[i+1]; 
				hrow=row;
				hcol=col;
				i+=1;col+=1;
				outfile<<hrow<<"行"<<hcol<<"列 "<<"符号 "<<b[0]<<endl;
				
					tok[k].pro=t_sym;
					strcpy(tok[k].name,b[j].c_str());
					cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
					k++;
			}
			else
			{
				b[j]=a[i];
				hrow=row;
				hcol=col;
				//i++;col++;
				outfile<<hrow<<"行"<<hcol<<"列 "<<"符号 "<<b[0]<<endl;
				
					tok[k].pro=t_sym;
					strcpy(tok[k].name,b[j].c_str());
					cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
					k++;
			}
			break;
		}
		
		case'>':{
			if(a[i+1]=='=')
			{
				b[j]=a[i];
				b[j]+=a[i+1]; 
				hrow=row;
				hcol=col;
				i+=1;col+=1;
				outfile<<hrow<<"行"<<hcol<<"列 "<<"符号 "<<b[0]<<endl;
				
					tok[k].pro=t_binop;
					strcpy(tok[k].name,b[j].c_str());
					cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
					k++;
			}
			else
			{
				b[j]=a[i];
				hrow=row;
				hcol=col;
				//i++;col++;
				outfile<<hrow<<"行"<<hcol<<"列 "<<"符号 "<<b[0]<<endl;
				
					tok[k].pro=t_binop;
					strcpy(tok[k].name,b[j].c_str());
					cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
					k++;
			}
			break;
		}
		
		case'<':{
			if(a[i+1]=='=')
			{
				b[j]=a[i];
				b[j]+=a[i+1]; 
				hrow=row;
				hcol=col;
				i+=1;col+=1;
				outfile<<hrow<<"行"<<hcol<<"列 "<<"符号 "<<b[0]<<endl;
				
					tok[k].pro=t_binop;
					strcpy(tok[k].name,b[j].c_str());
					cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
					k++;
			}
			else if(a[i+1]=='>')
			{
				b[j]=a[i];
				b[j]+=a[i+1]; 
				hrow=row;
				hcol=col;
				i+=1;col+=1;
				outfile<<hrow<<"行"<<hcol<<"列 "<<"符号 "<<b[0]<<endl;
				
					tok[k].pro=t_binop;
					strcpy(tok[k].name,b[j].c_str());
					cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
					k++;
			}
			else
			{
				b[j]=a[i];
				hrow=row;
				hcol=col;
				//i++;col++;
				outfile<<hrow<<"行"<<hcol<<"列 "<<"符号 "<<b[0]<<endl;
				
					tok[k].pro=t_binop;
					strcpy(tok[k].name,b[j].c_str());
					cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
					k++;
			}
			break;
		}
		
		case'+':{
			b[j]=a[i];
			hrow=row;
			hcol=col;
			//i++;col++;
			outfile<<hrow<<"行"<<hcol<<"列 "<<"符号 "<<b[0]<<endl;
			
					tok[k].pro=t_binop;
					strcpy(tok[k].name,b[j].c_str());
					cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
					k++;
			break;
		}
		
		case'-':{
			if(a[i+1]=='>')
			{
				b[j]=a[i];
				b[j]+=a[i+1]; 
				hrow=row;
				hcol=col;
				i+=1;col+=1;
				outfile<<hrow<<"行"<<hcol<<"列 "<<"符号 "<<b[0]<<endl;
				
					tok[k].pro=t_sym;
					strcpy(tok[k].name,b[j].c_str());
					cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
					k++;
			}
			else
			{
				b[j]=a[i];
				hrow=row;
				hcol=col;
				//i++;col++;
				outfile<<hrow<<"行"<<hcol<<"列 "<<"符号 "<<b[0]<<endl;
				
					tok[k].pro=t_binop;
					strcpy(tok[k].name,b[j].c_str());
					cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
					k++;
			}
			break;
		}
		
		case'*':{
			b[j]=a[i];
			hrow=row;
			hcol=col;
			//i++;col++;
			outfile<<hrow<<"行"<<hcol<<"列 "<<"符号 "<<b[0]<<endl;
			
					tok[k].pro=t_binop;
					strcpy(tok[k].name,b[j].c_str());
					cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
					k++;
			break;
		}
		
		case'/':{
			b[j]=a[i];
			hrow=row;
			hcol=col;
			//i++;col++;
			outfile<<hrow<<"行"<<hcol<<"列 "<<"符号 "<<b[0]<<endl;
			
					tok[k].pro=t_binop;
					strcpy(tok[k].name,b[j].c_str());
					cout<<"token流:"<<k<<" 属性:"<<tok[k].pro<<" 名称 "<<tok[k].name<<endl;
					k++;
			break;
		}
		
		
			default:{
				break;
			}
		}

	}
	

	col++;
	if(a[i]=='\n')
	{
		
		row++;
		col=1;
	}
	
#ifdef debug 
outfile<<row<<"行"<<col<<"列 "<<" 结束时位置 "<<i<<" "<<a[i]<<endl;
#endif
	
}


//parser

ASTNODE* root=newnode(0);//根节点 

i=0;
root->sons[i]=parse_program();i++;

while(root->sons[i-1]!=NULL)
{
	root->sons[i]=parse_program();
	i++;
}

cout<<"识别到的语法树"<<endl; 
printast(root);

#ifdef debug
cout<<curtok->name<<endl;
#endif

 	cout<<"分析完毕"<<endl; //dos界面显示,则分析成功   
    outfile.close();//关闭文件   
    infile.close();//关闭文件 
	system("pause");  
    return 0 ;  
}
