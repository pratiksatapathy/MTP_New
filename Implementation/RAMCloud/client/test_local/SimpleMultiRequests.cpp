/*
g++ --std=c++0x -g -I/home/jashdave/RAMCloud/install/include -I. SimpleMultiRequests.cpp  -o SimpleMultiRequests.o -L/home/jashdave/RAMCloud/install/bin -lramcloud -pthread -Wl,-rpath=/home/jashdave/RAMCloud/install/bin
*/


// #include "../../Implementation/RAMCloud/client/src/KVStore.h"
#include <iostream>
#include <thread>

#include <stdio.h>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <string>
#include <ctime>

#include "ramcloud/RamCloud.h"

using namespace std;
using namespace RAMCloud;

int main(){
	string config="tcp:host=10.129.28.101,port=11100";
	string tablename = "TestTable2";

	int sz=3;
	string keys[]={"key1", "key2", "key3"};
	string vals[]={"val1", "val2", "val3"};


	RamCloud cluster(config.c_str());
	int tableId = cluster.createTable(tablename.c_str());

	MultiWriteObject *mwo[sz];
  for(int i=0;i<sz;i++) {
		mwo[i] = new MultiWriteObject(tableId, keys[i].c_str(), keys[i].size(), vals[i].c_str(), vals[i].size()+1);
	}
	cluster.multiWrite(mwo, sz);


// keys[0]="testkey";
	MultiReadObject *mro[sz];
	Tub<ObjectBuffer> retval[sz];
  // for(int i=0;i<sz;i++) {
	// 	cout<<retval[i].get()->getNumKeys()<<endl;
	// }
  for(int i=0;i<sz;i++) {
		mro[i] = new MultiReadObject(tableId, keys[i].c_str(), keys[i].size(), &retval[i]);
	}
	cluster.multiRead(mro,sz);

	for(int i=0;i<sz;i++) {
		cout<<(int)retval[i].get()->getNumKeys()<<endl;
	}

		for(int i=0;i<sz;i++) {
			cout<<(char *)retval[i].get()->getValue()<<endl;
		}
}
