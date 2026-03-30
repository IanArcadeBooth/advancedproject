#include <time.h>
#include <stdio.h>

const double gravity = 1.6;
const double maxThrust = 2.98; 
const double sideThrust = 0.116;
const double nsMultiplier = 0.000000001;

double deltaT(struct timespec*, struct timespec*);

typedef struct BUTTONS
{
    int start;
    int left;
    int right;
    double thrust;
    double landZone;
}Buttons;

int main()
{

    // Setup initial conditions of variables
    double xVel = 1.6;
    double yVel = 0;
    double xPos = 0;
    double yPos = 100;
    int sideThrust = 0; // -1 if right thrusters on, or +1 if left thrusters on
    double deltaTime = 0; // Time between frames
    double simTime = 0; // Keeps track of how long sim has been running (just for debugging atm)
    
    // Setup structures for time tracking            
    struct timespec T1, T2;
    
    FILE* fp;
    Buttons inputs;
    inputs.start = 1;    
 
    while (inputs.start)
    {
        while ((fp = fopen("inputs.txt", "r")) == NULL){} // Wait until file successfully opened
        fscanf(fp, "%d %d %d %lf %lf", &inputs.start, &inputs.left, &inputs.right, &inputs.thrust, &inputs.landZone);
        fclose(fp);
        remove("inputs.txt");
    }    
   
    clock_gettime(CLOCK_MONOTONIC, &T1);
    // Main physics loop, runs until land/crash
    while (yPos > 0)
    {
        // Read Inputs
        while ((fp = fopen("inputs.txt", "r")) == NULL){} // Wait until file successfully opened
        fscanf(fp, "%d %d %d %lf %lf", &inputs.start, &inputs.left, &inputs.right, &inputs.thrust, &inputs.landZone); 
        sideThrust = (inputs.left - inputs.right);
        fclose(fp);
        remove("inputs.txt");

        clock_gettime(CLOCK_MONOTONIC, &T2); // Get current time
        deltaTime = deltaT(&T1, &T2); // Calculate time since last here
        clock_gettime(CLOCK_MONOTONIC, &T1); // Save time last here
        simTime += deltaTime;

        // Update position and velocity
        xPos = xPos + (xVel * deltaTime);
        yPos = yPos + (yVel * deltaTime);
        yVel = yVel - (gravity * deltaTime) + (maxThrust * inputs.thrust * deltaTime);
        xVel += sideThrust * deltaTime;
        
        printf("Position: (%lf, %lf), Velocity: (%lf, %lf), Time: %lf\n", xPos, yPos, xVel, yVel, simTime); 
    }
}

//Returns the difference between two times in seconds
double deltaT(struct timespec* t1, struct timespec* t2)
{
    return  (t2->tv_sec - t1->tv_sec) + (t2->tv_nsec - t1->tv_nsec) * nsMultiplier;
}
