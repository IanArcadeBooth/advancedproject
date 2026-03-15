#include <time.h>
#include <stdio.h>

const double gravity = 1.6;
const double maxThrust = 2.98; 
const double sideThrust = 0.116;
const double nsMultiplier = 0.000000001;

double deltaT(struct timespec*, struct timespec*);

int main()
{
    // Setup initial conditions of variables
    double xVel = 1.6;
    double yVel = 0;
    double xPos = 0;
    double yPos = 100;
    double thrust = 0; // Range 0 - 1 for power of main thruster
    int sideThrust = 0; // -1 if right thrusters on, or +1 if left thrusters on
    double simTime = 0; // Keeps track of how long sim has been running (just for debugging atm)
    
    // Setup structures for time tracking            
    struct timespec T1, T2;
    clock_gettime(CLOCK_MONOTONIC, &T1);

    while (yPos > 0)
    {
        clock_gettime(CLOCK_MONOTONIC, &T2); // Get current time
        double deltaTime = deltaT(&T1, &T2); // Calculate time since last here
        clock_gettime(CLOCK_MONOTONIC, &T1); // Save time last here
        simTime += deltaTime;

        // Update position and velocity
        xPos = xPos + (xVel * deltaTime);
        yPos = yPos + (yVel * deltaTime);
        yVel = yVel - (gravity * deltaTime) + (maxThrust * thrust * deltaTime);
        xVel += sideThrust * deltaTime;

        printf("Position: (%lf, %lf), Velocity: (%lf, %lf), Time: %lf\n", xPos, yPos, xVel, yVel, simTime); 
    }
}

//Returns the difference between two times in seconds
double deltaT(struct timespec* t1, struct timespec* t2)
{
    return  (t2->tv_sec - t1->tv_sec) + (t2->tv_nsec - t1->tv_nsec) * nsMultiplier;
}
