/*
 * File: serial_read.c
 * Author: Ian Booth
 * Date: 2026/03/29
 * Description: Reads button and potentiometer data from Arduino,
 * stores the most recent values, and writes them to inputs.txt
 * only when the previous file has been removed.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#define SERIAL_PORT "/dev/ttyUSB0"
#define BAUD B9600
#define BUF_SIZE 256
#define INPUT_FILE "inputs.txt"
#define TENBIT 1023.0

/*
 * Description: Stores one complete set of input values
 */
typedef struct
{
    int b1;
    int b2;
    int b3;
    double p1;
    double p2;
} InputSet;

/*
 * Description: Opens and configures the serial port used to
 * communicate with the Arduino.
 * @param portName: serial device path
 * @return: file descriptor for serial port, or -1 on error
 */
int setupSerial(const char *portName);

/*
 * Description: Attempts to write the most recent input set
 * to inputs.txt if the file does not already exist.
 * @param latestInput: pointer to newest input values
 * @param hasPendingInput: flag indicating new data is available
 * @return: none
 * side effects: may create inputs.txt and clear pending flag
 */
void tryWriteLatestInput(const InputSet *latestInput, int *hasPendingInput);

int main(void)
{
    int serialFd;
    char buf[BUF_SIZE];
    int index = 0;
    char ch;

    int rawP1, rawP2;
    InputSet latestInput;

    int hasPendingInput = 0; // 1 = new data ready to write

    serialFd = setupSerial(SERIAL_PORT); //initialize serial port
    if (serialFd < 0)
    {
        return 1;
    }

    while (1)
    {
        // Read serial input one character at a time
        if (read(serialFd, &ch, 1) > 0)
        {
            if (ch == '\n') //end of line means got full line of data
            {
                buf[index] = '\0';
                index = 0;
                
                // Parse incoming CSV line
                if (sscanf(buf, "%d,%d,%d,%d,%d",
                           &latestInput.b1,
                           &latestInput.b2,
                           &latestInput.b3,
                           &rawP1,
                           &rawP2) == 5)
                {
                    // Scale potentiometer values
                    latestInput.p1 = rawP1 / TENBIT;
                    latestInput.p2 = rawP2 / TENBIT;

                    hasPendingInput = 1; // store latest data
                }
            }
            else
            {
                if (ch != '\r' && index < BUF_SIZE - 1) //stores character in buffer
                {
                    buf[index++] = ch;
                }
            }
        }

        // Only writes when file is gone, never blocks
        tryWriteLatestInput(&latestInput, &hasPendingInput);
    }

    close(serialFd);
    return 0;
}

/*
* Description: Configures serial port settings (9600 baud, 8N1)
* @param portName: path to serial device
* @return: file descriptor or -1 on failure
* side effects: modifies terminal I/O settings
*/
int setupSerial(const char *portName)
{
    int serialFd;
    struct termios options;

    serialFd = open(portName, O_RDWR | O_NOCTTY); //open serial port for reading and writing
    if (serialFd == -1)
    {
        perror("Could not open serial port");
        return -1;
    }

    tcgetattr(serialFd, &options); //get current port settings

    cfsetispeed(&options, BAUD); //set input and output baud rate
    cfsetospeed(&options, BAUD);

    options.c_cflag |= (CLOCAL | CREAD); //configure port: enable receiver and set local mode
    
    options.c_cflag &= ~PARENB; //display parity and stop bits, set 8 data bits
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    options.c_lflag = 0; //set raw input/output mode
    options.c_oflag = 0;
    options.c_iflag = 0;

    //configure read behavior:
    // Vmin =1, wait for at least 1 character
    //vtime = 0, no timeout
    options.c_cc[VMIN] = 1;
    options.c_cc[VTIME] = 0;

    tcflush(serialFd, TCIFLUSH); //clear existing data in buffer
    tcsetattr(serialFd, TCSANOW, &options); //apply new setting  immediately

    return serialFd;
}

/*
* Description: Writes latest input to file if previous file is gone
* @param latestInput: pointer to newest input values
* @param hasPendingInput: pointer to pending flag
* @return: none
* side effects: creates inputs.txt and resets pending flag
*/
void tryWriteLatestInput(const InputSet *latestInput, int *hasPendingInput)
{
    FILE *file;

    if (*hasPendingInput == 0) //if not new data, do nothing
        return; // nothing new

    if (access(INPUT_FILE, F_OK) == 0) //do not overwrite file if physics has not read it
        return; // old file still exists

    file = fopen(INPUT_FILE, "w"); //open file for writing new input data
    if (file == NULL)
    {
        perror("Could not open inputs.txt");
        return;
    }
    
    //write values in space seperated format for physics
    fprintf(file, "%d %d %d %.3f %.3f\n",
            latestInput->b1,
            latestInput->b2,
            latestInput->b3,
            latestInput->p1,
            latestInput->p2);

    fclose(file);
    
    *hasPendingInput = 0; // mark as written
}
