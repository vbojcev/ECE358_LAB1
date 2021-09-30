#include <iostream>  //For debugging
#include <cmath>  //For logarithm
#include <vector>  //Used only as variable-size array (not explicit queue)
#include <algorithm>  //Used to sort the events in order of occurence
#include <time.h>  //Used to set the seed of the random number generator
#include <fstream>  //Used to output to a CSV file
#include <climits>

using namespace std;

enum eventType {ARRIVAL, DEPARTURE, OBSERVER};  //Three possible types of events

float expVar(float rate) {  //Important to note that this is the AVERAGE RATE being input, applicable directly for the rate of arrivals. For packet length, we know the average OF the random variable, therefore we will input 1/L
  return (float)-(1/rate)*log(1-((float)(rand()%1000)/1000));
}

struct event {  //Each event is only distinguished by its type and its time of occurence
  float t;
  int type;
};

event adder;  //A modifiable instance of an event that is used to add new events to the vector

bool compareTime (event i,event j) {  //This initializes the comparison condition for the sorting function
  return (i.t < j.t);
}

int main(int argc, char* argv[]) {

  vector<event> eventList;
  
  int initTime = (unsigned)time(nullptr);

  srand(initTime);

  float T = stof(argv[1]);  //First argument: simulation time
  
  float rho = stof(argv[2]);  //Second argument: utilization
  
  int pktL = stoi(argv[3]);  //Third argument: average packet length
  
  int bitRate = stoi(argv[4]);  //Fourth argument: bitrate of the channel (both sides)
  
  float currTime = 0;
  
  float lam = (rho)*(bitRate)/(pktL);  //Calculate the average rate of arrivals based on utilization, bitrate, and average packet size.
  
  //cout << lam << endl;
  
  while (currTime < T) {  //Generate arrivals until simulation time is reached
    currTime += expVar(lam);
    adder.t = currTime;
    adder.type = ARRIVAL;
    eventList.push_back(adder);
  }
  
  currTime = 0;
  
  int numArrivals = eventList.size();  //Total number of arrivals in the simulation time
  
  float transTime;  //Transmission time calculated for each packet
  
  for (int i = 0; i < numArrivals && currTime <= T; ++i) {
    transTime = expVar((float)1/pktL)/(float)bitRate;  //We input 1/L for the exponential variable since we want the average to be 1/lambda = 1/(1/L) = L
    //Important: we can calculate a packet's lenth after it's arrived because we are assuming the arrival time is the point at which it has completely entered the buffer, not the point at which the first bit hits the input. Therefore, arrival is independent of packet length and the two variable can be independently determined.
    if (eventList[i].t > currTime) {  //Case where the arrival we're examining has occured more recently than the last departure (queue empty, instant servicing).
      currTime = (float)eventList[i].t + transTime;
    } else {  //Case where a departure has occured after this arrival, meaning the departure time for this packet will be later since queue is nonempty.
      currTime += transTime;
    }
    adder.t = currTime;
    adder.type = DEPARTURE;
    eventList.push_back(adder);
  }
  
  currTime = 0;
  
  while (currTime < T) {  //Observers are generated in the exact same way as arrivals, but with a faster rate.
    currTime += expVar(lam*5);
    adder.t = currTime;
    adder.type = OBSERVER;
    eventList.push_back(adder);
  }
  
  sort(eventList.begin(), eventList.end(), compareTime);  //Automatically sorts the event list by order of time.
  
  int numArrive = 0;  //Total arrived packets.
  int numDepart = 0;  //Total departed packets.
  int numObserve = 0; //Total observing events.
  unsigned long int numQueue = 0;   //Running total of packets in queue, added at each observer event.
  int numIdle = 0;    //Number of observer events at which arrivals == departures.
  float averageIdle;  //Proportion of oberver events at which arrivals == departures.
  float averageQueueSize;  //Average number of packets in queue.
  
  ofstream output;
  output.open("out.csv");  //Outputting to forced file type "CSV" for data plotting
  output << "Utilization,E[n},P_idle\n";  //Setting up the column names
  
  for (int i = 0; i < eventList.size(); ++i) {  //We now iterate through the event list one last time, incrementing counters as we go and making calculations at observer events.
    switch(eventList[i].type) {
      case ARRIVAL:
        ++numArrive;
        break;
      case DEPARTURE:
        ++numDepart;
        break;
      case OBSERVER:
        ++numObserve;
        numQueue += numArrive - numDepart;  //The danger here is that if the utilization factor or packet size is too large, there is a possibility this value could overflow.
        if (numArrive == numDepart) {
          ++numIdle;
        }
        averageIdle = (float)numIdle/numObserve;
        averageQueueSize = (float)numQueue/numObserve;
    }
  }
  
  output << averageQueueSize << "," << averageIdle << "\n";
  
  cout << "E[n] = " << (float)averageQueueSize << ". P_idle = " << (float)averageIdle << ". Arrivals: " << numArrive << ". Departures: " << numDepart << ". Obervers: " << numObserve << endl;
  
  output.close();
  
  cout << "Program complete. Runtime: " << (unsigned)time(nullptr) - initTime << " seconds." << endl;

}
