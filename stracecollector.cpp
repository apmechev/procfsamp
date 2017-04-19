#include <fstream>         /* std::ifstream */
#include <iostream>         /* std::cout */
#include <string.h>            /*strcat*/
#include <chrono>           /*system_clock::now()*/
//#include "include/GUTimer.h"        /*Timers: time_h, rdtsc, chrono_hr, ctime, LOFAR_timer*/
#include <unistd.h>
//#include <sys/stat.h>
#include <csignal>       /*kill()*/
#include <algorithm>    /*remove_if*/
#include <libgen.h>     /*basename()*/
#include <getopt.h>	/*option*/
#include <map>	/*vector for execvp*/
#include <array>
#include <memory>

//=============
//=============

#include <sys/wait.h>


void
tsdb_stdout(std::fstream& outfile,std::string metric="exe.0.null",std::string data=""){
  //Writes metric, stamp and data to a file
  //  TODO: Make this also sendable through std::out for tcollector
  using namespace std::chrono;
  std::chrono::milliseconds ms = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
  outfile << "put "+metric+" "+std::to_string(ms.count())+" "+data+"\n";
  std::cout<<metric+" "+std::to_string(ms.count())+" "+data+"\n";
  std::flush(std::cout);
return;
}


void incr_dict(std::map<std::string,int> &dict, std::string call_name){
 if (dict.find(call_name)!=dict.end())
  {dict[call_name]++;}
 else 
  {dict.insert(std::pair<std::string,int>(call_name,1));}
 return;
}

std::string exec(const char *cmd, std::map<std::string,int> &dict) {
    std::array<char, 256> buffer;
    std::string result, tmp;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);//strace -p 8598
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 256, pipe.get()) != NULL) 
            tmp = buffer.data();
            if (tmp.find("not permitted")!=std::string::npos){throw 91;} //ignores attach and detach tags
            incr_dict(dict , tmp.substr(0, tmp.find("(", 0)));
            result+= tmp.substr(0, tmp.find("(", 0))+"\n";
    }
    return result;
}


void
getPiD(std::string& str_pid, std::string& str_pname)
{
 /*Use pidof to find the Process ID of str_pname, puts in addres of str_pid*/
 if(str_pid=="")
  {
    while(str_pid=="")
    {
      FILE* fpidof = popen(("pgrep -u `id -u` "+str_pname).c_str(),"r"); //Replace pidof with pgrep to get only current user //ok done, u happy?
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


int launch_from_config(std::string configfile,std::string& delays,int& delay, std::string& str_pname, std::string& str_pid){
	/*Taking this out of the main, but basically launches from the config file and updates variables*/
       std::cerr<<"Launching from cfg file"<<std::endl;
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
         delay=1000*std::stoi(std::string(delays));
        }
       infile.close();
       // Check if process name exists and get PID
       getPiD(str_pid,str_pname); //this needs fixing
//       configfile=str_pname;   //This shold probably be removed
       //
    return 0;
}

void strace_loop(std::string command, std::map<std::string,int> &dict, std::string metric){
  exec(command.c_str(),dict);
  for(std::map<std::string,int>::iterator iter = dict.begin(); iter != dict.end(); ++iter)
  {

    std::string k=iter->first;
    if (k.find("tached")!=std::string::npos){continue;} //ignores attach and detach tags
    if (k.find("SIGCHLD")!=std::string::npos){continue;} //ignores attach and detach tags
    int v = iter->second;
    using namespace std::chrono;
    std::chrono::milliseconds ms = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
    std::cout<<metric<<"."<<k<<" "<<std::to_string(ms.count())<<" "<<v<<std::endl;
    iter->second=0;
   }
  return; 
}

int
main(int argc, char *argv[]) {
 std::string str_pid,configfile="",str_launch="",str_pname="",delays,str_command,metric;
 int run_delay=1,delay=0; //delay controls looping (each second, 10 sec, etc)
 while(1)                 //whereas run_delay is how long to trace for
 {
  static struct option long_options []=
   {
    {"pid",   required_argument,    0,   'p'},
    {"pname", required_argument,    0,   'n'},
    {"launch",required_argument,    0,   'l'},
    {"metric",required_argument,    0,   'm'},
    {"delay", required_argument,    0,   'd'},
    {"config",required_argument,    0,   'c'},
    {"strace_delay",required_argument,    0,   'r'},
    {0, 0, 0, 0}};

  int x; 
  int option_index = 0;
  x = getopt_long (argc, argv, "p:n:l:m:d:c:r:",
                       long_options, &option_index);
  
  if (x == -1)
        break;

   switch (x)
         {
         case 0:
           /* If this option set a flag, do nothing else now. */
           if (long_options[option_index].flag != 0)
             break;
           printf ("option %s", long_options[option_index].name);
           if (optarg)
             printf (" with arg %s", optarg);
           printf ("\n");
           break;
  
 
         case 'p':
           std::cerr<<"Process_ID set as "<< optarg <<std::endl;
 	   str_pid=optarg;
           break;
 
         case 'n':
           std::cerr<<"Process name set as "<<optarg<<std::endl;
 	   str_pname=optarg;
           break;
 
         case 'l': //doesnt work fully
           std::cerr<<"Launching process   "<<optarg<<std::endl;
           str_launch=optarg; 
           break;
 
         case 'r':
           std::cerr<<"strace sampling time  set as "<< optarg<<std::endl;
           delays=optarg;
           run_delay=std::stoi(delays);
           break;
 
         case 'm':
           std::cerr<<"OpenTSDB metric set as "<< optarg<<std::endl;
           metric=optarg;
           break;
 
         case 'c':
           std::cerr<<"Config File is "<<optarg<<std::endl;
           configfile=optarg;
 	   launch_from_config(configfile,delays,delay,str_pname, str_pid);
           break;
        case 'd':
            int s;
            s=std::stoi(optarg);
            switch (s){
                case -1:
                    std::cerr<<"Running once "<<std::endl;delay=-1;break;
                case 0:
                    std::cerr<<"Running continuously"<<std::endl;delay=0; break;
                default:
                    std::cerr<<"Looping every "<<optarg<<" seconds"<<std::endl;delay=s;break;
            };break;
         case '?':
            std::cout<<"???";
           exit(1);
           break;
 
         default:
           break;
         }
 };

  std::cerr<<"$proc "<<str_pid<<" "<<str_pname<< " from "<<configfile<<" with delay (s) "<<delay<<std::endl;
  getPiD(str_pid,str_pname);
  if (str_launch!=""){
   std::cerr<< "launching process "<< str_launch<<std::endl;
   str_pname=str_launch;
  }


  std::ifstream filec("/proc/"+str_pid+"/cmdline");
  filec>>str_command;
  std::cerr<<"$proc cmd: "<<str_command<<std::endl;
  // rusage ru;

  time_t t = time(0);
  struct tm * now = localtime( & t );
  std::cerr <<"$proc-Start time: "<< (now->tm_hour)<<":"<<(now->tm_min)<<":"<<(now->tm_sec) << std::endl; 
  std::remove_if(configfile.begin(), configfile.end(), isspace);
  if (metric.empty()){
    metric="exe."+str_pname+"."+str_pid+".strace";}
  else {
    metric="exe."+metric+"."+str_pid+".strace";}
  std::map <std::string, int > dict;
  std::fstream tsdbfile; 

  std::string cmd1="timeout "+std::to_string(run_delay)+" strace -p ";
  std::string cmd3=" 2>&1";
  std::string command=cmd1+str_pid+cmd3;
  try{
   if (delay<0 ){
    strace_loop(command, dict, metric);}
   else if (delay==0 or delay<run_delay){
    while(not(kill(std::stoi(str_pid),NULL))){
      strace_loop(command, dict, metric);}}
   else { while(not(kill(std::stoi(str_pid),NULL))){
      strace_loop(command, dict, metric);
      usleep(delay*1000000);}}
  }
  catch(int e ){std::cerr<<"could not attach strace: Operation not permitted"<<std::endl;return -1;}
  return 0;
}


