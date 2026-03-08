/*
 * File: serial_read.c
 * Author: Ian Booth
 * Date: 2026/03/08
 * Description: This program reads button and potentiometer data
 * sent from an Arduino Nano over USB serial. The Arduino sends
 * comma-separated values representing three button states and
 * two potentiometer readings. The Raspberry Pi reads each line,
 * parses the values, and prints simple status messages.
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

/*
 * Description: Opens and configures the serial port used to
 * communicate with the Arduino.
 * @param portName: serial device path (ex: /dev/ttyUSB0)
 * @return: file descriptor for the serial port, or -1 on error
 * side effects: configures serial communication settings
 */
int setupSerial(const char *portName);

/*
 * Description: Processes parsed button and potentiometer data
 * and prints simple status messages.
 * @param b1: button 1 state
 * @param b2: button 2 state
 * @param b3: button 3 state
 * @param p1: potentiometer 1 value
 * @param p2: potentiometer 2 value
 * @return: none
 * side effects: prints information to the terminal
 */
void handleInput(int b1, int b2, int b3, int p1, int p2);

/*
 * Description: Main program loop. Opens the serial port,
 * reads incoming characters, rebuilds lines of data from
 * the Arduino, and parses the values into variables.
 * @param: none
 * @return: returns 0 if program exits normally
 * side effects: continuously reads serial data
 */
int main(void) {

    int fd;
    char buf[BUF_SIZE];
    int index = 0;
    char ch;

    int b1, b2, b3, p1, p2;

    fd = setupSerial(SERIAL_PORT);
    if (fd < 0) {
        return 1;
    }

    printf("Listening on %s\n", SERIAL_PORT);

    while (1) {

        /* Read one character at a time from the serial port */
        if (read(fd, &ch, 1) > 0) {

            /* When a newline arrives we have a full message */
            if (ch == '\n') {

                buf[index] = '\0';
                index = 0;

                /* Parse comma-separated values */
                if (sscanf(buf, "%d,%d,%d,%d,%d", &b1, &b2, &b3, &p1, &p2) == 5) {
                    /* Scale potentiometer values from 0–1023 to 0–100 */
    p1 = (p1 * 100) / 1023;
    p2 = (p2 * 100) / 1023;
                    handleInput(b1, b2, b3, p1, p2);
                }

            } else {

                /* Store characters until newline */
                if (ch != '\r' && index < BUF_SIZE - 1) {
                    buf[index++] = ch;
                }
            }
        }
    }

    close(fd);
    return 0;
}

/*
 * Description: Initializes the serial port and sets the
 * communication parameters.
 */
int setupSerial(const char *portName) {

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

/*
 * Description: Displays the current input values and reacts
 * to button presses.
 */
void handleInput(int b1, int b2, int b3, int p1, int p2) {

    printf("B1=%d B2=%d B3=%d P1=%d P2=%d\n", b1, b2, b3, p1, p2);
    printf("\n");
}
