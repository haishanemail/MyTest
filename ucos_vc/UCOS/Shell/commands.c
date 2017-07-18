/*	 
 *	commands.c
 *	the genie shell command part for ucosII
 *	under skyeye
 *
 *	Bugs report:	 ganganwen@163.com
 *	Last modified:	 2005-04-25 
 *  changed by: wenjia
 */


#include	"includes.h"



command ShellComms[MAX_COMMAND_NUM];

FS_FILE *myfile;
char mybuffer[0x100];

char CurDir[64]="\\";		//show current dir
char TargetVal[64] = "\\";  
//TargetVal�����һ���ܹ��ܣ�������ϵ�ǰĿ¼������������ 
//CurDir = "\mydir"   cat 1.txt ʱ��ϳ�cat \mydir\1.txt
//�ڶ����ܣ�����CurDirĿ¼����Ϊ���cd������һ���ַ�Ϊ\ʱ�ʹ�&CurDir[0] 
//��ʼ���ƣ���������Ŀ¼������ԭ����CurDirĿ¼��ԭ��

INT8U DirExist(const char *DirName);
void RewindRoot(void);


void CreateTargetVal(const char *arg);

//�ж�Ŀ¼���Ƿ����
//1:Ŀ¼����  0:Ŀ¼������
INT8U DirExist(const char *DirName)
{
    FS_DIR *dirp;
    dirp = FS_OpenDir(DirName);
    if (dirp)
    {
        FS_CloseDir(dirp);
       // _log("Open %s Exist!\n", DirName);
        return 1;
    } else {
       // _log("The %s Dir is not exist!\n", DirName);
        return 0;
    }
} 
//--------------------------------------------
//#define CUR_DRI_MAX_LEN 32
//FS_DIR CurDir[CUR_DRI_MAX_LEN 32];
 // struct FS_DIRENT *direntp;
/*********************************************************************
*
*             _write_file
*
  This routine demonstrates, how to create and write to a file
  using the file system.
*/

static void _write_file(const char *name, const char *txt) {
  int x;
  
  /* create file */
  myfile = FS_FOpen(name,"w");
  if (myfile) {
    /* write to file */
    x = FS_FWrite(txt,1,strlen(txt),myfile);
    /* all data written ? */
    if (x!=(int)strlen(txt)) {
      /* check, why not all data was written */
      x = FS_FError(myfile);
      sprintf(mybuffer,"Not all bytes written because of error %d.\n\n",x);
      _error(mybuffer);
    }
    /* close file */
    FS_FClose(myfile);
	_log("\n\n");
  }
  else {
    sprintf(mybuffer,"Unable to create file %s\n\n",name);
    _error(mybuffer);
  }
}


/*********************************************************************
*
*             _dump_file
*
  This routine demonstrates, how to open and read from a file using 
  the file system.
*/

static void _dump_file(const char *name) {
  int x;

  /* open file */
  myfile = FS_FOpen(name,"r");
  if (myfile) {
    /* read until EOF has been reached */
    do {
      x = FS_FRead(mybuffer,1,sizeof(mybuffer)-1,myfile);
      mybuffer[x]=0;
      if (x) {
        _log(mybuffer);
      }
    } while (x);
    /* check, if there is no more data, because of EOF */
    x = FS_FError(myfile);
    if (x!=FS_ERR_EOF) {
      /* there was a problem during read operation */
      sprintf(mybuffer,"Error %d during read operation.\n",x);
      _error(mybuffer);
    }
    /* close file */
    FS_FClose(myfile);
	_log("\n\n");
  }
  else {
    sprintf(mybuffer,"Unable to open file %s.\n",name);
    _error(mybuffer);
  }
}


/*********************************************************************
*
*             _show_directory
*
  This routine demonstrates, how to read a directory.
*/

#if FS_POSIX_DIR_SUPPORT

static void _show_directory(const char *name) {
  FS_DIR *dirp;
  struct FS_DIRENT *direntp;

  dirp = FS_OpenDir(name);
  if (dirp) {
    do {
      direntp = FS_ReadDir(dirp);
      if (direntp) {
        sprintf(mybuffer,"%s\n",direntp->d_name);
        _log(mybuffer);
      }
    } while (direntp);
    FS_CloseDir(dirp);
	_log("\n");
  }
  else {
    _error("Unable to open directory\n");
  }
}

/*static INT8U _cd_directory(const char *name)
{
    FS_DIR *dirp;
    struct FS_DIRENT *direntp;   
	
}*/
#endif /* FS_POSIX_DIR_SUPPORT */


/*********************************************************************
*
*             _show_free
*
  This routine demonstrates, how to read disk space information.
*/

static void _show_free(const char *device) {
  FS_DISKFREE_T disk_data;
  int x;

  _log("Disk information of ");
  _log(device);
  _log("\n");
  x = FS_IoCtl(device,FS_CMD_GET_DISKFREE,0,(void*) &disk_data);
  if (x==0) {
    sprintf(mybuffer,"total clusters     : %lu\navailable clusters : %lu\nsectors/cluster    : %u\nbytes per sector   : %u\n",
          disk_data.total_clusters, disk_data.avail_clusters, disk_data.sectors_per_cluster, disk_data.bytes_per_sector);
    _log(mybuffer);
  } 
  else {
    _error("Invalid drive specified\n\n");
  }
}

/*********************************************************************
*
*             InitCommands
*
  This routine init shell
*/
void InitCmd(void)
{
    INT8U TmpCnt = MAX_COMMAND_NUM;
    while (TmpCnt--)
    {
        ShellComms[TmpCnt].CommandFunc = NULL;
        ShellComms[TmpCnt].name = NULL;
        ShellComms[TmpCnt].num  = 0;
    }
}

INT8U InitCommands()
{
    InitCmd();
	ShellComms[0].num = 0;
	ShellComms[0].name = "ls";
	ShellComms[0].CommandFunc = lsFunc;

	ShellComms[1].num = 1;
	ShellComms[1].name = "rm";
	ShellComms[1].CommandFunc = rmFunc;

	ShellComms[2].num = 2;
	ShellComms[2].name = "write";
	ShellComms[2].CommandFunc = writeFunc;

	ShellComms[3].num = 3;
	ShellComms[3].name = "cat";
	ShellComms[3].CommandFunc = catFunc;

	ShellComms[4].num = 4;
	ShellComms[4].name = "format";
	ShellComms[4].CommandFunc = formatFunc;

	ShellComms[5].num = 5;
	ShellComms[5].name = "mkdir";
	ShellComms[5].CommandFunc = mkdirFunc;

	ShellComms[6].num = 6;
	ShellComms[6].name = "rmdir";
	ShellComms[6].CommandFunc = rmdirFunc;

	ShellComms[7].num = 7;
	ShellComms[7].name = "free";
	ShellComms[7].CommandFunc = freeFunc;

	ShellComms[8].num = 8;
	ShellComms[8].name = "help";
	ShellComms[8].CommandFunc = helpFunc;

	ShellComms[9].num = 9;
	ShellComms[9].name = "cd";
	ShellComms[9].CommandFunc = cdFunc;

	ShellComms[10].num = 10;
	ShellComms[10].name = "pwd";
	ShellComms[10].CommandFunc = pwdFunc;


    ShellComms[11].num = 11;
	ShellComms[11].name = "exit";
	ShellComms[11].CommandFunc = ExitFunc;
        
    ShellComms[12].num = 12;
	ShellComms[12].name = "show";
	ShellComms[12].CommandFunc = ShowFunc;

	return 0;
}

INT8U ShowFunc(INT8U argc,char **argv)
{

    _log("\n=========  TargetVal =  ");
	_log(TargetVal);
	_log("========\n");
    return 0;
}

INT8U ExitFunc(INT8U argc,char **argv)
{
    exit(0);    
    return 0;
}


INT8U pwdFunc(INT8U argc,char **argv)
{
	if (argc != 0)
	{
		_error("PWD is single arg\n");
		return 1;
	}
	_log(CurDir);
	_log("\n");
	return 0;
}
//���ܣ����óɸ�Ŀ¼
void RewindRoot(void)
{
    CurDir[0] = '\\';
    CurDir[1] = '\0';
}


//���ĵ�ǰĿ¼����
INT8U cdFunc(INT8U argc,char **argv)
{
	INT8U DirCurPos = 0;
    INT8U CmdLen    = 0;
	INT8U TmpVal    = 0;
    INT8U IsRoot    = 0;
    INT8U IsBackup  = 0;
    INT8U BackupLen = 0;
	DirCurPos = strlen(CurDir);   //ȡ�õ�ǰĿ¼���ַ�����
    if (argc != 1)
	{
		_error("please input command as:cd directoryname ");
        _error("\n cd \\ change to root\n cd .. \n cd \\MyDir\\Dir1\n");
		_error("\n\n");
		return 1;
	}
	if (strcmp(argv[0], "..") == 0)  //�ص���һ��
	{
		if (1 == DirCurPos)         //�趨����Ϊ1��Ϊ��Ŀ¼
		{
#if EN_DEBUG == 1
			_error("This is root dir\n");
#endif
			return 1;
		} else {
			TmpVal = DirCurPos;   //ȡ�õ�ǰĿ¼�ĳ��Ȳ������ʼ����\
		   //�����ʼ���ҽ��ü�Ŀ¼��� 
           //����\my\dir1  ��1��ʼ�ҵ�r����ҵ�\dir��\���
            while (TmpVal--) 
		   {
			   if (CurDir[TmpVal] == '\\')   //�Ƿ��ҵ���һ��Ŀ¼
			   {
				   if (TmpVal == 0)         //��Ŀ¼
				   {
					   RewindRoot();
				   } else {
				      CurDir[TmpVal] = '\0';   //������\���ɽ�����
				   }
#if EN_DEBUG  == 1
                   _log("\n CurDir Len = %d\n", strlen(CurDir));
#endif
				   break;    //�˳�ѭ��
               } else {
                   CurDir[TmpVal] = '\0';  //����ַ�
               }
		   }
		}
    } else	if (strcmp(argv[0], "\\") == 0)	{   //
		RewindRoot();   //�ص���Ŀ¼
	} else {
	    if (strcmp(CurDir, "\\") == 0)   //�ȽϷ������ڱȽ��Ƿ��ǵ�ǰĿ¼
		{
            IsRoot = 1;
            if ('\\' == argv[0][0])   //�������������Ǵ�\��ʼ�ģ���cd \dir\dir1
            {
                DirCurPos = 0;   
            }
#if EN_DEBUG == 1
            _log("\ncd start at root! DirCurPos = %d\n", DirCurPos);
#endif
		} else {
            if ('\\' == argv[0][0])   //�������������Ǵ�\��ʼ�ģ���cd \dir\dir1
            {
                //��Ϊ��ǰĿ¼���Ǹ�Ŀ¼������Ϊ�˷�ֹ������ԭĿ¼
                IsBackup  = 1;
                BackupLen = DirCurPos + 1;
                FS__CLIB_strncpy(&TargetVal[0], &CurDir[0], BackupLen);
                DirCurPos = 0;     //��ͷ��ʼ��������
            } else {
		        CurDir[DirCurPos] = '\\';
			    DirCurPos++;
            }
		}
        CmdLen    = strlen(argv[0]);
        FS__CLIB_strncpy(&CurDir[DirCurPos], argv[0], CmdLen + 1);  //ȷ���ַ�����'\0'�����뿴FS__CLIB_strncpyԴ��
#if EN_DEBUG == 1
        _log("\nafter FS__CLIB_strncpy() CurDir= %s  DirCurPos = %d CmdLen = %d\n", CurDir, DirCurPos, CmdLen);
#endif
        if (DirExist(CurDir) == 0)  //Ŀ¼������
        {
			_log(argv[0]);
            _log("  Dir is not exist!\n", );
            if (1 == IsRoot)   //�Ƿ��Ǹ�Ŀ¼
            {
                RewindRoot();               
            } else {
                if (1 == IsBackup)
                {
                    FS__CLIB_strncpy(&CurDir[0], &TargetVal[0], BackupLen);
                } else {
                    CurDir[DirCurPos - 1] = '\0';   //�������Ŀ¼���
                }
            }
        } else {
#if EN_DEBUG == 1
            _log("Open %s Exist!\n", CurDir);  //Ŀ¼����
#endif
        }
	}
#if EN_DEBUG == 1
    _log("\nCurDir = %s \n", CurDir);
#endif
	return 0;
}

INT8U lsFunc(INT8U argc,char **argv)
{			

	if(argc == 0)
    {
        _show_directory(CurDir);//_show_directory("");
    } else {
        _show_directory(argv[0]);
    }
	return 0;
}
		

INT8U rmFunc(INT8U argc,char **argv)
{

	if(argc != 1)
	{
		_error("please input command as:rm filename");
		_error("\n\n");
		return 1;
	}
    CreateTargetVal(argv[0]);
	//if(!FS_Remove(argv[0]))
    if(!FS_Remove(TargetVal))
	{
		_log("remove ");
		_log(argv[0]);
		_log(" successful\n\n");
	}
	else
	{
		_error("remove failure!\n\n");
	}
	return 0;
}

INT8U writeFunc(INT8U argc,char **argv)
{
	if(argc != 2)
	{
		_error("please input command as:create filename data");
		_error("\n\n");
		return 1;
	}
    CreateTargetVal(argv[0]);

	//_write_file(argv[0], argv[1]);
    _write_file(TargetVal, argv[1]);

	return 0;
}


//���ڽ�arg���������CurDir + arg = TargetParam
void CreateTargetVal(const char *arg)
{
    INT8U CurDirLen = 0;
    //INT8U ArgIsStartRoot = 0;

    CurDirLen = strlen(CurDir);
    if ('\\' == arg[0])
    {
     //   ArgIsStartRoot  = 1;
        FS__CLIB_strncpy(&TargetVal[0], arg, strlen(arg) + 1);
    } else {
        if (1 == CurDirLen)  //�ж��Ƿ�ǰĿ¼
        {
           //if (1 == ArgIsStartRoot)
        //{
        //    FS__CLIB_strncpy(&TargetVal[0], arg, strlen(arg) + 1);
        //} else{
            FS__CLIB_strncpy(&TargetVal[CurDirLen], arg, strlen(arg) + 1);
        //}
    //} else {
      //  if (1 == ArgIsStartRoot)
       // {
       //      FS__CLIB_strncpy(&TargetVal[0], arg, strlen(arg) + 1);
        } else {
            FS__CLIB_strncpy(TargetVal, CurDir, CurDirLen + 1);
            TargetVal[CurDirLen++] = '\\';
#if EN_DEBUG == 1
            _log("\n %s\n", TargetVal);
#endif
            FS__CLIB_strncpy(&TargetVal[CurDirLen], arg, strlen(arg) + 1);
        }
    }
#if EN_DEBUG == 1
    _log("\nTargetVal = %s\n", TargetVal);
#endif
}

INT8U catFunc(INT8U argc,char **argv)
{
    if(argc != 1)
	{
		_error("please input command as:cat filename");
		_error("\n\n");
		return 1;
	}
    CreateTargetVal(argv[0]);
    _dump_file(TargetVal);
	return 0;
}

INT8U formatFunc(INT8U argc,char **argv)
{
	int x;

    /* format your RAM disk */
    x = FS_IoCtl("ram:0:",FS_CMD_FORMAT_MEDIA,FS_MEDIA_RAM_16KB,0);
    if (x!=0) {
      _error("Cannot format RAM disk.\n\n");
	  return 1;
	}
	else{
		_log("format successful\n\n");
		return 0;
	}
}

INT8U mkdirFunc(INT8U argc,char **argv)
{
	if(argc != 1)
	{
		_error("please input command as:mkdir filename");
		_error("\n\n");
		return 1;
	}
    CreateTargetVal(argv[0]);
	//if(!FS_MkDir(argv[0]))
    if(!FS_MkDir(TargetVal))
	{
		_log("make directory ");
		_log(argv[0]);
		_log(" successful\n\n");
	}
	else
	{
		_error("mkdir failure! please check disk full or unformated\n");
	}
	return 0;
}

INT8U rmdirFunc(INT8U argc,char **argv)
{
	if(argc != 1)
	{
		_error("please input command as:mkdir filename");
		_error("\n\n");
		return 1;
	}
    CreateTargetVal(argv[0]);
	//if(!FS_RmDir(argv[0]))
    if(!FS_RmDir(TargetVal))
	{
		_log("remove ");
		_log(argv[0]);
		_log(" successful\n\n");
	}
	else
	{
		_error("remove failure! please check the directory name\n");
	}
	return 0;
}


INT8U freeFunc(INT8U argc,char **argv)
{
	_show_free("");
	_log("\n");
	return 0;
}

INT8U helpFunc(INT8U argc,char **argv)
{
    ShowHelp();
	return 0;
}

void ShowHelp(void)
{
	_log("\nformat - ��ʽ��\nmkdir  - ����Ŀ¼\nwrite  - �����ļ�\nrmdir  - ɾ��Ŀ¼\nls  \
   - ��ʾĿ¼\ncat    - ���ļ�\nrm     - ɾ���ļ�\nfree   - ��ʾ�ռ�\ncd  \
   - ���ĵ�ǰĿ¼\nshow   - ��ʾTargetValֵ\npwd    - ��ʾ��ǰĿ¼\nexit   - �˳�\n");
}
