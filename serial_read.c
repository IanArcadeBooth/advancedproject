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

    serialFd = setupSerial(SERIAL_PORT);
    if (serialFd < 0)
    {
        return 1;
    }

    while (1)
    {
        // Read serial input one character at a time
        if (read(serialFd, &ch, 1) > 0)
        {
            if (ch == '\n')
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
                    latestInput.p1 = rawP1 / 1023.0;
                    latestInput.p2 = rawP2 / 1023.0;

                    hasPendingInput = 1; // store latest data
                }
            }
            else
            {
                if (ch != '\r' && index < BUF_SIZE - 1)
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

int setupSerial(const char *portName)
{
    int serialFd;
    struct termios options;

    serialFd = open(portName, O_RDWR | O_NOCTTY);
    if (serialFd == -1)
    {
        perror("Could not open serial port");
        return -1;
    }

    tcgetattr(serialFd, &options);

    cfsetispeed(&options, BAUD);
    cfsetospeed(&options, BAUD);

    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    options.c_lflag = 0;
    options.c_oflag = 0;
    options.c_iflag = 0;

    options.c_cc[VMIN] = 1;
    options.c_cc[VTIME] = 0;

    tcflush(serialFd, TCIFLUSH);
    tcsetattr(serialFd, TCSANOW, &options);

    return serialFd;
}

void tryWriteLatestInput(const InputSet *latestInput, int *hasPendingInput)
{
    FILE *file;

    if (*hasPendingInput == 0)
        return; // nothing new

    if (access(INPUT_FILE, F_OK) == 0)
        return; // old file still exists

    file = fopen(INPUT_FILE, "w");
    if (file == NULL)
    {
        perror("Could not open inputs.txt");
        return;
    }

    fprintf(file, "%d %d %d %.3f %.3f\n",
            latestInput->b1,
            latestInput->b2,
            latestInput->b3,
            latestInput->p1,
            latestInput->p2);

    fclose(file);
    

    *hasPendingInput = 0; // mark as written
}
