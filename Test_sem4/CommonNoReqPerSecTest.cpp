/*
   g++ --std=c++0x -g -I../install/include -I. RMCLoadTestMultiThreaded.cpp  -o RMCLoadTestMultiThreaded.out -L../install/bin -lramcloud -Wl,-rpath=../install/bin -pthread
 */


#define RUN_TIME 10
#define THREAD_COUNT 20
#define IP "10.129.26.81"
#define PORT "11100"
// #define IP "10.129.28.44"
// #define PORT "7001"


// #include "../../Implementation/RAMCloud/client/src/KVStore.h"
#include <iostream>
#include <thread>
#include <stdio.h>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <string>
#include <ctime>
#include <unistd.h>
#include "TestUtils.h"
#include "KVImplementation.h"
// #include "MessageClient.cpp"
// #include "ramcloud/RamCloud.h"

// #ifdef REDIS
//   #include <redis>
//   using namespace kvstore;
// #endif /* REDIS */

using namespace std;
// using namespace RAMCloud;
// using namespace kvstore;
// using namespace MessageClientNS;

#ifdef DEFAULTIMPL
  class KVImplementation {
  public:
    bool bind(string ip, string port){return false;}
    bool put(string key, string value){return false;}
    string get(string key){return "Err";}
  };
#endif


//------------------------------------------------------------------------------
bool run;
long long counter[THREAD_COUNT];
long long failure[THREAD_COUNT];
double total_latency[THREAD_COUNT];
thread td[THREAD_COUNT];
long long count;
long long fcount;
double avg_latency;
string value = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789";


void sumUp(){
    count=0;
    fcount=0;
    avg_latency=0;
    for(int i=0;i<THREAD_COUNT;i++){
      count+=counter[i];
      fcount+=failure[i];
      avg_latency+=total_latency[i];
    }
    avg_latency/=count;
}

void reset(){
  run=false;
  count=0;
  fcount=0;
  avg_latency=0;
  for(int i=0;i<THREAD_COUNT;i++){
    counter[i]=0;
    failure[i]=0;
    total_latency[i]=0;
  }
}

void printinfo(string info=""){
  cout<<endl;
  cout<<info<<endl;
  cout<<"Run Time : "<<RUN_TIME<<" seconds"<<endl;
  cout<<"THREAD_COUNT : "<<THREAD_COUNT<<""<<endl;
  cout<<"Avg. Latency : "<<avg_latency<<" us"<<endl;
  cout<<"Failure : "<<fcount/double(RUN_TIME)<<" ops"<<endl;
  cout<<"Throughput : "<<count/double(RUN_TIME)<<" ops"<<endl;
  cout<<"Good Throughput : "<<(count-fcount)/double(RUN_TIME)<<" ops"<<endl;
}

//------MOVE TO TestUtils.h-------------------------
inline high_resolution_clock::time_point start(){
  return high_resolution_clock::now();
}

inline double end(high_resolution_clock::time_point& t1){
  high_resolution_clock::time_point t2=high_resolution_clock::now();
  return (double)duration_cast<microseconds>(t2 -t1).count();
}
//--------------------------------------------------

void doPut(int tid,KVImplementation k){
  string key = "Key"+to_string(tid);
  while(!run);
  while(run){
    auto t1 = start();
    if(!k.put(key,value)){
      failure[tid]++;
      // cout<<"Error"<<endl;
    }
    total_latency[tid]+=end(t1);
    counter[tid]++;
  }
}

void doGet(int tid,KVImplementation k){
  string key = "Key"+to_string(tid);
  while(!run);
  while(run){
    auto t1 = start();
    string val = k.get(key);
    total_latency[tid]+=end(t1);
    if(val!=value){
      failure[tid]++;
      // cout<<"Error"<<endl;
    }
    counter[tid]++;
  }
}


void pinThreadToCPU(thread *th,int i){
  long num_cpus;
  cpu_set_t cpuset;
  int rc;
  num_cpus = std::thread::hardware_concurrency();
  CPU_ZERO(&cpuset);
  CPU_SET(i%num_cpus, &cpuset);
  rc = pthread_setaffinity_np(th->native_handle(), sizeof(cpu_set_t), &cpuset);
  if (rc != 0) {
  	std::cerr << "Error calling pthread_setaffinity_np: " << rc << "\n";
  }
}

//----------------------------------------------
#define doTest(info,...) \
  reset();\
  for(int i=0;i<THREAD_COUNT;i++){\
    KVImplementation k;\
    k.bind(IP,PORT);\
    td[i] = thread(__VA_ARGS__);\
  }\
  run=true;\
  sleep(RUN_TIME);\
  run=false;\
  for (int i = 0; i < THREAD_COUNT; i++) {\
    if (td[i].joinable()) {\
      td[i].join();\
    }\
  }\
  sumUp();\
  printinfo(info);
//---------------------------------------------

int main(int argc, char *argv[]){
  {doTest("___PUT Operations____",doPut,i,k)}
  {doTest("___GET Operations____",doGet,i,k)}
  return 0;
}
