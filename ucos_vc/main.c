/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
description:	��ǰһ���汾�Ļ�����������cd����

date��			20050429
author��		�ļ� Email��ganganwen@163.com 
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

HANDLE mainhandle;		//���߳̾��
CONTEXT Context;		//���߳��л�������
BOOLEAN FlagEn = 1;		//����һ��ȫ�ֱ�������Ϊ�Ƿ�ʱ�ӵ��ȵı�־


/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void TaskStart(void * pParam) ;
void Task1(void * pParam) ;                            /* Function prototypes of tasks                  */

void VCInit(void);						//��ʼ����ر���,һ����Ҫ

/*$PAGE*/
/*
*********************************************************************************************************
*                                                MAIN
*********************************************************************************************************
*/

int main(int argc, char **argv)
{
	VCInit();	//��ʼ��һЩ����
	
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
	cp = GetCurrentProcess();	//�õ���ǰ���̾��
	ct = GetCurrentThread();	//�õ���ǰ�߳�α���
	DuplicateHandle(cp, ct, cp, &mainhandle, 0, TRUE, 2);	//α���ת��,�õ��߳�����
		
}

void TaskStart(void * pParam) 
{	
	char err;	
	OS_EVENT *sem1;

	timeSetEvent(1000/OS_TICKS_PER_SEC, 0, OSTickISR, 0, TIME_PERIODIC);	//����һ����ʱ���߳�,�о�10 ticks/s�ȽϺ�
	sem1 = OSSemCreate(0);
	while(1)
	{		
		OSSemPend(sem1, 0, &err);   //sleep ,wait for sem1,run shell
	}
}
