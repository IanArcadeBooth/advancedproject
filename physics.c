#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include "lunar_display.h"

const double gravity = 1.6;
const double maxThrust = 2.98; 
const double sideThrust = 0.116;
const double nsMultiplier = 0.000000001;

typedef struct BUTTONS
{
    int start;
    int left;
    int right;
    double thrust;
    double landZone;
}Buttons;

void writeInfo(FILE*, Buttons*, double, double, double, double);
double deltaT(struct timespec*, struct timespec*);

int main()
{
    // Setup structures for time tracking            
    struct timespec T1, T2;
    
    FILE* fp;
    Buttons inputs;

    while (1)
    {
        // Setup initial conditions of variables
        double xVel = 1.6;
        double yVel = 0;
        double xPos = 0;
        double yPos = 100;
        int sideThrustState = 0; // -1 if right thrusters on, or +1 if left thrusters on
        double deltaTime = 0; // Time between frames
        double simTime = 0; // Keeps track of how long sim has been running (just for debugging atm)
        double fixedLandZone = 0;
        
        inputs.start = 1;    
     
        while (inputs.start)
        {
            while ((fp = fopen("inputs.txt", "r")) == NULL)
            {
                usleep(10000);
            } // Wait until file successfully opened

            fscanf(fp, "%d %d %d %lf %lf", &inputs.start, &inputs.left, &inputs.right, &inputs.thrust, &inputs.landZone);
            fclose(fp);
            remove("inputs.txt");

            fixedLandZone = inputs.landZone;
            writeInfo(fp, &inputs, xPos, yPos, xVel, yVel);
        }    
       
        clock_gettime(CLOCK_MONOTONIC, &T1);

        // Main physics loop, runs until land/crash
        while (yPos > SURFACE_Y + LANDER_R)
        {
            // Read Inputs
            while ((fp = fopen("inputs.txt", "r")) == NULL)
            {
                usleep(10000);
            } // Wait until file successfully opened

            fscanf(fp, "%d %d %d %lf %lf", &inputs.start, &inputs.left, &inputs.right, &inputs.thrust, &inputs.landZone);
            fclose(fp);
            remove("inputs.txt");

            inputs.landZone = fixedLandZone; 
            sideThrustState = (inputs.left - inputs.right);

            clock_gettime(CLOCK_MONOTONIC, &T2); // Get current time
            deltaTime = deltaT(&T1, &T2); // Calculate time since last here
            clock_gettime(CLOCK_MONOTONIC, &T1); // Save time last here
            simTime += deltaTime;

            // Update position and velocity
            xPos = xPos + (xVel * deltaTime);
            yPos = yPos + (yVel * deltaTime);
            yVel = yVel - (gravity * deltaTime) + (maxThrust * inputs.thrust * deltaTime);
            xVel += sideThrustState * deltaTime;
            
            if (yPos <= SURFACE_Y + LANDER_R)
            {
                yPos = SURFACE_Y + LANDER_R;
                
                while (access("rocketInfo.txt", F_OK) == 0)
                {
                    usleep(10000);
                }
                writeInfo(fp, &inputs, xPos, yPos, xVel, yVel);
                break;
            }
            
            writeInfo(fp, &inputs, xPos, yPos, xVel, yVel); 
        }

                while (inputs.start == 0)
        {
            while ((fp = fopen("inputs.txt", "r")) == NULL)
            {
                usleep(10000);
            }

            fscanf(fp, "%d %d %d %lf %lf", &inputs.start, &inputs.left, &inputs.right, &inputs.thrust, &inputs.landZone);
            fclose(fp);
            remove("inputs.txt");
        }

        while (inputs.start == 1)
        {
            while ((fp = fopen("inputs.txt", "r")) == NULL)
            {
                usleep(10000);
            }

            fscanf(fp, "%d %d %d %lf %lf", &inputs.start, &inputs.left, &inputs.right, &inputs.thrust, &inputs.landZone);
            fclose(fp);
            remove("inputs.txt");
        }

        while (inputs.start == 0)
        {
            while ((fp = fopen("inputs.txt", "r")) == NULL)
            {
                usleep(10000);
            }

            fscanf(fp, "%d %d %d %lf %lf", &inputs.start, &inputs.left, &inputs.right, &inputs.thrust, &inputs.landZone);
            fclose(fp);
            remove("inputs.txt");
        }
    }
}

void writeInfo(FILE* fp, Buttons* inputs, double xPos, double yPos, double xVel, double yVel)
{
    if (access("rocketInfo.txt", F_OK) == 0)
    {
        
        return; // Previous info has not yet been read
    }
    fp = fopen("rocketInfo.txt", "w");
    fprintf(fp, "%lf %lf %lf %lf %lf %lf", xPos, yPos, xVel, yVel, inputs->thrust, inputs->landZone);
    fclose(fp);
}

//Returns the difference between two times in seconds
double deltaT(struct timespec* t1, struct timespec* t2)
{
    return  (t2->tv_sec - t1->tv_sec) + (t2->tv_nsec - t1->tv_nsec) * nsMultiplier;
}
