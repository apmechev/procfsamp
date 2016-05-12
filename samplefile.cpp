#include <fstream>         /* std::ifstream */
#include <iostream>         /* std::cout */

//Timer includes
//#include <boost/timer.hpp>

#include "GUTimer.h"        /*Timers: time_h, rdtsc, chrono_hr, ctime, LOFAR_timer*/
#include <unistd.h>

//typedef long  DWORD; //4bytes
//template <typename Word>
//std::ifstream& read_word( std::ifstream& ins, Word& value );

//
//std::string
//getFileContent(const std::string& path)
//{
//    GUTimer t(GUTimer::chrono_hr);// GUTimer t(GUTimer::time_h)
//    GUTimer t1(GUTimer::chrono_hr);// GUTimer t(GUTimer::time_h)
//    t.start();
//    std::ifstream file(path,std::ifstream::binary);
//    t.stop();
////   DWORD word1;
////   DWORD word2;
//
//  t1.start();
//
////  std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
//  file.seekg(7);                          //rchar
//  char *  result= new char [70];
//  file.read(result,70);
//  //
//// file.seekg(238);                          //VMpeak
//// file.read((char*)&word1,sizeof(long)-1);  //VMpeak (+-1 byte)//
//// file.seekg(258);                          //VMsize
//// file.read((char*)&word1,sizeof(long)-1);  //VMsize (+-1 byte)
////
////
////  file.close();
//  t1.stop();
////  std::stringstream stream; stream << std::hex << word1;
////  std::string result( stream.str() );
//  std::cout<<t.getElapsed()<<" "<<t1.getElapsed()<<"\n";
//  std::cout<<"$samp "<<result<<'\n';
////   delete[] result;
//
//  return "";
//}
















//=============
//=============

std::string
getio(const std::string& path)
{

  std::ifstream file(path);


  std::string readchars,dummy,wrchars,syscw,syscr,read_bytes,write_bytes;
  file>>dummy>> readchars>>dummy>>wrchars>>dummy>>syscr>>dummy>>syscw>>dummy>>read_bytes>>dummy>>write_bytes;

  std::string result = readchars+" "+wrchars+" "+syscr+" "+syscw+" "+read_bytes+" "+write_bytes+'\n';

  return result;
}

std::string
getmem(const std::string& pid)//Get memory information (in one block right now)
{
    std::string path="/proc/"+pid+"/statm";
    std::ifstream file(path,std::ifstream::binary);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    return content;
}

std::string
getstat(const std::string& pid)//Get memory information (in one block right now)
{

    std::string state,dummy,minflt,mjflt,utime,s_time,nthreads,vsize,rss,iodelay;
    std::string path="/proc/"+pid+"/stat";
    std::ifstream file(path,std::ifstream::binary);
    file>>dummy>>dummy>>state>>dummy>>dummy>>dummy>>dummy>>dummy>>dummy>>minflt>>dummy>>mjflt>>dummy>>utime>>s_time>>dummy>>dummy>>dummy>>dummy>>nthreads>>dummy>>dummy>>vsize>>rss;
    std::string content=state+" "+minflt+" "+mjflt+" "+utime+" "+s_time+" "+nthreads+" "+vsize+" "+rss+'\n';

    return content;
}


//template <typename Word>
//std::ifstream& read_word( std::ifstream& ins, Word& value )
//  {
//  for (unsigned size = 0, value = 0; size < sizeof( Word ); ++size)
//    value |= ins.get() << (8 * size);
//  return ins;
//  }
#include <sys/stat.h>

inline bool exists (const std::string& name) {
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}

int main(int argc, char *argv[]) {
 std::string pid_s,configfile="",pnames="",delays;
 int delay=1000000;
 if ( argc < 2 ){
    std::cout<<"usage: "<< argv[0] <<" PID configfile \n";
    return 0;
    }
 else
  { if(isdigit(argv[1][0])) //Process ID number
    {
      pid_s= argv[1];
    }
    else                    //Either *.cfg file or process launch (NOT implemented)
    {
     configfile=argv[1];
     if (configfile!="" and configfile.find(".cfg"))
      {
       std::ifstream infile(configfile);
       if (infile.good())
        {
         getline(infile,pnames);
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
      }
      else
      {
        pnames=argv[1];
      }
    }
  }



 if(pid_s=="")
  {
    while(pid_s=="")
    {
      FILE* fpidof = popen(("pidof "+pnames).c_str(),"r");
      if (fpidof)
      {
        int p=0;
        if (fscanf(fpidof, "%d", &p)>0 && p>0)
        pid_s = std::to_string(p);
        pclose(fpidof);
      }
    }
  }
  std::cout<<pid_s<<" "<<pnames<< " from "<<configfile<<" with delay (ms) "<<delay/1000<<std::endl;

  std::cout<<"$proc-m VmSize(pg), VMRSS (pg), shared-pages, code, 0,  data+stack, 0\n";
  std::cout<<"$proc-i rchar, wchar, syscr, syscw, read_bytes, write_bytes\n";
  std::cout<<"$proc-s state, minflt, mjflt, utime ,s_time, nthreads, VMSize, RSS(pages)'\n';";
// rusage ru;
 time_t t = time(0);
 struct tm * now = localtime( & t );
 std::cout <<"$Start time "<< (now->tm_hour)<<":"<<(now->tm_min)<<":"<<(now->tm_sec) << std::endl;

 while(exists("/proc/"+pid_s+"/exe"))
 {
    printf(("$proc-io "+getio("/proc/"+pid_s+"/io")).c_str());
    printf(("$proc-mem " + getmem(pid_s)).c_str());
    printf(("$proc-stat "+getstat(pid_s)).c_str());

  usleep(delay);
 }
return 0;
}
