#include <iostream>
#include <iomanip>

using namespace std;

class Grid2d{
private:
  const static int SIZE = 10;
  double* g;
  double* gf;
  uint32_t rows;
  uint32_t cols;
public:
  Grid2d(uint32_t rows, uint32_t cols, double val = 0) : rows(rows), cols(cols){       //CONSTRUCTOR
    g = new double[rows * cols];
    gf = new double[rows * cols];
    for(int p = 0; p < rows * cols; p++)
      g[p] = val;
  }

  ~Grid2d(){                                                                      //DESTRUCTOR
    delete []g; 
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
  /*
   const int N = -cols;                  outside method!!! YOU DON'T WANT TO INITIALIZE IT EVERY TIME
   const int S = cols;
   const int E = 1;
   const int W = -1;


  void diffuse(double c1, double c2){                       PROBLEM WITH BOUNDARIES

   for(int i = 0; i < rows * cols; i++){
     gf[i] = c1*g[i];
     
     if( i+N >= 0 && i+N < rows*cols )
       gf[i] += c2*g[i+N];
     if( i+S >= 0 && i+S < rows*cols )
       gf[i] += c2*g[i+S];
     if( i+E >= 0 && i+E < rows*cols )
       gf[i] += c2*g[i+E];
     if( i+W >= 0 && i+W < rows*cols )
       gf[i] += c2*g[i+W];
   }

   double* temp = g;
   g = gf;
   gf = g;

  }
  */

  const int dirs[4][2] = {{-1,0},{0, -1}, {1, 0}, {0, 1}};
  
  void diffuse(double c1, double c2){
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
  
  friend ostream& operator << (ostream& s, const Grid2d& grid);
  
};

ostream& operator << (ostream& s, const Grid2d& grid){
  for(int p = 0; p < grid.rows * grid.cols; p++){
    if( p != 0 && p % grid.cols == 0)
      s << '\n';
    s << grid.g[p] << '\t';
  }
  return s << '\n';  
}




int main() {
  Grid2d grid(10, 10);
  // cout << grid; 
  grid.startPoint(0,0,5);
  grid.startPoint(9,9,2.5);
  cout << grid << endl;
  for(int i = 0; i < 5; i++) {
    grid.diffuse(0.8, 0.2);
  }
  cout << grid << endl;
  //grid.specPoint(0,0);
  /*
  while(grid.checkDisp()){
    grid.diffuse(0.4, 0.2);
    cout << grid;}

  /*
  g.printInitGrid();
  g.printGrid();
  g.printFinalGrid();*/
  }
