#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <execinfo.h>
#include <pthread.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#ifndef SYS_gettid
#error "SYS_gettid unavailable on this system"
#endif
#include <time.h>
//#include <iomanip>
//#include <fstream>
//#include <file.h>
//using namespace std;
//using namespace std::chrono;
#define gettid() ((pid_t)syscall(SYS_gettid))
#define CALLSTACK_SIZE 10
#if 0
const char* get_process_name_by_pid(const int pid)
{
    char* name = (char*)calloc(1024,sizeof(char));
    if(name){
        sprintf(name, "/proc/%d/cmdline",pid);
        FILE* f = fopen(name,"r");
        if(f){
            size_t size;
            size = fread(name, sizeof(char), 1024, f);
            if(size>0){
                if('\n'==name[size-1])
                    name[size-1]='\0';
            }
            fclose(f);
        }
    }
    return name;
}

void get_process_name_by_pid(const int pid, char name[1024])
{
    printf("[%d]getpid \n",getpid());
    sprintf(name, "/proc/%d/cmdline",pid);
    FILE* f = fopen(name,"r");
    if(f){
         size_t size;
         size = fread(name, sizeof(char), 1024, f);
         if(size>0){
            if('\n'==name[size-1])
            name[size-1]='\0';
         }
         fclose(f);
        }

    char *stop_token = NULL;
    char *token;

    for (token = strtok_r(name, "/,", &stop_token); token != NULL; token = strtok_r(NULL, "/", &stop_token))
    {
        strcpy(name,token);
    }
    return;
}
#endif
pid_t pid;
void take_stacktrace(){
    char *args[10];
    char arg[20] = "take_stack.sh";
    char comm[64];
    char snap_time[64];
    char podname[128];
    char pname[64];
    char *env[] = { 0 };  /* leave the environment list null */
    int  pid = getpid();
    char pName[1024];
  //  get_process_name_by_pid(pid, pName);
 
    //strcpy(pname, pName);
    sprintf(comm,"%d",pid);
    sprintf(snap_time,"%lu",time(0));
    args[0] = arg;
    args[1] = comm;
    //args[2] = (char*) pname;
    args[2]=0;
    args[3] = 0;
    args[4] = snap_time;
    args[5] = 0;
    printf("[%d]takestack signal received \n",getpid());
    pid_t ret = vfork();
    if(ret==0){       
        execve("/wa/take_stack.sh",args,env);
        printf("[%d]exec \n",getpid());
        //sleep
    }else if(ret != -1){
        kill(pid,SIGSTOP);
        int status=0;       
        waitpid(ret,&status,0);       
    }
    printf("[%d]return \n",getpid());
}
FILE *fp;
static void print_stack(void) {
    int i, nptrs;
    void *buf[CALLSTACK_SIZE + 1];
    char **strings;
    char cBufMessage[512];
    int fd;
    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

   // std::stringstream ss;
   // auto now = std::chrono::system_clock::now();

    // Convert to local time
    //auto localTime = std::chrono::system_clock::to_time_t(now);
    
    // Format the timestamp as a strin
   // ss << std::put_time(std::localtime(&localTime), "%F_%H-%M-%S") << extension;
    
    nptrs = backtrace(buf, CALLSTACK_SIZE);
    printf("%s: backtrace() returned %d addresses\n", __func__, nptrs);

    strings = backtrace_symbols(buf, nptrs);

    if(strings == NULL) {
        printf("%s: no backtrace captured\n", __func__);
        return;
    }
    //const char path = "/wa/BT"+ asctime (timeinfo) +".txt";
    char filename[40];
    struct tm *timenow;

    time_t now = time(NULL);
    timenow = gmtime(&now);

    strftime(filename, sizeof(filename), "/wa/BT_%Y-%m-%d_%H:%M:%S", timenow);

    fp = fopen(filename, "a");

     //   ulBufLength =
       //     (ULONG)snpritf((char *)cBufMessage,
       //       512, P3("\n: 0x%x Length:%u \n", (ULONG *)buf, 11));
    //(void)fwrite(buf, 1, 512, fp);
    fd = (unsigned long)fileno(fp);
    fseek(fp, 0, SEEK_END);
    backtrace_symbols_fd(buf, nptrs, fd);

    for(i = 0; i < nptrs; i++) {
        printf("%s\n", strings[i]);
    }

    free(strings);
}

pid_t pid;
void sigHandler(int signo)
{
  printf("[%d]signal received,signo %d \n",getpid(), signo);
    char *args[10];
    char arg[20] = "take_stack.sh";
    char comm[64];
    char snap_time[64];
    char podname[128];
    char pname[64];
    char *env[] = { 0 };  /* leave the environment list null */
    int  pid = getpid();
    char pName[1024];
    //get_process_name_by_pid(pid, pName);

    //strcpy(pname, pName);
    sprintf(comm,"%d",pid);
    sprintf(snap_time,"%lu",time(0));
    args[0] = arg;
    args[1] = comm;
   // args[2] = (char*) pname;
   args[2] = 0;
    args[3] = 0;
    args[4] = snap_time;
    args[5] = 0;
    printf("[%d]takestack signal received \n",getpid());

    signal(SIGABRT,SIG_DFL);
    signal(SIGTERM,SIG_DFL);
    signal(SIGSEGV,SIG_DFL);
    signal(SIGKILL,SIG_DFL);

#if 1
  pid = vfork();
  if(pid  ==  0)
  {
    printf("[%d] I m child process \n",getpid());
   // signal(SIGHUP,sigHandler);
    //execl("./chainNew", "chainNew", (char *)0);
//   execve("/wa/take_stack.sh",args,env);
  }
  else if(pid > 0)
  {
    printf("[%d] I m Parent process \n",getpid());
    sleep(60*10);
    exit(0);
  }
  else
  {
    printf("[%d]  Fork Failed \n",getpid());
  }
#endif
  //take_stacktrace();
  signal(SIGABRT,SIG_DFL);
  printf("[%d]before abort \n",getpid());
  print_stack();
  abort();
  return;
}
int SigCatch(int aSig)
{
  printf("[%d]sigcatch \n",getpid());
    struct sigaction sigact;
    sigact.sa_handler = sigHandler;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    sigaction(aSig, &sigact, (struct sigaction *)NULL);

    return 1;
}

int main ()
{
 // signal(SIGHUP,sigHandler);
 // signal(SIGTERM,sigHandler);
  //signal(SIGKILL,sigHandler);
//SIGABRT
//SIGSEGV
//SIGINT
  SigCatch(SIGTERM);
  SigCatch(SIGKILL);
  SigCatch(SIGABRT);
  SigCatch(SIGSEGV);
  SigCatch(SIGINT);
  /*sig child ignoring*/
  signal (SIGCHLD, SIG_IGN);

#if 0
 //generate segmentation fault
  char x[5];
  int i = 1;
  for(i; i<=5; i++)
  {
    printf("%s",x[i]);
  }
  #endif

#if 1  
  while (1)
  {
    printf("[%d] Waiting for Signal, tid %d \n",getpid(), gettid());
    sleep(2);
  }
#endif
}
