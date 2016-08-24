#include <fstream>         /* std::ifstream */
#include <iostream>         /* std::cout */
#include <chrono>           /*system_clock::now()*/
#include "include/GUTimer.h"        /*Timers: time_h, rdtsc, chrono_hr, ctime, LOFAR_timer*/
#include <unistd.h>
#include <sys/stat.h>
#include<csignal>       /*kill()*/
//=============
//=============

#include <sys/wait.h>


void tsdb_stdout(std::ofstream& outfile,std::string metric="exe.0.null",std::string data=""){
using namespace std::chrono;
milliseconds ms = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
outfile << "put "+metric+" "+std::to_string(ms.count())+" "+data+"\n";
return;
}

void
getio(const std::string& path,std::ofstream& tsdbfile,std::string metric)
{

  std::ifstream file(path);

  std::string readchars,dummy,wrchars,syscw,syscr,read_bytes,write_bytes;
  file>>dummy>> readchars>>dummy>>wrchars>>dummy>>syscr>>dummy>>syscw>>dummy>>read_bytes>>dummy>>write_bytes;
  metric+=".io";
  tsdb_stdout(tsdbfile,metric+".rchar",readchars);
  tsdb_stdout(tsdbfile,metric+".wchar",wrchars);
  tsdb_stdout(tsdbfile,metric+".syscr",syscr);
  tsdb_stdout(tsdbfile,metric+".syscw",syscw);
  tsdb_stdout(tsdbfile,metric+".read_b",read_bytes);
  tsdb_stdout(tsdbfile,metric+".write_b",write_bytes);

  return;
}

void
getmem(const std::string& pid,std::ofstream& tsdbfile,std::string metric)//Get memory information (in one block right now)
{
  metric+=".mem";
  std::string path="/proc/"+pid+"/statm";
  std::ifstream file(path,std::ifstream::binary);
  std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  size_t pos = 0;
  std::string token;
  std::string delim=" ";
  std::string metrics[]={"VmSize","VMRSS","shr-pgs","code","NA","Data_and_stack"};
  int met_pos=0;
  while ((pos = content.find(delim)) != std::string::npos) {
    token = content.substr(0, pos);
    content.erase(0, pos + delim.length());
    tsdb_stdout(tsdbfile,metric+"."+metrics[met_pos],token);
    met_pos+=1;
}
    return ;
}

void
getstat(const std::string& pid,std::ofstream& tsdbfile,std::string metric)//Get memmemory information (in one block right now)
{

    std::string dummy,minflt,mjflt,utime,s_time,nthreads,vsize,rss,iodelay;
    char state;
    std::string path="/proc/"+pid+"/stat";
    std::ifstream file(path,std::ifstream::binary);
    file>>dummy>>dummy>>state>>dummy>>dummy>>dummy>>dummy>>dummy>>dummy>>minflt>>dummy>>mjflt>>dummy>>utime>>s_time>>dummy>>dummy>>dummy>>dummy>>nthreads>>dummy>>dummy>>vsize>>rss;
    metric+=".stat";
    std::string st_int;
    switch(state){
        case 'R':
        st_int="1";break;
        case 'S':
        st_int="2"; break;
        case 'D':
        st_int="3"; break;
        case 'T':
        st_int="4"; break;
        default:
        st_int="0";

    }
    tsdb_stdout(tsdbfile,metric+".state",st_int);
    tsdb_stdout(tsdbfile,metric+".minflt",minflt);
    tsdb_stdout(tsdbfile,metric+".mjrflt",mjflt);
    tsdb_stdout(tsdbfile,metric+".utime",utime);
    tsdb_stdout(tsdbfile,metric+".stime",s_time);
    tsdb_stdout(tsdbfile,metric+".nthreads",nthreads);
    return;
}

void
getPiD(std::string& str_pid, std::string& str_pname)
{

 if(str_pid=="")
  {
    while(str_pid=="")
    {
      FILE* fpidof = popen(("pidof "+str_pname).c_str(),"r");
      if (fpidof)
      {
        int p=0;
        if (fscanf(fpidof, "%d", &p)>0 && p>0)
        str_pid = std::to_string(p);
        pclose(fpidof);
      }
    }
  }
return;
}

void handle_sigchld(int sig) {
  while (waitpid((pid_t)(-1), 0, WNOHANG) > 0) {}
}


int
main(int argc, char *argv[]) {
 std::string str_pid,configfile="",str_pname="",delays,str_command;
 int delay=1000000;
 if ( argc < 2 ){
    std::cout<<"usage: "<< argv[0] <<" PID configfile \n";
    return 0;
    }
 else
  { if(isdigit(argv[1][0])) //Process ID number
    {
      str_pid= argv[1];
    }
    else                    //Either *.cfg file or process launch (NOT implemented)
    {
     configfile=argv[1];

     if (configfile.find(".cfg")!=std::string::npos) //if a configure file is specified
      {
       std::ifstream infile(configfile);
       if (infile.good())
        {
         getline(infile,str_pname);
         getline(infile,delays);
        }
        if (delays=="")
        {
         delay=1000000;
        }
        else
        {
         delay=1000*std::stoi(delays);
        }
       infile.close();
       // Check if process name exists and get PID
       getPiD(str_pid,str_pname);
      }

      else if(argv[1]!=""){  //launch process from here
      std::cout<<"Launching process "<<configfile<<std::endl;
      std::flush(std::cout);
      int pid = fork();
      if (pid==-1){std::cout<<"Fork Failed somehow!!";}
      if(pid==0)
        {
        std::cout<<"Launching process"<<'\n';
        std::cout<<argv[1]<<" "<<argc<<'\n';

        int rc = execvp(argv[1] ,&argv[1]);

        std::cout<<"Output of fork is "<<rc<<std::endl;
        if (rc==-1) std::cout<<"Error launching process "<<configfile<<" Did you have the path right?"<<std::endl;
        }
      else  //parent process
        {
           std::cout<<"Parent Process";
           std::string str_name=argv[1];//extract name from path
           // Check if process name exists and get PID
           getPiD(str_pid,str_name);
        }
      }
      else //otherwise the process name to track
      {
        str_pname=argv[1];
        std::cout<<str_pname;
        std::flush(std::cout);
      }
    }
  }

  std::cout<<"$proc "<<str_pid<<" "<<str_pname<< " from "<<configfile<<" with delay (ms) "<<delay/1000<<std::endl;

  std::ifstream filec("/proc/"+str_pid+"/cmdline");
  filec>>str_command;
  std::cout<<"$proc cmd: "<<str_command<<std::endl;

  std::cout<<"$proc-m VmSize(pg), VMRSS (pg), shared-pages, code, 0,  data+stack, 0\n";
  std::cout<<"$proc-i rchar, wchar, syscr, syscw, read_bytes, write_bytes\n";
  std::cout<<"$proc-s state, minflt, mjflt, utime ,s_time, nthreads, VMSize, RSS(pages)'\n';";
// rusage ru;
 time_t t = time(0);
 struct tm * now = localtime( & t );
 std::cout <<"$proc-Start time: "<< (now->tm_hour)<<":"<<(now->tm_min)<<":"<<(now->tm_sec) << std::endl;
 std::string command_metric;
// command_metric<<str_command;
 std::string metric="exe."+str_pid+"."+argv[1];
 std::ofstream tsdbfile;

 tsdbfile.open ("tcollector_proc.out");
 //while(exists("/proc/"+str_pid+"/exe"))//main loop, executes while the process is running (maybe faster way?)
 while(not(kill(std::stoi(str_pid),NULL))) //continues if pid exists, not sure if works fasters
 {
    getio("/proc/"+str_pid+"/io",tsdbfile,metric);
    getmem(str_pid,tsdbfile,metric);
    getstat(str_pid,tsdbfile,metric);
    signal(SIGCHLD, handle_sigchld ); //Handles the death of the child (with grief)
    usleep(delay);
 }
 tsdbfile.close();
return 0;
}


