/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
description:	在前一个版本的基础上增加了cd命令

date：			20050429
author：		文佳 Email：ganganwen@163.com 
*********************************************************************************************************
*/

#include "includes.h"
#include "fs_api.h"
/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/

#define  TASK_STK_SIZE                 2048       /* Size of each task's stacks (# of WORDs)            */
#define  N_TASKS                        10       /* Number of identical tasks                          */

#define TaskStart_Prio	1
#define Task1_Prio		2

/*
*********************************************************************************************************
*                                               VARIABLES
*********************************************************************************************************
*/

OS_STK  TaskStk[N_TASKS][TASK_STK_SIZE];    // Tasks stacks

HANDLE mainhandle;		//主线程句柄
CONTEXT Context;		//主线程切换上下文
BOOLEAN FlagEn = 1;		//增加一个全局变量，做为是否时钟调度的标志


/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void TaskStart(void * pParam) ;
void Task1(void * pParam) ;                            /* Function prototypes of tasks                  */

void VCInit(void);						//初始化相关变量,一定需要

/*$PAGE*/
/*
*********************************************************************************************************
*                                                MAIN
*********************************************************************************************************
*/

int main(int argc, char **argv)
{
	VCInit();	//初始化一些变量
	
	OSInit();
	FS_Init();         /* Init the file system */
	OSTaskCreate(TaskStart, 0, &TaskStk[0][TASK_STK_SIZE-1], TaskStart_Prio);
	OSTaskCreate(shelltask, 0, &TaskStk[1][TASK_STK_SIZE-1], Task1_Prio);

	OSStart();	//start never return
	FS_Exit();         /* End using the file system */
	return 0;
}

void VCInit(void)
{
	HANDLE cp,ct;
	Context.ContextFlags = CONTEXT_CONTROL;
	cp = GetCurrentProcess();	//得到当前进程句柄
	ct = GetCurrentThread();	//得到当前线程伪句柄
	DuplicateHandle(cp, ct, cp, &mainhandle, 0, TRUE, 2);	//伪句柄转换,得到线程真句柄
		
}

void TaskStart(void * pParam) 
{	
	char err;	
	OS_EVENT *sem1;

	timeSetEvent(1000/OS_TICKS_PER_SEC, 0, OSTickISR, 0, TIME_PERIODIC);	//开启一个定时器线程,感觉10 ticks/s比较好
	sem1 = OSSemCreate(0);
	while(1)
	{		
		OSSemPend(sem1, 0, &err);   //sleep ,wait for sem1,run shell
	}
}
