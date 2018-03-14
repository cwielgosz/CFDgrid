#include <mpi.h>
#include <iostream>
#include <cmath>
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

bool isPrime(int p){
  if( p <= 1 ) return false;
  for(int i = 2; i <= sqrt(p); i++){
    if( p % i == 0) return false;
  }
  return true;
}

int countPrimes(int start, int end){
  int count = 0;
  for(int i = start; i <= end; i++)
    if(isPrime(i))
      count ++;
  return count;
}

void run(){
  int totNum = 100;
  int range = totNum / worldSize;
  int* buffer = new int[sizeof(int)*range]; 

  MPI_Scatter(&totNum, range, MPI_INT, buffer, range, MPI_INT, 0, MPI_COMM_WORLD);

  int subTotPrimes = countPrimes(buffer[0], buffer[sizeof(buffer)/sizeof(int)]); //PROBLEM!!
      
  int* subPrimes = nullptr;
  if( worldRank == 0 ){
  subPrimes = new int[sizeof(int)*&subTotPrimes];
  }

  MPI_Gather(&subTotPrimes, subTotPrimes, MPI_INT, subPrimes, subTotPrimes, MPI_INT, 0, MPI_COMM_WORLD);

  if (worldRank == 0){
  cout << subTotPrimes << '\n';
  }
  
}

  
	     
  /*
  int res = countPrimes (worldRank*range + 1, range*(worldRank + 1));
  cout << res;
}
  */
};



int main(){
  
  WorldManager wm;
  wm.run();  
  
  // cout << "yello world from processor" <<' '<< wm.getPName() << ", rank" <<' '<< wm.getRank() <<' '<< "out of" <<' '<< wm.getSize() <<' '<< "processors." << '\n';
}
