// g++ -std=c++11 TestKVRequest.cpp -lkvs_redis -lboost_serialization -pthread -Dthread_count=1

//NOTE: KVRequest operations are not performed in given sequence

#include "../../Implementation/Redis/client/src/KVStore.h"
#include<iostream>
#include<thread>

using namespace kvstore;
using namespace std;

void doSomething(int tid){
  const int iter = 1;

  KVRequest kr;
  //kr.bind("127.1.1.1:8090");//Invoked exactly once
  // kr.bind("127.0.0.1:7000");//Invoked exactly once
  kr.bind("10.129.28.101:7000");//Invoked exactly once
  //kr.bind("10.129.26.223:8090");//Invoked exactly once

  for(int i=0;i<iter;i++) {

  kr.put<int,string>(tid*1000+10,"OM","T1");
  kr.put<int,double>(tid*1000+10,0.9876,"T2");
  kr.get<int,string>(tid*1000+10,"T1");
  kr.get<int,double>(tid*1000+10,"T2");
  kr.del<int,double>(tid*1000+10,"T2");
  kr.get<int,double>(tid*1000+10,"T2");

  auto rs = kr.execute();
   if(rs.get<string>(2).ierr!=0 || rs.get<double>(3).ierr!=0 || rs.get<double>(5).ierr!=-1){
     cout<<"Error in tid "<<tid<<" "<<rs.get<string>(2).serr<<" "<<rs.get<string>(3).serr<<" "<<rs.get<string>(5).serr<<endl;
   }
   kr.reset();

   kr.get<int,double>(tid*1000+10,"T2");
   rs = kr.execute();
   if(rs.get<double>(0).ierr==0){
      cout<<"Error got data"<<tid<<" "<<rs.get<double>(0).value<<endl;
   } else {
     cout<<"SError"<<rs.get<double>(0).serr<<endl;
   }
      kr.reset();

  }


}

int main(){

  //const int thread_count=50;
  int i;

  thread threads[thread_count];
  auto fp=doSomething;
  for (i = 0; i < thread_count; i++) {
    threads[i] = thread(fp,i);
  }
  for (i = 0; i < thread_count; i++) {
    if (threads[i].joinable()) {
      threads[i].join();
    }
  }

  // KVRequest kr;
  // kr.bind("127.1.1.1:8090");//Invoked exactly once
  // kr.put<int,string>(10,"OM","T1");
  // kr.put<int,double>(10,0.9876,"T2");
  // kr.get<int,string>(10,"T1");
  // kr.get<int,double>(10,"T2");
  // kr.del<int,double>(10,"T2");
  // kr.get<int,double>(10,"T2");
  //
  // auto rs = kr.execute();
  // cout<<"RS size"<<rs.size()<<endl;
  //     cout<<"Get 10 T1 :"<<rs.get<string>(2).value<<endl;
  //     cout<<"Get 10 T2 :"<<rs.get<double>(3).value<<endl;
  //     cout<<"Get 10 T2 :"<<rs.get<double>(6).serr<<endl;
  //
  //
  //      rs=kr.execute();
  //   cout<<"RS size"<<rs.size()<<endl;
  //       cout<<"Get 10 T1 :"<<rs.get<string>(2).value<<endl;
  //       cout<<"Get 10 T2 :"<<rs.get<double>(3).value<<endl;
  // cout<<"Get 10 T2 :"<<rs.get<double>(5).serr<<endl;

  return 0;
}
