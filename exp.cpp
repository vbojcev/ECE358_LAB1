#include <cmath>
#include <iostream>
#include <string>
#include <time.h>

using namespace std;

float lam;
float U = 1;
float dataSet[1000];
float x;
float sum = 0;
float squareSum = 0;
float mean;
float squareMean;
float variance;

int main(int argc, char* argv[]) {

cout << argv[0] << endl;

  if (argc != 2 ) {
    cout << "Invalid input. Please enter lambda and ONLY lambda" << endl;
    return 1;
  } else {
  
    lam = stof(argv[1]);
    
    cout << "lambda is: " << lam << endl;
    
    srand((unsigned)time(nullptr));  //the seed is the current time.
    
    for (int i = 0; i < 1000; i++) {
    
      U = (float)(rand()%1000)/1000;
      x = -(1/lam)*log(1-U);
      dataSet[i] = x;
      
    }
    
    for (int i = 0; i < 1000; i++) {
      sum += dataSet[i];
      squareSum += pow(dataSet[i], 2);
    }
    
    mean = sum/1000;
    squareMean = squareSum/1000;
    
    variance = squareMean - pow(mean, 2);
    
    cout << "Mean/Expected Value: " << (float)mean << endl;
    cout << "Variance: " << (float)variance << endl;
  
    return 0;
  }

  return 0;
}
