#include <iomanip>
#include <mpi.h>
#include <cmath>
#include <vector>
#include <iostream>
using namespace std;

class WorldManager{
private:
  int worldSize;
  int worldRank;
  char processorName[MPI_MAX_PROCESSOR_NAME];
  int nameLen;
public:
  WorldManager(){
    MPI_Init(nullptr, nullptr);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
    MPI_Get_processor_name(processorName, &nameLen);
  }


int getSize(){
    return worldSize;
}

int getRank(){
  return worldRank;
}

string getPName(){
    string s = "";
    for(int i = 0; i < nameLen; i++)
      s += processorName[i];
    return s;
}

~ WorldManager(){
    MPI_Finalize();
}

  void send(int* data, int dest){
    MPI_Send(& data, sizeof(data)/sizeof(int), MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);
  }

  void receive(int* data, int source){
    MPI_Recv(&data, sizeof(data)/sizeof(int), MPI_DOUBLE, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
};


int main(){
  
  WorldManager w;
  int pingpongcount = 0;
  int partRank = (w.getRank() + 1) % 2;
  while(pingpongcount < 10){
    if(w.getRank() == pingpongcount % 2){
      pingpongcount ++;
      w.send(&pingpongcount, partRank);
      cout << w.getRank() << "sent and incremented ppc" << pingpongcount << "to" << partRank << "\n";
    }

    else{
      w.receive(&pingpongcount, partRank);
      cout << w.getRank() << "received ppc" << pingpongcount << "from" << partRank << "\n";
    }
  }
}
