/*
 * File: serial_read.c
 * Author: Ian Booth
 * Date: 2026/03/08
 * Description: This program reads button and potentiometer data
 * sent from an Arduino Nano over USB serial. The Arduino sends
 * comma-separated values representing four button states and
 * two potentiometer readings. The Raspberry Pi reads each line,
 * parses the values, scales the potentiometer values from 0.0
 * to 1.0, and writes them to inputs.txt when that file does
 * not already exist.
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
 * Description: Opens and configures the serial port used to
 * communicate with the Arduino.
 * @param portName: serial device path
 * @return: file descriptor for serial port, or -1 on error
 */
int setupSerial(const char *portName);

/*
 * Description: Writes one complete set of button and potentiometer
 * values to inputs.txt if the file does not already exist.
 * @param b1: button 1 state
 * @param b2: button 2 state
 * @param b3: button 3 state
 * @param b4: button 4 state
 * @param p1: potentiometer 1 value from 0.0 to 1.0
 * @param p2: potentiometer 2 value from 0.0 to 1.0
 * @return: none
 */
void handleInput(int b1, int b2, int b3, int b4, double p1, double p2);

int main(void)
{
    int fd;
    char buf[BUF_SIZE];
    int index = 0;
    char ch;

    int b1, b2, b3, b4;
    int rawP1, rawP2;
    double p1, p2;

    fd = setupSerial(SERIAL_PORT);
    if (fd < 0) {
        return 1;
    }

    while (1) {
        if (read(fd, &ch, 1) > 0) {

            if (ch == '\n') {
                buf[index] = '\0';
                index = 0;

                if (sscanf(buf, "%d,%d,%d,%d,%d,%d",
                           &b1, &b2, &b3, &b4, &rawP1, &rawP2) == 6) {

                    p1 = rawP1 / 1023.0;
                    p2 = rawP2 / 1023.0;

                    handleInput(b1, b2, b3, b4, p1, p2);
                }
            }
            else {
                if (ch != '\r' && index < BUF_SIZE - 1) {
                    buf[index++] = ch;
                }
            }
        }
    }

    close(fd);
    return 0;
}

int setupSerial(const char *portName)
{
    int fd;
    struct termios options;

    fd = open(portName, O_RDWR | O_NOCTTY);
    if (fd == -1) {
        perror("Could not open serial port");
        return -1;
    }

    tcgetattr(fd, &options);

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

    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &options);

    return fd;
}

void handleInput(int b1, int b2, int b3, int b4, double p1, double p2)
{
    FILE *file;

    //Wait until physics has deleted the old input file
    while (access(INPUT_FILE, F_OK) == 0) {
        usleep(5000);
    }

    file = fopen(INPUT_FILE, "w");
    if (file == NULL) {
        perror("Could not open inputs.txt");
        return;
    }

    fprintf(file, "%d %d %d %d %.3f %.3f\n", b1, b2, b3, b4, p1, p2);
    fclose(file);
}
