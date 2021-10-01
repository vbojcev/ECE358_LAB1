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
  bool dropped;
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

  float startRho = stof(argv[2]); //Second argument: utilization

  float endRho = stof(argv[3]);  //Third argument: utilization

  int pktL = stoi(argv[4]);  //Fourth argument: average packet length

  int bitRate = stoi(argv[5]);  //Fifth argument: bitrate of the channel (both sides)

  int bufferSize = 1;
  if (argc > 6) {
    bufferSize = stoi(argv[6]);  //Sixth argument: size of the queue
  }

  ofstream output;
  output.open("out.csv");  //Outputting to forced file type "CSV" for data plotting
  output << "Utilization,E[n},P_idle,P_loss\n";  //Setting up the column names

  int numArrive = 0;  //Total arrived packets.
  int numArriveDropped = 0;  //Total arrived packets dropped.
  int numDepart = 0;  //Total departed packets.
  int numObserve = 0; //Total observing events.
  unsigned long int numQueue = 0;   //Running total of packets in queue, added at each observer event.
  int numIdle = 0;    //Number of observer events at which arrivals == departures.
  float averageIdle = 0;  //Proportion of oberver events at which arrivals == departures.
  float averageLoss = 0;  //Proportion of arrivals dropped.
  float averageQueueSize = 0;  //Average number of packets in queue.

  for (float j = startRho * 10; j <= endRho * 10; ++j) {
    eventList.clear();

    float rho = j / 10;

    float currTime = 0;

    float lam = (rho)*(bitRate)/(pktL);  //Calculate the average rate of arrivals based on utilization, bitrate, and average packet size.

    while (currTime < T) {  //Generate arrivals until simulation time is reached
      currTime += expVar(lam);
      adder.t = currTime;
      adder.type = ARRIVAL;
      eventList.push_back(adder);
    }

    currTime = 0;

    int numArrivals = eventList.size();  //Total number of arrivals in the simulation time
    int numArrivalsDropped = 0;  //Number of arrivals dropped
    int bufferCapacity = 0; //Number of events in buffer
    int departureIndex = 0; //Number of departures accounted for

    float transTime;  //Transmission time calculated for each packet

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

    currTime = 0;

    while (currTime < T) {  //Observers are generated in the exact same way as arrivals, but with a faster rate.
      currTime += expVar(lam*5);
      adder.t = currTime;
      adder.type = OBSERVER;
      eventList.push_back(adder);
    }

    sort(eventList.begin(), eventList.end(), compareTime);  //Automatically sorts the event list by order of time.

    numArrive = 0;  //Total arrived packets.
    numArriveDropped = 0;  //Total arrived packets dropped.
    numDepart = 0;  //Total departed packets.
    numObserve = 0; //Total observing events.
    numQueue = 0;   //Running total of packets in queue, added at each observer event.
    numIdle = 0;    //Number of observer events at which arrivals == departures.
    averageIdle = 0;  //Proportion of oberver events at which arrivals == departures.
    averageLoss = 0;  //Proportion of arrivals dropped.
    averageQueueSize = 0;  //Average number of packets in queue.

    for (int i = 0; i < eventList.size(); ++i) {  //We now iterate through the event list one last time, incrementing counters as we go and making calculations at observer events.
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

    output << rho << "," << averageQueueSize << "," << averageIdle << "," << averageLoss << "\n";

    cout << "Rho: " << rho << ".E[n] = " << (float)averageQueueSize << ". P_idle = " << (float)averageIdle << ". P_loss = " << (float)averageLoss << ". Arrivals: " << numArrive << ". Departures: " << numDepart << ". Obervers: " << numObserve << endl;
  }

  output.close();

  cout << "Program complete. Runtime: " << (unsigned)time(nullptr) - initTime << " seconds." << endl;

}
