#include <cmath>  //Power
#include <iostream>
#include <string>  //stoi() function
#include <time.h>  //Random seed

using namespace std;

float lam;  //Exponential parameter
float U;  //Uniform random variable on [0,1]
float dataSet[1000];  //Array of outputs
float x;  //Exponential random variable
float sum = 0;  //For calculating mean
float squareSum = 0;  //For calculating the variance
float mean;
float squareMean;
float variance;

int main(int argc, char* argv[]) {

  if (argc != 2 ) {
  
    cout << "Invalid input. Please enter lambda and ONLY lambda" << endl;
    return 1;
    
  } else {

    lam = stof(argv[1]);
    
    cout << "lambda is: " << lam << endl;
    
    srand((unsigned)time(nullptr));  //the seed is the current time. Only needs to be done once.
    
    for (int i = 0; i < 1000; i++) {
    
      U = (float)(rand()%1000)/1000;  //Determine a uniform random variable in [0,1]
      x = -(1/lam)*log(1-U);  //Use the uniform random variable to determine an exponential random variable
      dataSet[i] = x;
      
    }
    
    for (int i = 0; i < 1000; i++) {
      sum += dataSet[i];
      squareSum += pow(dataSet[i], 2);
    }
    
    mean = sum/1000;  //mean of the random variable, or approximately the expected value E[X]
    squareMean = squareSum/1000;  //Equal to E[(x)^2]
    
    variance = squareMean - pow(mean, 2);  //Var(X) = E[(X^2)] - (E[X])^2
    
    cout << "Mean/Expected Value: " << (float)mean << endl;
    cout << "Variance: " << (float)variance << endl;
  
  }

  return 0;
}
