/* CS 370
   Project #3 
   Gerardo Gomez-Martinez
   October 16, 2014

   Program acts as a simplified version of the Linux scheduler. It accepts a file,
   through Unix redirection, as input containing the information of various process
   to run and acts as a CPU scheduler. 
*/

#include <iostream>
#include <iomanip>
#include <string>
#include <stdlib.h>

using namespace std;

class processType
{
public:
  processType(int, int, int, int, int[], int[]);
  int getPid();
  int getTimeSlice();
  int getPriority();
  int getArrivalTime();
  void calcTimeSlice();
  void calcPriority();
  void performCPU();
  bool cpuBurstE();
  void performIO();
  int getIOBurstLeft();
  void setFinishTime(int);
  int getFinishTime();
  bool isFinished();
  int getTurnAroundTime();
  int getWaitingTime();
  double getCpuUtilization();
  void printInf();
  void printInfo();
private:
  int originalPriority();
  void calcBonus();
  int niceValue; // A process' nice value
  int arrivalTime; // A process' arival time
  int cpuBursts; // A process' number of cpu bursts
  int ioBursts; // A process' number io bursts
  int priority; // A process' current priority
  int timeSlice; // A process' current time slice
  int pid; // A process' ID
  int bonus; // A process' current bonus
  int* cpuBurstsL; // A process' list of CPU bursts
  int* ioBurstsL; // A process' list of IO bursts
  int totalCPU; // The process' total time spent in the CPU up to this point
  int totalIO; // The process' total time spent in IO up to this point
  int finishTime; // A process' finish time
  bool finished; // If a process has finished executing
  bool cpuBurstEx; // If a current CPU burst has been exhausted
  bool ioBurstEx; // If a current IO burst has been exhausted
};

// processType constructor initializes the process' variables to values obtained from the
// input file or to default values
  processType::processType(int id, int nice, int arrival, int numCPU, int cpuB[], int ioB[])
  {
  pid = id;
  niceValue = nice;
  arrivalTime = arrival;
  cpuBursts = numCPU;
  ioBursts = numCPU - 1;
  cpuBurstsL = new int[cpuBursts];
  ioBurstsL = new int[ioBursts];
  for (int i = 0; i < cpuBursts; i++)
    {
      cpuBurstsL[i] = cpuB[i];
    }
  for (int i = 0; i < ioBursts; i++)
    {
      ioBurstsL[i] = ioB[i];
    }
  totalCPU = 0;
  totalIO = 0;
  priority = originalPriority();
  cpuBurstEx = false;
  ioBurstEx = false;
  finished = false;
  }

// Returns the process id of a process
int processType::getPid()
{
  return pid;
}

// Returns the time slice of a process
int processType::getTimeSlice()
{
  return timeSlice;
}

// Returns the priority of a process
int processType::getPriority()
{
  return priority;
}

// Returns the arrival time of a process
int processType::getArrivalTime()
{
  return arrivalTime;
}

// Calculate a new time slice for the process whenever it is called
void processType::calcTimeSlice()
{
  timeSlice = static_cast<int>((1-(priority/150.0)) * 395 + 0.5) + 5;
}

// Calculates and returns the original priority of the process
int processType::originalPriority()
{
  int origP;

  origP = static_cast<int>(((niceValue+20)/39.0)*30+0.5)+105;

  return origP;
}

// Calculates a new priority for the process whenever it is called
void processType::calcPriority()
{
  calcBonus();

  priority = originalPriority() + bonus;
}

// Calculates a new bonus for the process whenever it is called
void processType::calcBonus()
{
  if(totalCPU < totalIO)
    bonus = static_cast<int>(((1 - (totalCPU/static_cast<double>(totalIO)))*-5)-0.5);
  else
    bonus = static_cast<int>(((1 - (totalIO/static_cast<double>(totalCPU)))*5)+0.5);
}
 
// Performs the functions of the CPU by decrementing the value of the current CPU burst
// being processed and also checks if the current CPU burst is finished or if all CPU
// bursts are finished 
void processType::performCPU()
{
  int i = 0;
  
  cpuBurstEx = false;
  timeSlice--;
  
  while (i < cpuBursts && cpuBurstsL[i] == 0)
    {
      i++;
    }
    
  if (i == cpuBursts)
    {
      finished = true;
      cpuBurstEx = true;
    }
  else
    { 
      
      cpuBurstsL[i] = cpuBurstsL[i] - 1;
      totalCPU++;
      
      if (cpuBurstsL[i] == 0)
        {
          cpuBurstEx = true;
          i++;
          if (i == cpuBursts)
            {
              finished = true;
            }
        }
    }
  
}

// Returns a boolean variable conatining if the current CPU burst has been exhausted
bool processType::cpuBurstE()
{
  return cpuBurstEx;
}

// Performs the function of the IO device by decrementin the value of the current IO
// burst
void processType::performIO()
{
  int i = 0;
    
  while (i < ioBursts && ioBurstsL[i] == 0)
    {
      i++;
    }
    
  if (i < ioBursts)
    {
      ioBurstsL[i] = ioBurstsL[i] - 1;
      totalIO++;
    }
}

// Returns the amount of time left in the current IO burst being processed
int processType::getIOBurstLeft()
{
  int i = 0;
  
  while (ioBurstsL[i] == 0 && i < ioBursts)
    {
      i++;
    }
    
  if (i < ioBursts) 
    return ioBurstsL[i];
  else
    return 0;
}

// Accepts an int variable containing the time when a process finished executing and
// assigns it to the finishTime variable
void processType::setFinishTime(int time)
{
  finishTime = time;
}

// Returns the time when a process finished executing
int processType::getFinishTime()
{
  return finishTime;
}

// Returns a boolean variable containing if the process has finished executing
bool processType::isFinished()
{
  return finished;
}

// Returns the turn around time of a process
int processType::getTurnAroundTime()
{
  return finishTime-arrivalTime;
}

// Returns the waiting time of a process
int processType::getWaitingTime()
{
  return getTurnAroundTime() - totalCPU - totalIO;
}

// Returns the CPU utilization of a process
double processType::getCpuUtilization()
{
  return (totalCPU*1.0 / getTurnAroundTime())*100;
}

// Prints some info about the process
void processType::printInf()
{
  cout << "PID: " << pid << endl
       << "Nice Value: " << niceValue << endl
       << "Arrival Time: " << arrivalTime << endl
       << "CPU bursts: " << cpuBursts << endl
       << "IO bursts: " << ioBursts << endl
       << "CPU bursts list: ";
  for (int i = 0; i < cpuBursts; i++)
    {
      cout << cpuBurstsL[i] << " ";
    }
  cout << endl << "IO bursts list: ";
  for (int i = 0; i < ioBursts; i++)
    {
      cout << ioBurstsL[i] << " ";
    }
  cout << endl
       << "Priority: " << priority << endl
       << "Time Slice: " << timeSlice << endl;
}

// Prints some statistics about the process, after it has finished, containing the pid,
// turnaround time, total CPU time, total waiting time, percentage CPU utilization
void processType::printInfo()
{
  cout << "Process pid: " << pid << endl
       << "Turnaround time: " << getTurnAroundTime() << endl
       << "Total CPU time: " << totalCPU << endl
       << "Total Waiting time: " << getWaitingTime() << endl
       << "Percentage of CPU utilization time: " << getCpuUtilization() << endl << endl;

}

// Struct node for queue linked list
struct nodeType
{
  processType *process;
  nodeType *next;
};

class queue
{
public:
  queue();
  bool isEmptyList();
  bool front(processType**);
  bool insertStartUpQueue(processType*);
  bool insertAEQueue(processType*);
  bool insertIOQueue(processType*);
  bool insertFinishedQueue(processType*);
  bool deleteFront();
  void performIOQueue();
  void switchLists(queue&);
  void printOutput();
  void printI();
private:
  nodeType *first; // Points to the first node on the list
  nodeType *last; // Points to the last node on the list
};

// queue constructor initializes the list pointers to NULL
  queue::queue()
  {
    first = NULL;
    last = NULL;
  }

// Determines whether the list is empty and return teur is it is or false otherwise
bool queue::isEmptyList()
{
  if (first != NULL)
    return false;
  else
    return true;
}

// Returns the item at the front of the list
bool queue::front(processType **item)
{
  if (first == NULL)
    return false;
  else
    {
      *item = first->process;
      return true;
    }
}

// Inserts an item into the start up queue ordering it by its start time
bool queue::insertStartUpQueue(processType *item)
{
  nodeType *curr, *prev, *newNode;
  bool found = false;

  newNode = new nodeType;
  newNode->process = item;
  newNode->next = NULL;

  if (first == NULL)
    {
      first = newNode;
      last = newNode;
    }
  else
    {
      curr = first;
      while (curr != NULL && !found)
	      {
	        if (curr->process->getArrivalTime() > item->getArrivalTime())
	          { 
	            found = true;
	          }
	        else
	          {
	            prev = curr;
	            curr = curr->next;
	          }
	      }

      if (curr == first)
	      {
	        newNode->next = first;
	        first = newNode;
	      }
      else
	      {
	        prev->next = newNode;
	        newNode->next = curr;
	        if (curr == NULL)
	          last = newNode;
	      }
    }
}

// Inserts an item into the active or expired queue ordering it by its priority
bool queue::insertAEQueue(processType *item)
{
  nodeType *curr, *prev, *newNode;
  bool found = false;

  newNode = new nodeType;
  newNode->process = item;
  newNode->next = NULL;

  if (first == NULL)
    {
      first = newNode;
      last = newNode;
    }
  else
    {
      curr = first;
      while (curr != NULL && !found)
	      {
	        if (curr->process->getPriority() > item->getPriority())
	          {
	            found = true;
	          }
	        else
	          {
	            prev = curr;
	            curr = curr->next;
	          }
	      }

      if (curr == first)
	      {
	        newNode->next = first;
	        first = newNode;
	      }
      else
	      {
	        prev->next = newNode;
	        newNode->next = curr;
	        if (curr == NULL)
	          last = newNode;
	      }
    }
}

// Inserts an item into the IO queue ordering it by its current IO burst left
bool queue::insertIOQueue(processType *item)
{
  nodeType *curr, *prev, *newNode;
  bool found = false;

  newNode = new nodeType;
  newNode->process = item;
  newNode->next = NULL;

  if (first == NULL)
    {
      first = newNode;
      last = newNode;
    }
  else
    {
      curr = first;
      while (curr != NULL && !found)
	      {
	        if (curr->process->getIOBurstLeft() > item->getIOBurstLeft())
	          {
	            found = true;
	          }
	        else
	          {
	            prev = curr;
	            curr = curr->next;
	          }
	      }

      if (curr == first)
	      {
	        newNode->next = first;
	        first = newNode;
	      }
      else
	      {
	        prev->next = newNode;
	        newNode->next = curr;
	        if (curr == NULL)
	          last = newNode;
	      }
    }
}

// Inserts an item into the finished queue ordering it by its finish time
bool queue::insertFinishedQueue(processType *item)
{
  nodeType *curr, *prev, *newNode;
  bool found = false;

  newNode = new nodeType;
  newNode->process = item;
  newNode->next = NULL;

  if (first == NULL)
    {
      first = newNode;
      last = newNode;
    }
  else
    {
      curr = first;
      while (curr != NULL && !found)
	      {
	        if (curr->process->getFinishTime() > item->getFinishTime())
	          {
	            found = true;
	          }
	        else
	          {
	            prev = curr;
	            curr = curr->next;
	          }
	      }

      if (curr == first)
	      {
	        newNode->next = first;
	        first = newNode;
	      }
      else
	      {
	        prev->next = newNode;
	        newNode->next = curr;
	        if (curr == NULL)
	          last = newNode;
	      }
    }
}

// Deletes the item at the front of the queue
bool queue::deleteFront()
{
  nodeType *p;

  if (first != NULL)
    {
      if ( first == last)
        {
          delete first;
          first = NULL;
          last = NULL;
        }
      else
        {
          p = first;
          first = first->next;
          delete p;
        }
    }
}

// Calls the performIO function for each item in the current queue
void queue::performIOQueue()
{
  nodeType *curr;

  curr = first;

  while(curr != NULL)
    {
      curr->process->performIO();
      curr = curr->next;
    }
}

// Switches the contents of two lists 
void queue::switchLists(queue &otherQueue)
{
  nodeType *f, *l;
  
  f = first;
  l = last;
  first = otherQueue.first;
  last = otherQueue.last;
  otherQueue.first = f;
  otherQueue.last = l;
}

// When every process has finished it prints and overall performance output for each
// process and then an average performance output for all the process' in the queue
void queue::printOutput()
{
  int n = 0;
  nodeType *curr;
  double avgTAT = 0;
  double avgWT = 0;
  double avgCPU = 0;

  curr = first;

  cout << "\n----------------Overall Performance Output----------------" << endl;
  if (curr==NULL)
    cout << endl;
  
  cout << fixed << showpoint << setprecision(1);
  while(curr != NULL)
    {
      curr->process->printInfo();
      avgTAT = avgTAT + curr->process->getTurnAroundTime();
      avgWT = avgWT + curr->process->getWaitingTime();
      avgCPU = avgCPU + curr->process->getCpuUtilization();
      n++;
      curr = curr->next;
    }
    
  avgTAT = avgTAT / n;
  avgWT = avgWT/n;
  avgCPU = avgCPU/n;
  
  cout << fixed << showpoint << setprecision(3);
  cout << "Average Waiting Time: " << avgWT << endl
       << "Average Turnaround Time: " << avgTAT << endl
       << "Average CPU Utilization: " << avgCPU << endl;
}

// Prints some info regarding the current contents of the list
void queue::printI()
{
  nodeType *curr;
  int i = 1;

  curr = first;

  if (curr==NULL)
    cout << endl;

  while(curr != NULL)
    {
      cout << "---------------------------------------------" << endl;
      curr->process->printInf();
      curr = curr->next;
    }
}

void readInput(queue&);

int main()
{
  queue startUpQueue, activeQueue, expiredQueue, ioQueue, finishedQueue;
  int clock = 0;
  bool simulate = true;
  processType *currProcess;
  processType *cpu = NULL;

  readInput(startUpQueue);
  
  while (simulate)
    {
      // insert processes to the active queue if they are to start at this
      // clock tick. (calculate priority and timeslice).
      while(startUpQueue.front(&currProcess))
	      {
	        if (clock == currProcess->getArrivalTime())
	          {
	            currProcess->calcTimeSlice();
	            activeQueue.insertAEQueue(currProcess);
	            startUpQueue.deleteFront();
	            cout << "[" << clock << "] <" << currProcess->getPid() << "> Enters ready "
	                 << "queue (Priority: " << currProcess->getPriority() << ", Timeslice: "
	                 << currProcess->getTimeSlice() << ")" << endl;
	          }
	        else
	          break;
	      }
	      
	    // if the cpu is empty the lowest priority process in the active queue
	    // is put into the cpu.
	    if (cpu == NULL)
	      {
	        if(activeQueue.front(&cpu))
	          {
	            activeQueue.deleteFront();
	            cout << "[" << clock << "] <" << cpu->getPid() << "> Enters the CPU"
	                 << endl;
	          }
	      }
	    
	    
	    // check if the lowest priority process in the active queue has a lower 
	    // priority than the process in the cpu. If so then preempt.
	    // Preempted process goes back to the active queue.
	    if (activeQueue.front(&currProcess) && cpu!= NULL)
	      {
	        if (currProcess->getPriority() < cpu->getPriority())
	          {
	            activeQueue.deleteFront();
	            activeQueue.insertAEQueue(cpu);
	            cout << "[" << clock << "] <" << currProcess->getPid() << "> Preempts "
	                 << "Process " << cpu->getPid() << endl;
	            cpu = currProcess;
	          }
	      }
	    
	    // Perform cpu (decrement the timeslice and CPU burst of the process in the cpu)
	    if (cpu != NULL)
	      cpu->performCPU();
	    
	    // Perform IO (decrement the IO burst for all processes in the IO queue)
	    if (!ioQueue.isEmptyList())
	      {
	        ioQueue.performIOQueue();
	      }
	    
	    // if there is a process in the cpu (call this process p
	    if (cpu != NULL)
	      {
	        // if p's current cpu burst is exhausted
	        if (cpu->cpuBurstE())
	          {
	            // if p is done with all cpu bursts send to the finished queue
	            if (cpu->isFinished())
	              {
	                cpu->setFinishTime(clock);
	                finishedQueue.insertFinishedQueue(cpu);
	                cout << "[" << clock << "] <" << cpu->getPid() << "> Finishes "
	                     << "and moves to the Finished Queue" << endl;
	                cpu = NULL;
	              }	            
	            // if p is not done with all cpu bursts (which means there is an IO
	            // burst) send to the IO queue.
	            else
	              {
                  cout << "[" << clock << "] <" << cpu->getPid() << "> Moves to "
	                     << "the IO Queue" << endl;
	                ioQueue.insertIOQueue(cpu);
	                cpu = NULL;
	              }
	          }
	          
	        // if p's timeslice is exhausted send to the expired queue and
	        // recalculate priority and timeslice
	        else if (cpu->getTimeSlice() == 0)
	          {
	            cpu->calcPriority();
	            cpu->calcTimeSlice();
	            expiredQueue.insertAEQueue(cpu);
	            cout << "[" << clock << "] <" << cpu->getPid() << "> Finishes its "
	                 << "time slice and moves to the Expired" << endl << "\tQueue "
	                 << "(Priority: " << cpu->getPriority() << ", Timeslice: "
	                 << cpu->getTimeSlice() << ")" << endl;
	            cpu = NULL;
	          }
	      }
	    
	    // if there is any process in the IO queue that is finished with its IO
	    // burst (there can be more than one, call this process p)
	    while (ioQueue.front(&currProcess))
	      {
	        if (currProcess->getIOBurstLeft() == 0)
	          {
	            // If p's timeslice is exhausted move to the expired queue and
	            // recalculate the priority and timeslice
	            if (currProcess->getTimeSlice() == 0)
	              {
	                currProcess->calcPriority();
	                currProcess->calcTimeSlice();
	                expiredQueue.insertAEQueue(currProcess);
	                cout << "[" << clock << "] <" << currProcess->getPid() << "> Finishes IO "
	                     << "and moves to the Expired Queue( " << endl
	                     << "\tPriority: " << currProcess->getPriority() << ", Timeslice: "
	                     << currProcess->getTimeSlice() << ")" << endl;
	              }
	            // If p still has timeslice left insert p into the active queue
	            else
	              {
	                activeQueue.insertAEQueue(currProcess);
	                cout << "[" << clock << "] <" << currProcess->getPid() << "> Finishes IO "
	                     << "and moves to the Ready Queue" << endl;
	              }
	            ioQueue.deleteFront();
	          }
	        else
	          break;
	      }
	    
	    // if the ready queue and cpu are empty
	    if (cpu == NULL  && activeQueue.isEmptyList())
	      {
	      // if the expired queue is no empty then switch the expired and active 
	      // queues
	      if (!expiredQueue.isEmptyList())
	        {
	          expiredQueue.switchLists(activeQueue);
	          cout << "[" << clock << "] *** Queue Swap" << endl;
	        }
	      
	      // if the startup queue, expired queue, IO queue are all empty then break
	      // out of the while loop (the simulation is complete)
	      else if (startUpQueue.isEmptyList() && ioQueue.isEmptyList())
	        {
	          simulate = false;
	          break;
	        }
        }
      
      // Increment the clock
      clock++;
    }
    
  finishedQueue.printOutput();

  return 0;
}

// Read input function reads the values in a file containing the info of the processes 
// to simulate and dinamically creates a new process and adds it to the start up queue
void readInput(queue &startUpQueue)
{
  int niceValue;
  int arrivalTime;
  int cpuBursts;
  int* cpuBurstsL;
  int* ioBurstsL;
  string nice;
  processType *process;
  int pid = 0;
  
  cin >> niceValue;
  cin >> arrivalTime;
  cin >> cpuBursts;
  cpuBurstsL = new int[cpuBursts];
  ioBurstsL = new int[cpuBursts-1];
  cin >> cpuBurstsL[0];
  for (int i = 0; i < (cpuBursts-1); i++)
    {
      cin >> ioBurstsL[i];
      cin >> cpuBurstsL[i+1];
    }
  process = new processType(pid, niceValue, arrivalTime, cpuBursts, cpuBurstsL, ioBurstsL);
  startUpQueue.insertStartUpQueue(process);
  pid++;
  delete ioBurstsL;
  delete cpuBurstsL;
  cin >> nice;

  while (nice != "***")
    {
      niceValue = atoi(nice.c_str());
      cin >> arrivalTime;
      cin >> cpuBursts;
      cpuBurstsL = new int[cpuBursts];
      ioBurstsL = new int[cpuBursts-1];
      cin >> cpuBurstsL[0];
      for (int i = 0; i < (cpuBursts-1); i++)
	      {
	        cin >> ioBurstsL[i];
	        cin >> cpuBurstsL[i+1];
	      }
      process = new processType(pid, niceValue, arrivalTime, cpuBursts, cpuBurstsL, ioBurstsL);
      startUpQueue.insertStartUpQueue(process);
      pid++;
      delete ioBurstsL;
      delete cpuBurstsL;
      cin >> nice;
    }
}
