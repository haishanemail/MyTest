/*	 
*	shelltask.c
*	the genie shell task for ucosII
*	under skyeye
*
*	Bugs report:	 Yang Ye  ( yangye@163.net )
*	Last modified:	 2005-05-6 
*  changed by: wenjia
*/

#include   "includes.h"


extern command ShellComms[MAX_COMMAND_NUM];
extern void CommRxIntEn(INT8U ch);
extern char CurDir[64];
char *argv[10];
INT8U argc;

//BOOLEAN PC_GetKey (INT8U *c)
//{
 //   if (kbhit()) {                                         /* See if a key has been pressed            */
  //      *c = getch();                            /* Get key pressed                          */
  //      return (TRUE);
  //  } else {
   //     *c = 0x00;                                         /* No key pressed                           */
 //       return (FALSE);
//    }
//}

extern void InitRamDisk();
void shelltask(void *pParam)
{
	INT8U i=0,num;		/*i is the pointer of commandbuf */
	char ch;
	INT8U (*Func)(INT8U argc,char **argv);
	char CommandBuf[MaxLenComBuf+1];	/*store '\0'*/
	InitCommands();
	CommandBuf[0] = '\0';
	
	//to do: add some lib functions for ucosII ,like clear screen .
	//clrscr();
	_log("\n\r**************************************************************\n");
	_log("\n\r*		 Welcom to genie shell	                 \n");
	_log("\n\r*		   Author:YangYe 20021102   	         \n");
	_log("\n\r*		VC version by нд╪я 20050409            \n");
	_log("\n\r*	  use \"help\" command to help yourself    \n");
	_log("\n\r*		bug report: ganganwen@163.com			\n");
	_log("\n\r**************************************************************\n\n");
	
	/*To be done: Login & Password*/
	InitRamDisk();
    ShowHelp();
	_log("RAM:");
	_log(CurDir);
	_log(">");
    
	for(;;){
		do
		{							//only accept a-z,0-9,A-Z,.,space,/,-
			if (!PC_GetKey((INT16S *)&ch))	 //if no keyevent happen sleep 1 ticks
		        OSTimeDly(1);
		}while(!((ch>='0'&&ch<='9')||(ch>='a'&&ch<='z')||(ch>='A'&&ch<='Z')||(ch=='.')||(ch==' ')||(ch==':')||(ch=='\\')||(ch=='-')||(ch=='/')||(ch==0xd)||(ch=='\b')||(ch==',')));
		switch(ch)
		{
		case 0xd:				//enter
			if (i==0)
			{      						//commandbuf is null,begin a new line
				_log("\nRAM:");
				_log(CurDir);
				_log(">");
			}
			else{
				if(CommandBuf[i-1]==' ') 
					i--;			//get rid of the end space
				CommandBuf[i] = '\0';
				num = CommandAnalys(CommandBuf);	//analys the argv in the commandbuf
				if(num==ERRORCOMMAND)
				{             	//error or none exist command
					i = 0;
					CommandBuf[i] = '\0';
					_log("error command!\n\n\r");
					_log("RAM:");
					_log(CurDir);
					_log(">");
				}
				else{
					Func = ShellComms[num].CommandFunc;	//call corresponding CommandFunc
					Func(argc,argv);
					i = 0;
					CommandBuf[i] = '\0';
					_log("RAM:");
					_log(CurDir);
					_log(">");
				}
			}
			break;
			
		case '\b':				//backspace
			if ( i==0 ){		//has backed to first one
				//do nothing
			}
			else{
				i--;			//pointer back once
				OS_ENTER_CRITICAL();
				putchar('\b');		//cursor back once
				putchar(' ');	      //earse last char in screen
				putchar('\b');		//cursor back again
				OS_ENTER_CRITICAL();
			}
			break;
			
		case ' ':               //don't allow continuous or begin space(' ')
			if((CommandBuf[i-1] == ' ')||(i==0)||(i>MaxLenComBuf)){
				//do nothing
			}
			else
			{
				CommandBuf[i] = ch;
				i++;
				OS_ENTER_CRITICAL();
				putchar(ch);  //display and store ch
				OS_ENTER_CRITICAL();
			}
			break;
			
		default:				//normal key
			if (i>MaxLenComBuf){	//the buf reached MAX 
				//do nothing
			}			
			else{
				CommandBuf[i] = ch;
				i++;
				OS_ENTER_CRITICAL();
				putchar(ch);  //display and store ch
				OS_ENTER_CRITICAL();
			}
			break;
		}  //switch
	}//for(;;)
}

INT8U CommandAnalys(char *Buf)
{
	INT8U i;
	INT8U pointer;
	INT8U num;
	char name[20];		//command name length <20
	
	argc = 0;              //argc is global
	pointer = 0;
	num = 0;
	_log("\n\r");
	
	while((Buf[pointer]!=' ') && (Buf[pointer]!='\0') && pointer<20 ){
		name[pointer]=Buf[pointer];
		pointer++;
	}
	name[pointer] = '\0';	//now got the command name, and pointer is to the first space in the Buf
	
	for(i=0;i<MAX_COMMAND_NUM;i++){
		if(!strcmp(name,ShellComms[i].name)){
			num = i;
			break;
		}				//find the command number
	}					
	//not find it
	if (i==MAX_COMMAND_NUM) return ERRORCOMMAND;
	
	while(Buf[pointer]!='\0'){
		if(Buf[pointer]==' '){
			if(argc>0){
				Buf[pointer] = '\0';			//end of last argv
			}
			pointer++;
			argv[argc] = &Buf[pointer];			//add a parameter for every space
			argc++;
		}
		else{
			pointer++;
		}
	}//while
	
	return num;
}
