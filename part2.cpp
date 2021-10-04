#include <iostream>   //Debugging
#include <cmath>      //Logarithm
#include <vector>     //Variable-size array (not explicit queue)
#include <algorithm>  //Sort the events in order of occurence
#include <time.h>     //Set the seed of the random number generator
#include <fstream>    //Output to a file

using namespace std;

enum eventType {ARRIVAL, DEPARTURE, OBSERVER};  //Three possible types of events

//Each event is only distinguished by its type and its time of occurence (dropped is only used for arrivals)
struct event {
  float t;
  int type;
  bool dropped;
};

event adder;  //A modifiable instance of an event that is used to add new events to the vector

//Important to note that this is the AVERAGE RATE being input, applicable directly for the rate of arrivals.
//For packet length, we know the average OF the random variable, therefore we will input 1/L
float expVar(float rate) {
  return (float)-(1/rate)*log(1-((float)(rand()%1000)/1000));
}

//Comparison condition for the sorting function
bool compareTime (event i,event j) {
  return (i.t < j.t);
}

int main(int argc, char* argv[]) {
  int initTime = (unsigned)time(nullptr);
  srand(initTime);

  //Parameters
  float T = stof(argv[1]);              //First argument: simulation time
  float startRho = stof(argv[2]);       //Second argument: utilization
  float endRho = stof(argv[3]);         //Third argument: utilization
  int pktL = stoi(argv[4]);             //Fourth argument: average packet length
  int bitRate = stoi(argv[5]);          //Fifth argument: bitrate of the channel (both sides)
  int bufferSizeArr[] = {10, 25, 50};   //Array of buffer sizes

  ofstream output1;
  ofstream output2;
  output1.open("q4_data1.txt");
  output2.open("q4_data2.txt");

  vector<event> eventList;          //Vector of all events
  int bufferSize = 0;               //Size of the buffer
  int numArrive = 0;                //Total arrived packets.
  int numArriveDropped = 0;         //Total arrived packets dropped.
  int numDepart = 0;                //Total departed packets.
  int numObserve = 0;               //Total observing events.
  unsigned long int numQueue = 0;   //Running total of packets in queue, added at each observer event.
  int numIdle = 0;                  //Number of observer events at which arrivals == departures.
  float averageIdle = 0;            //Proportion of oberver events at which arrivals == departures.
  float averageLoss = 0;            //Proportion of arrivals dropped.
  float averageQueueSize = 0;       //Average number of packets in queue.

  //Loop through rho range
  for (float j = startRho * 10; j <= endRho * 10; ++j) {
    float rho = j / 10;
    output1 << rho;
    output2 << rho;

    //Loop through K range
    for (int k = 0; k < sizeof(bufferSizeArr)/sizeof(int); ++k) {
      bufferSize = bufferSizeArr[k];

      eventList.clear();

      float lam = (rho)*(bitRate)/(pktL);  //Calculate the average rate of arrivals based on utilization, bitrate, and average packet size.

      //Generate arrivals
      float currTime = 0;
      while (currTime < T) {
        currTime += expVar(lam);
        adder.t = currTime;
        adder.type = ARRIVAL;
        eventList.push_back(adder);
      }

      int numArrivals = eventList.size();   //Total number of arrivals in the simulation time
      int numArrivalsDropped = 0;           //Number of arrivals dropped
      int bufferCapacity = 0;               //Number of events in buffer
      int departureIndex = 0;               //Number of departures accounted for
      float transTime;                      //Transmission time calculated for each packet

      //Generate departures
      //Keeps track of the number of packets in the buffer by counting the number of departures between the current and previous arrivals
      currTime = 0;
      for (int i = 0; i < numArrivals && currTime <= T; ++i) {
        for (int j = departureIndex; j < i - numArrivalsDropped; j++) {  //Loops from last departure accounted for to most recent departure
          if (eventList[i].t >= eventList[numArrivals + j].t) {
            --bufferCapacity;
            ++departureIndex;
          }
        }
        if (bufferCapacity == bufferSize) {
          eventList[i].dropped = true;
          ++numArrivalsDropped;
        } else {
          ++bufferCapacity;
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
      }

      //Generate observers
      currTime = 0;
      while (currTime < T) {
        currTime += expVar(lam*5);
        adder.t = currTime;
        adder.type = OBSERVER;
        eventList.push_back(adder);
      }

      sort(eventList.begin(), eventList.end(), compareTime);  //Automatically sorts the event list by order of time.

      numArrive = 0;          //Total arrived packets.
      numArriveDropped = 0;   //Total arrived packets dropped.
      numDepart = 0;          //Total departed packets.
      numObserve = 0;         //Total observing events.
      numQueue = 0;           //Running total of packets in queue, added at each observer event.
      numIdle = 0;            //Number of observer events at which arrivals == departures.
      averageIdle = 0;        //Proportion of oberver events at which arrivals == departures.
      averageLoss = 0;        //Proportion of arrivals dropped.
      averageQueueSize = 0;   //Average number of packets in queue.

      //Iterate through the eventList one last time, incrementing counters as we go and making calculations at observer events.
      for (int i = 0; i < eventList.size(); ++i) {
        switch(eventList[i].type) {
          case ARRIVAL:
            if (!eventList[i].dropped) {
              ++numArrive;
            } else {
              ++numArriveDropped;
            }
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
            averageLoss = (float)numArriveDropped/(numArrive + numArriveDropped);
            averageQueueSize = (float)numQueue/numObserve;
        }
      }

      output1 << " " << averageQueueSize;
      output2 << " " << averageLoss;

      cout << "Rho: " << rho << ". K: " << bufferSize << ". E[n] = " << (float)averageQueueSize << ". P_idle = " << (float)averageIdle << ". P_loss = " << (float)averageLoss << ". Arrivals: " << numArrive << ". Departures: " << numDepart << ". Obervers: " << numObserve << endl;
    }
    output1 << "\n";
    output2 << "\n";
  }

  output1.close();
  output2.close();

  cout << "Program complete. Runtime: " << (unsigned)time(nullptr) - initTime << " seconds." << endl;

}
