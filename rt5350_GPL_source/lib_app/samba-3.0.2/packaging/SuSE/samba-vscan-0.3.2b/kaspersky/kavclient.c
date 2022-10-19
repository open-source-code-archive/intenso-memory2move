/* 2003.02.15 kjw@rightsock.com - patched several memory and file descriptor leaks */

#include"shmem.h"
#include "helpers.h"
#include"kavclient.h"

#include <includes.h>

#ifndef SHUT_RDWR
#define SHUT_RDWR 2
#endif


//-------------------------------helpers----------------------------------------
#ifndef offsetof
#define offsetof(type, member) ( (int) & ((type*)0) -> member )
#endif

#define TIMEOUT 15   //timeout value in seconds

#define bool unsigned long

#define MAX_PATH_LEN 1024
/*getDate() - returns a pointer to 16-char string(16th is 0),
  representing current date and time*/
char* getDate(void);

/*arr2str() - writes strings from src array and puts them
 in one string, divided by "term" symbol*/
char* arr2str(char** src,char term);

/*confirm() - provides confirmation info for KAVResponse*/
char* confirm(void);

/*paths2key() - converts an array of target paths in char* representation
  into a key o{path1:path2:...:pathN}*/
char* paths2key(char** targets);
//------------------------------------------------------------------------------

/*-------------------------------global variables-----------------------------*/
MemForUse *ShMem;
key_t shm_key;
extern int errno;
/*----------------------------------------------------------------------------*/

int KAVConnect(char *KAVDaemonPath, char flags)
{
  int kav_socket;
  struct sockaddr_un kav_sockaddr;
  int size;
  
  if(KAVDaemonPath==0)
  {
    /*if no path specified, bailing out*/
    DEBUG(0, ("vscan-kavp: KAVConnect: no path to KAV daemon specified [-1]\n"));
    return -1;
  }
  
  /*create socket*/
  if((kav_socket=socket(AF_UNIX,SOCK_STREAM,0))<0)
  {
    /*if socket creation failed bailing out bailing out*/
    DEBUG(0, ("vscan-kavp: KAVConnect: Error creating socket[-2]\n"));
    return -2;
  }
  
  kav_sockaddr.sun_family=AF_UNIX;
  fstrcpy(kav_sockaddr.sun_path, KAVDaemonPath);
  /*calculates kav_sockaddr size*/
  size = offsetof(struct sockaddr_un,sun_path)+strlen(kav_sockaddr.sun_path)+1;
  if(connect(kav_socket,(struct sockaddr*)&kav_sockaddr,size)<0)
  {
    /*if connect failed(e.g. wrong path was given) bailing out*/
    DEBUG(0, ("vscan-kavp: KAVConnect: Error creating socket, wrong path [-3]\n"));
    return -3;
  }
  
  return kav_socket;
}

char* KAVVersion(int kav_socket, char flags)
{
//sending enquiry
  int section1;
  unsigned long section2;
  char* enquiry;
  char* result=NULL;
  char* date=getDate();
  int enqlength=strlen("<4>:") + strlen(date)+1;
  enquiry=(char*)malloc(enqlength);
  if(enquiry==NULL)
  {
    DEBUG(0, ("vscan-kavp: KAVVersion: malloc for enquiry failed [0]\n"));
    free(date);
    return 0;
  }
  
  slprintf(enquiry, enqlength, "<4>%s:", date);/*building enquiry*/
  /*write enquiry to KAVDaemon*/
  if(write(kav_socket,enquiry,strlen(enquiry))<0)
  {
    DEBUG(0, ("vscan-kavp: KAVVersion: write() enquiry to socket failed [0]\n"));
    free(date);
    free(enquiry);
    return 0;
  }
  
  free(date);
  free(enquiry);
//readind&analyzing response
  if(timeoutread(TIMEOUT,kav_socket,(char*)&section1,2)<2)		/*read first 2 bytes*/
  {
    DEBUG(0, ("vscan-kavp: KAVVersion: read from socket failed (1) [0]\n"));
    return 0;
  }

  if(timeoutread(TIMEOUT,kav_socket,(char*)&section2,sizeof(unsigned long))<0)
  {
    DEBUG(0, ("vscan-kavp: KAVVersion: read from socket failed (2) [0]\n"));
    return 0;
  }

  if(section2!=0)
  {
    result=(char*)malloc(section2+1);
    if(result==NULL)
    {
      DEBUG(0, ("vscan-kavp: memory allocation for account [0]\n"));
      return 0;
    }
    
    result[0]=0;
    if(timeoutread(TIMEOUT,kav_socket,result,section2)<0)
    {
	DEBUG(0, ("vscan-kavp: KAVVersion: read from socket failed (3) [0]\n"));
        free(result);
        return 0;
    }

    result[section2]=0;
  }

  return result;
}

int KAVRequestShmem(int kav_socket, char* path, char* keyname,char chSHM, char flags)
{
  char* date;
  struct stat fst;
  int ppath;

  key_t key;
  int shmid;
  unsigned long size;
  char* enquiry;
  int enqlength;
  
  /*check that path is a regular file*/
  if(isadir(path,flags)!=0)
  {
	DEBUG(0, ("vscan-kavp: KAVRequestShmem: Error requesting scan \
	    for directory using shared memory [-4]\n"));
        return -4;
  }

  /*get file statistics*/
  if(lstat(path,&fst)<0)
  {
    DEBUG(0, ("vscan-kavp: KAVRequestShmem: Error lstat for %s [-5]\n", path));
    return -5;
  }
  
  size=fst.st_size;
  /*open file*/
  if((ppath=open(path,O_RDONLY))<0)
  {
    DEBUG(0, ("vscan-kavp: KAVRequestShmem: Error: opening file for copy to shmem [-6]\n"));
    return -6;
  }

  /*set file iterator*/
  lseek(ppath,0,SEEK_SET);
  /*check keyname file accessibility*/
  if(access(keyname,0)<0)
  {
    DEBUG(0, ("vscan-kavp: KAVRequestShmem: Cannot access keyname of shmem [-7]\n"));
    close(ppath);
    return -7;
  }

  if((key=ftok(keyname,chSHM))<0)	/*generate a key*/
  {
    DEBUG(0, ("vscan-kavp: KAVRequestShmem: Cannot get key [-8]\n"));
    close(ppath);
    return -8;
  }

  /*get shared memory*/
  if((shmid=shmget(key,(sizeof(MemForUse)+size+(MemAlignment-1)) & ~(MemAlignment-1),PERM|IPC_CREAT))<0)
  {
    DEBUG(0, ("vscan-kavp: KAVRequestShmem: Cannot get shmem by key [-9]\n"));
    close(ppath);
    return -9;
  }
  if((ShMem=(MemForUse*)shmat(shmid,0,0))<0)	/*attach shared memory*/
  {
    DEBUG(0, ("vscan-kavp: KAVRequestShmem: Cannot attach shared memory [-10]\n"));
    if(shmctl(shmid,IPC_RMID,NULL)<0)
	DEBUG(0, ("vscan-kavp: KAVRequestShmem: Enable to delete shared memory [-10]\n"));
    close(ppath);
    return -10;
  }
  ShMem->params.ShMSize=size;	/*set shmem size*/
  if(read(ppath,ShMem->buf,size)<0)		/*copy file to a shared memory*/
  {
    if(shmdt((char*)ShMem)<0)
      DEBUG(0, ("vscan-kavp: KAVRequestShmem: Unable to detach shared memory [-11]\n"));
    if(shmctl(shmid,IPC_RMID,NULL)<0)
      DEBUG(0, ("vscan-kavp: KAVRequestShmem: Unable to delete shqared memory [-11]\n"));
    close(ppath);
    return -11;
  }
  close(ppath);					/*close file*/

  ShMem->params.ShMKey=key;
  shm_key=key;
  if(shmdt((char*)ShMem)<0)				/*detach shmem*/
  {
    DEBUG(0, ("vscan-kavp: KAVRequestShmem: Error detaching shared memory [-12]\n"));
    return -12;
  }
  /*lets get local time*/
  date=getDate();
  enqlength = strlen("<3>:<||>")+strlen(date)+sizeof(key)+sizeof(size)+1;
  enquiry=(char*)malloc(enqlength);
  if(enquiry==NULL)
  {
    DEBUG(0, ("vscan-kavp: KAVRequestShmem: Malloc failed for enquiry [-13]\n"));
    free(date);
    return -13;
  }

  slprintf(enquiry, enqlength, "<3>%s:<%x|%lx|>",date,key,size);/*building enquiry*/
  free(date);

  /*write enquiry to KAVDaemon*/
  if(write(kav_socket,enquiry,strlen(enquiry))<0)
  {
    DEBUG(0, ("vscan-kavp: KAVRequestShmem: write() failed for enquiry to socket [-14]\n"));
    free(enquiry);
    return -14;
  }

  free(enquiry);
  return 0;
}

int KAVRequestPath(int kav_socket,char* path,char flags)
{
  char* date;
  char* enquiry;
  int enqlength;

  if(isadir(path,flags)<0)/*check that path is a regular file or a dir*/
  {
    DEBUG(0, ("vscan-kavp: KAVRequestPath: given paths is neither a file nor a director [-15]\n"));
    return -15;
  }

/*first lets get local time*/
  date=getDate();			/*get current date*/
  enqlength=strlen("<0>:")+strlen(date)+strlen(path)+1;
  enquiry=(char*)malloc(enqlength);
  if(enquiry==NULL)
  {
    DEBUG(0, ("vscan-kavp: KAVRequestPath: Malloc() failed for enquiry [-16]\n"));
    free(date);
    return -16;
  }
  slprintf(enquiry, enqlength, "<0>%s:%s", date, path);/*enquiry is constructed*/
  free(date);

  /*write enquiry to KAVDaemon*/
  if(write(kav_socket,enquiry,strlen(enquiry))<0)
  {
    DEBUG(0, ("vscan-kavp: KAVRequestPath: write() failed for enquiry [-17]\n"));
    free(enquiry);
    return -17;
  }
  free(enquiry);
  return 0;
}

int KAVRequestMulti(int kav_socket, char** keys,char** paths,char flags)
{
  char* enquiry;
  int enqlength;
  char* skeys;
  char* spaths;
  char* date;
  
  /*lets get local time*/
  date=getDate();
  /*keys->key1|key2|...|keyN*/
  skeys=arr2str(keys,'|');
  /*paths->path1;path2;...pathN*/
  spaths=arr2str(paths,';');
  enqlength = strlen("<0>:\xfe\xfe")+strlen(date)+strlen(skeys)+strlen(spaths)+3;
  enquiry=(char*)malloc(enqlength);

  if(enquiry==NULL)
  {
     DEBUG(0, ("vscan-kavp: KAVRequestPath: Malloc() failed for enquiry [-18]\n"));
     free(date);
     free(skeys);
     free(spaths);
     return -18;
  }

  slprintf(enquiry, enqlength, "<0>%s:\xfe",date);
  free(date);

  if(skeys[0]!=0)
    fstrcat(enquiry,skeys);
  fstrcat(enquiry,"|\xfe");

  if(spaths[0]!=0)
  {
    fstrcat(enquiry,spaths);
  }

  if(write(kav_socket,enquiry,strlen(enquiry))<0)
  {
    DEBUG(0, ("vscan-kavp: KAVRequestMulti: write() failed for enquiry [-19]\n"));
    free(skeys);
    free(spaths);
    free(enquiry);
    return -19;
  }

  free(skeys);
  free(spaths);
  free(enquiry);
  return 0;
}

char* KAVResponse(int kav_socket,int* exit_code,char flags,char* filename)
{
  int section1;
  int pfile;
  unsigned long int section2;
  unsigned long int size;
  char* conf=NULL;
  char hi,lo;
  int shmid;
  int res;
  char* account=NULL,*acc_buffer=NULL;

waitrez:
  if(timeoutread(TIMEOUT,kav_socket,(char*)&section1,2)<2)		/*read first 2 bytes*/
  {
    DEBUG(0, ("vscan-kavp: KAVResponse: read from socket failed (1) [0]\n"));
    return 0;
  }
  lo=(char)(section1&0xff)-0x30;
  hi=(char)(section1>>8);

  if(hi==0x1)
  /*read length of an account buffer and then read the buffer*/
  {
    if(timeoutread(TIMEOUT,kav_socket,(char*)&section2,sizeof(unsigned long))<0)
    {
      DEBUG(0, ("vscan-kavp: KAVResponse: read from socket failed (2) [0]\n"));
      return 0;
    }

    if(section2!=0)
    {
      acc_buffer=(char*)malloc(section2+1);
      if(acc_buffer==NULL)
      {
	DEBUG(0, ("vscan-kavp: KAVResponse: memory reallocation for acc_buffer [1]\n"));
        return 0;
      }
      acc_buffer[0]=0;
      account=acc_buffer;
      /* use account as an incrementing pointer to read all parts of section2 */
      while((section2>0)&&((res=timeoutread(TIMEOUT,kav_socket,account,section2))!=0))
      {
        if(res<0)
        {
	  DEBUG(0, ("vscan-kavp: KAVResponse: read from socket failed (3) [0]\n"));
          free(account);
          return 0;
        }
        section2-=res;
        account[res]=0;
        account+=res;
      }
    }
    else
      DEBUG(0, ("vscan-kavp: KAVResponse: Zero-size account received from daemon\n"));
  }
  else /* (hi!=0x1) */
  {
    acc_buffer=(char*)malloc(strlen("no info received")+1);
    if(acc_buffer==NULL)
    {
	DEBUG(0, ("vscan-kavp: KAVResponse: memory reallocation for acc_buffer [0]\n"));
      return 0;
    }
    fstrcpy(acc_buffer, "no info received");
  }

  /*switch by 1st(lower order) byte*/
  switch(lo)
  {
    case 0:			/*No viruses were found*/
      DEBUG(2, ("vscan-kavp: KAVResponse: Test result: No viruses were found\n"));
      break;
    case 1:			/*Virus scan was not complete*/
      DEBUG(1, ("vscan-kavp: KAVResponse: Virus scan was not complete\n"));
      break;
    case 2:			/*Mutated or corrupted viruses were found*/
      DEBUG(1, ("vscan-kavp: KAVResponse: Test result: Mutated or corrupted viruses were found\n"));
      break;
    case 3:			/*Suspicious objects were found*/
      DEBUG(1, ("vscan-kavp: KAVResponse: Test result: Suspicious objects were found\n"));
      break;
    case 4:			/*Known viruses were detected*/
      DEBUG(1, ("vscan-kavp: KAVResponse: Test result: Known viruses were detected\n"));
      break;
    case 5:			/*All detected viruses have been succesfully cured*/
      DEBUG(1, ("vscan-kavp:  KAVResponse: Test result: All detected viruses have been succesfully cured\n"));
      if(hi==0x2)		/*if cured object is in shared memory*/
      {
        if(filename==0)
        {
    	  DEBUG(0, ("vscan-kavp: KAVResponse: for ShMem error: no filename is specified [0]\n"));
          free(acc_buffer);
          return 0;
        }

        if(timeoutread(TIMEOUT,kav_socket,(char*)&section2,4)<4)
        {
    	  DEBUG(0, ("vscan-kavp: KAVResponse: Failed to read shmem size [0]\n"));
          free(acc_buffer);
          return 0;
        }

        size=section2+sizeof(MemForUse);
        if((shmid=shmget(shm_key,size,0))<0)
        {
    	  DEBUG(0, ("vscan-kavp: KAVResponse: Failed to get shmem [0]\n"));
          free(acc_buffer);
          return 0;
        }
        
	if((ShMem=(MemForUse*)shmat(shmid,0,0))<0)
        {
    	  DEBUG(0, ("vscan-kavp: KAVResponse: Failed to attach shmem [0]\n"));
          if(shmctl(shmid,IPC_RMID,NULL)<0)
    	    DEBUG(0, ("vscan-kavp: KAVResponse: Unable to delete shared memory [0]\n"));
          free(acc_buffer);
          return 0;
        }
	
        if((pfile=open(filename,O_WRONLY))<0)
        {
    	  DEBUG(0, ("vscan-kavp: KAVResponse: Failed to open file for curing [0]\n"));
          if(shmdt((char*)ShMem)<0)
    	    DEBUG(0, ("vscan-kavp: KAVResponse: unable to detach shared memory [0]\n"));
          if(shmctl(shmid,IPC_RMID,NULL)<0)
    	    DEBUG(0, ("vscan-kavp: KAVResponse: unable to delete shared memory [0]\n"));
          free(acc_buffer);
          return 0;
        }
        lseek(pfile,0,SEEK_SET);
        if(write(pfile,ShMem->buf,section2)<0)
        {
    	  DEBUG(0, ("vscan-kavp: KAVResponse: failed to write cured file [0]\n"));
          free(acc_buffer);
          return 0;
        }
        ftruncate(pfile,section2);
        close(pfile);
    	DEBUG(0, ("vscan-kavp: KAVResponse: ... and written from shared memory to disk [0]\n"));
        if(shmdt((char*)ShMem)<0)
    	    DEBUG(0, ("vscan-kavp: KAVResponse: unable to detach shared memory [0]\n"));
        if(shmctl(shmid,IPC_RMID,NULL)<0)
    	    DEBUG(0, ("vscan-kavp: KAVResponse: unable to delete shared memory [0]\n"));
      }
      else
      {
/*        if(verbose)printf("\n"); */
      }
      break;
    case 6:		/*All infected objects have been deleted*/
      DEBUG(0, ("vscan-kavp: KAVResponse: Test result: All infected objects have been deleted\n"));
      break;
    case 7:		/*File KAVDaemon is corrupted*/
      DEBUG(0, ("vscan-kavp: KAVResponse: Test result: File KAVDaemon is corrupted\n"));
      break;
    case 8:		/*Corrupted objects were found*/
      DEBUG(0, ("vscan-kavp: KAVResponse: Test result: Corrupted objects were found\n"));
      break;
    case 0xf:		/*Confirmation required for cure*/
      /*if(verbose) printf("\n");*/
      conf=confirm();
      if(send(kav_socket,conf,1,0)<0)
      {
        DEBUG(0, ("vscan-kavp: KAVResponse: Failed writing dialog result to KAVDaemon\n"));
        break;
      }
      free(conf);
      free(acc_buffer);
      conf=NULL;
      acc_buffer=NULL;
      goto waitrez;
    default:				/*some shit happened*/
      DEBUG(0, ("vscan-kavp: KAVResponse: Incorrect test result returned: %d\n",lo));
      break;
  }
  *exit_code=section1;
  return acc_buffer;
}

int KAVClose(int kav_socket,char flags)
{
  if(shutdown(kav_socket,SHUT_RDWR)<0)
  {/*if shit happened(e.g. wrong socket descriptor was given)*/
   /*bailing out*/
    DEBUG(0, ("vscan-kavp: KAVClose error [-30]\n"));
    return -30;
  }
  return 0;

}
/*----------------------------------HELPERS------------------------------------*/
int isadir(const char* path,char flags)
{
  struct stat fst;
  char tmp[MAX_PATH_LEN];
  if(stat(path,&fst)<0)
  {
    DEBUG(0, ("vscan-kavp: isadir: isadir failed for %s",path));
    return -31;
  }
  if(fst.st_mode&S_IFDIR)
    return 1;
  else if(fst.st_mode&S_IFREG)
    return 0;
  else if(fst.st_mode&S_IFLNK)
  {
    if(readlink(path,tmp,MAX_PATH_LEN)<0)
    {
      DEBUG(0, ("vscan-kavp: isadir: readlink() for %s failed:",path));
      return -32;
    }
    return isadir(tmp,flags);
  }
  else
  {
    DEBUG(0, ("vscan-kavp: isadir: %s is neither a file nor a directory\n",path));
    return -33;
  }
}

char* getDate()
{
  char* date;
  time_t t;
  time(&t);
  date=(char*)malloc(16);
  strncpy(date,ctime(&t)+4,15);
  date[15]=0;
  return date;
}

char* arr2str(char** src,char term)
{
  int size=0;
  int length=0;
  int i;
  char *res=0;
  char term1[2]={term,0};
  if(src==0)
  {
    res=(char*)malloc(1);
    res[0]=0;
    return res;
  }
  while(src[size]!=0)		/*counts space, required for result string*/
    length+=strlen(src[size++]);
  length+=size-1;		/*for "term" characters*/
  res=(char*)malloc(length+1);
  res[0]=0;
  for(i=0;i<size;i++)		/*builds result string*/
  {
    if(i>0)
      fstrcat(res,term1);
    fstrcat(res,src[i]);
  }
  return res;
}

/*
char* confirm()
{
  char* ch=(char*)malloc(1);
  ch[0]=getchar();
  if(ch[0]!='\n') while(!feof(stdin) && (fgetc(stdin)!='\n'));
  return(ch);
}
*/

char* paths2key(char** targets)
{
  char* key=arr2str(targets,':');
  char* res=(char*)malloc(strlen(key)+3+1);
  fstrcpy(res,"o{");
  fstrcat(res,key);
  fstrcat(res,"}");
  return res;
}

