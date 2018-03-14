#include <iostream>
#include <iomanip>
#include <mpi.h>
#include <cmath>
#include <vector>
using namespace std;
class Grid2d{
private:
  const static int SIZE = 10;
  double* g;
  double* gf;
  uint32_t rows;
  uint32_t cols;
  double c1, c2;
public:
  Grid2d(uint32_t rows, uint32_t cols, double val = 0) : rows(rows), cols(cols){       //CONSTRUCTOR
    g = new double[rows * cols];
    gf = new double[rows * cols];
    for(int p = 0; p < rows * cols; p++)
      g[p] = val;
  }

  Grid2d(uint32_t rows, uint32_t cols, double* array) : rows(rows), cols(cols) {
    g = new double[rows * cols];
    
    for(int i = 0; i < rows * cols; i++) {
      g[i] = array[i];
    }
    gf = new double[1];
  }

  double* getG(){
    return g;
  }
  
  ~Grid2d(){                                                                      //DESTRUCTOR
    delete []g;
    delete []gf;
  }

  uint32_t getRows(){
    return rows;
  }

  uint32_t getCols(){
    return cols;
  }
  
  Grid2d(const Grid2d& orig){                                                    //COPY CONSTRUCTOR
    g = new double[orig.rows * orig.cols];
    rows = orig.rows;
    cols = orig.cols;
    for(int p = 0; p < rows * cols; p++)
      g[p] = orig.g[p];
  }

  Grid2d& operator =(const Grid2d& orig){                                       //OPERATOR =
    Grid2d copy(orig);
    swap(copy.g,g);
    swap(copy.rows,rows);
    swap(copy.cols,cols);
  }

  void startPoint(uint32_t r, uint32_t c, double conc){
    g[r*cols + c] = conc;
  }

  /* void specPoint(uint32_t r, uint32_t c) const {                        instead overload ()
     cout <<  g[r*cols + c] << '\n';
     }*/

  double operator ()(uint32_t r, uint32_t c) const{
    return g [r*cols + c];
  }

  const int dirs[4][2] = {{-1,0},{0, -1}, {1, 0}, {0, 1}};
  
  void diffuse(double a1, double a2){
    c1 = a1;
    c2 = a2;
    for(int i = 0; i < rows; i++){
      for(int j = 0; j < cols; j++){
	double cur = c1 * g[ i * cols + j];
	for(auto dir : dirs){
	  int x = i + dir[0];
	  int y = j + dir[1];
	  if( x >= 0 && x < rows && y >= 0 && y < cols)
	    cur += c2 * g[ x * cols + y];
	}
	gf [ i * cols + j] = cur;
      }
    }
    double* temp = g;
    g = gf;
    gf = temp;
  }

  bool checkDisp(){
    for(int i = 0; i < rows*cols; i++){
      if(g[i] == 0)
	return true;}
    return false;
  }
  vector<double> edgeComputing(uint32_t r, uint32_t c, bool rowOrCol) {
    //true -> row, false -> col
    vector<double> v;
    if(rowOrCol) {
      c = 0;
      for(; c < cols; c++) {
	v.push_back(c2 * gf[r * cols + c]);
      }
    } else {
      r = 0;
      for(; r < rows; r++){
	v.push_back(c2 * gf[r * cols + c]);
      }
      
    }
    return v;
  }
  void addRowOrCol(uint32_t r, uint32_t c, bool rowOrCol, vector<double>& res) {
    //true -> row, false -> col
    //vector<double> v;
    if(rowOrCol) {
      c = 0;
      for(; c < cols; c++) {
	g[r * cols + c] += res[c];
      }
    } else {
      r = 0;
      for(; r < rows; r++){
	g[r * cols + c] += res[r];
      }
      
    }
    return ;
  }
  void addRowOrCol1(uint32_t r, uint32_t c, bool rowOrCol, vector<double>& res) {
    //true -> row, false-> col
    if(rowOrCol){
      for(int i = 0; i < cols; i++){
	g[r * cols + i] += res[i];
      }
    }
    else
      for(int i = 0; i < rows; i++){
	g[i * cols + cols - 1] += res[i];
      }    
  }
  
  friend ostream& operator << (ostream& s, const Grid2d& grid);
  
};

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
  /*
    void send(vector<double> data, int dest){
    MPI_Send(& data, data.size(), MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);
    }

    void receive(vector<double> data, int source){
    MPI_Recv(& data, data.size(), MPI_DOUBLE, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
  */
  Grid2d init() {
    int32_t col = 10;
    int32_t row = 10;
    Grid2d g(row/2, col/2);
    vector<vector<int>> pos = {{0,0}, {9,9}};
    vector<double> val = {5, 2.5};
    for(int i = 0; i < pos.size(); i++) {
      int offsetX = 0;
      int offsetY = 0;
      switch(getRank()) {
      case 0: break;
      case 1: offsetY = -col/2;
	break;
      case 2: offsetX = -row/2;
	break;
      case 3: offsetX = -row/2;
	offsetY = -col/2;
      }
      int x = pos[i][0] + offsetX;
      int y = pos[i][1] + offsetY;
      if(x < 0 || x >= row/2 || y < 0 || y >= col/2) continue;
      g.startPoint(x, y, val[i]);
    }
    //if(getRank() == 2) cout << g << endl;
    return g;
  }

  void rearrange(double* array1, double* array2, uint32_t rows, uint32_t cols){   //rows = rows/2 & cols = cols/2
    int k = 0;
    // rank 0
    for(int i = 0; i < rows; i++) {
      for(int j = 0; j < cols; j++) {
	array2[i * cols * 2 + j] = array1[k++]; 
      }
    }
    // rank 1
    for(int i = 0; i < rows; i++) {
      for(int j = 0; j < cols; j++) {
	array2[i * cols * 2 + j + cols] = array1[k++]; 
      }
    }
    // rank 2
    for(int i = 0; i < rows; i++) {
      for(int j = 0; j < cols; j++) {
	array2[( i + rows) * cols * 2 + j ] = array1[k++]; 
      }
    }
    //rank 3
    for(int i = 0; i < rows; i++) {
      for(int j = 0; j < cols; j++) {
	array2[( i + rows) * cols * 2 + j + cols ] = array1[k++]; 
      }
    }
  }
  
  void run() {
    Grid2d g = init();
    
    // if(getRank() == 2) cout << g << endl;
    uint32_t r = 0,c = 0;
    int des1, des2;
    switch(getRank()){
    case 0: r = g.getRows() - 1;
      c = g.getCols() - 1;
      des1 = 2; des2 = 1;
      break;
    case 1: r = g.getRows() - 1;
      des1 = 3; des2 = 0;
      break;
    case 2: c = g.getCols() - 1;
      des1 = 0; des2 = 3;
      break;
    case 3: des1 = 1; des2 =2;
    }
    vector<double> v1, v2;
    for(int i = 0; i < 5; i++) {
      g.diffuse(0.8, 0.2);
      v1 = g.edgeComputing(r, c, 1);
      v2 = g.edgeComputing(r, c, 0);
      MPI_Send(&v1[0], v1.size(), MPI_DOUBLE, des1, 0, MPI_COMM_WORLD);
      MPI_Recv(&v1[0], v1.size(), MPI_DOUBLE, des1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Send(&v2[0], v2.size(), MPI_DOUBLE, des2, 0, MPI_COMM_WORLD);
      MPI_Recv(&v2[0], v2.size(), MPI_DOUBLE, des2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

      g.addRowOrCol(r, c, 1, v1);
      g.addRowOrCol(r, c, 0, v2);
      //if(getRank() == 0) cout << g << endl;
    }
    double* array1 = nullptr;
    double* array2 = nullptr;
    if(getRank() == 0){
      array1 = new double[g.getRows()*g.getCols()*4];
      array2 = new double[g.getRows()*g.getCols()*4];
    }
    MPI_Gather(g.getG(), g.getRows()*g.getCols(), MPI_DOUBLE, array1, g.getRows()*g.getCols(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
    if(getRank() == 0) {
      rearrange(array1, array2, g.getRows(), g.getCols());
    
      Grid2d f(g.getRows()*2, g.getCols()*2, array2);

    
      delete[] array1;
      delete[] array2;
      cout << f;
    }
  }
  
};


ostream& operator << (ostream& s, const Grid2d& grid){
  for(int p = 0; p < grid.rows * grid.cols; p++){
    if( p != 0 && p % grid.cols == 0)
      s << '\n';
    s << grid.g[p] << '\t';
  }
  return s << '\n';  
}

int main(){
  WorldManager w;
  w.run();
}

/*
  int main() {
  WorldManager w();
  Grid2d grid(10, 10);
  // cout << grid; 
  grid.startPoint(0,0,5);
  grid.startPoint(9,9,5);
  cout << grid << endl;
  //grid.specPoint(0,0);
  grid.diffuse(0.4, 0.2);
  while(grid.checkDisp()){
  grid.diffuse(0.4, 0.2);
  cout << grid;
  }
  
  g.printInitGrid();
  g.printGrid();
  g.printFinalGrid();
  }
*/

