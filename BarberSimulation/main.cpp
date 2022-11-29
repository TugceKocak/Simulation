#include <iostream>
#include <queue>
#include <random>
#include <time.h>
#include "VariadicTable.h"
#define CUSTOMER_COUNT 10


using namespace std;

VariadicTable<int, int, int, int, int, int, int, int> customersInfos({"Customer","Arrival Time", "Service Time", "Start Time", "Finished Time", "Queue Size","Waiting time","Idle Time"});
//customer
class Customer{
public:
    int timeOfArrivalTheBarber;
    int timeOfStarting;
    int timeOfFinishing;
    int serviceTime;
    int waitingTime;

    void setCustomer(int arrivalTime, int serviceTime){
        this->timeOfArrivalTheBarber       = arrivalTime;
        this->serviceTime                  = serviceTime;
    }
};

class Barber{
public:
    bool idle = true;
    Customer currentCustomer;
    queue<Customer> queueOfCustomers;
    int customer_number=1;
    struct statistics_t{
        int averageWaitingTimeForCustomers = 0;
        int averageIdleTimeForBarber       = 0;
        int averageQueueLength             = 0;
        int maxWaitingTimeForCustomers 	   = 0;
        int maxIdleTimeForBarber           = 0;
        int maxQueueLength                 = 0;
    } statistics;

    bool availableToService(Customer customer, int newArrival) {
    	// If the barber is idle or there is no waiting in the queue, the new customer gets service
        if(idle || queueOfCustomers.size() == 0){
            currentCustomer = customer;
            currentCustomer.timeOfStarting    = currentCustomer.timeOfArrivalTheBarber;
            currentCustomer.timeOfFinishing   = currentCustomer.timeOfStarting + currentCustomer.serviceTime;
            queueOfCustomers.pop();
            idle = false;
            customersInfos.addRow(
            		this->customer_number,
                    currentCustomer.timeOfArrivalTheBarber,
                    currentCustomer.serviceTime,
                    currentCustomer.timeOfStarting,
                    currentCustomer.timeOfFinishing,
					0, 		//queue size
					0,		//waiting time
					currentCustomer.timeOfArrivalTheBarber);
			statistics.maxIdleTimeForBarber=currentCustomer.timeOfArrivalTheBarber;
            this->customer_number= this->customer_number+1;
            return idle;
        }
        else if(currentCustomer.timeOfFinishing <= newArrival){
            if(customer.timeOfArrivalTheBarber > currentCustomer.timeOfFinishing){  
                customer.timeOfStarting   = customer.timeOfArrivalTheBarber;
                customer.timeOfFinishing  = customer.timeOfStarting + customer.serviceTime;
                statistics.averageIdleTimeForBarber += (customer.timeOfArrivalTheBarber - currentCustomer.timeOfFinishing);
            }
            else{
                customer.timeOfStarting   = currentCustomer.timeOfFinishing;
                customer.timeOfFinishing  = customer.timeOfStarting + customer.serviceTime;
                customer.waitingTime=customer.timeOfStarting - customer.timeOfArrivalTheBarber;
                
                statistics.averageWaitingTimeForCustomers += customer.waitingTime;
                if(statistics.maxWaitingTimeForCustomers < customer.waitingTime){
                	statistics.maxWaitingTimeForCustomers=customer.waitingTime;
				}
            }
            int idle_time=customer.timeOfArrivalTheBarber - currentCustomer.timeOfFinishing;
            if(idle_time<0)		idle_time=0;
            if(statistics.maxIdleTimeForBarber<idle_time)  statistics.maxIdleTimeForBarber=idle_time;
			
            currentCustomer = customer;
            queueOfCustomers.pop();
            statistics.averageQueueLength+= this->queueOfCustomers.size();
            if(statistics.maxQueueLength<this->queueOfCustomers.size())	
				statistics.maxQueueLength=this->queueOfCustomers.size();
            idle = false;
            int queueSize=queueOfCustomers.size();
            if(queueSize>2) queueSize-=1;
            customersInfos.addRow(
            		this->customer_number,
                    currentCustomer.timeOfArrivalTheBarber,
                    currentCustomer.serviceTime,
                    currentCustomer.timeOfStarting,
                    currentCustomer.timeOfFinishing,
                    queueSize,
                    customer.waitingTime,
					idle_time);
            this->customer_number= this->customer_number+1;
            return idle;
        }
        return false;
    }
};

int main() {
	srand(time(NULL));
    Barber barber;

    for(int i = 0; i < CUSTOMER_COUNT; i++){
        Customer customer;
        // If the customer is the first to arrive, the interarrival time must be equal to the arrival time.
        if(i == 0){
		      int arrival	=rand()%20+5; 	//ta=(5,25)
		      int service	=rand()%20+15; 	//ts=(15,35)
            customer.setCustomer(arrival, service);
            barber.queueOfCustomers.push(customer);
        }
        // If it is not the first customer to arrive, 
		// the arrival time must be equal to the sum of the previous customer's arrival time and the interarrival time.
        else{
            int arrival = barber.queueOfCustomers.back().timeOfArrivalTheBarber + rand()%20+5;;
            int service = rand()%20+15;  				//ta=(5,25)
            customer.setCustomer(arrival, service); 	//ts=(15,35)
            barber.queueOfCustomers.push(customer);
        }
        barber.availableToService(barber.queueOfCustomers.front(), barber.queueOfCustomers.back().timeOfArrivalTheBarber);
    }
    while(!barber.queueOfCustomers.empty()){
        barber.availableToService(barber.queueOfCustomers.front(), barber.currentCustomer.timeOfFinishing);
    }

    customersInfos.print(cout);

    VariadicTable<double, double, double> statistics1Vt({"AverageIdleTimeForBarber", "AverageWaitingTimeForCustomer", "AverageQueueLength"});
    statistics1Vt.addRow((double) barber.statistics.averageIdleTimeForBarber / CUSTOMER_COUNT,
                        (double) barber.statistics.averageWaitingTimeForCustomers / CUSTOMER_COUNT,
                        (double) barber.statistics.averageQueueLength / CUSTOMER_COUNT);
    statistics1Vt.print(cout);
    
	
	VariadicTable<double, double, double> statistics2Vt({"MaxIdleTimeForBarber", "MaxWaitingTimeForCustomer", "MaxQueueLength"});
    statistics2Vt.addRow((double) barber.statistics.maxIdleTimeForBarber,
                        (double) barber.statistics.maxWaitingTimeForCustomers,
                        (double) barber.statistics.maxQueueLength);
    statistics2Vt.print(cout);
    return 0;
}
