#include "ComPort.h"

void ComPort::read() {

    int i;
    int n;
    int cport_nr = 3;          /* /dev/ttyS0 (COM1 on windows) */
    int bdrate = 256000;       /* 9600 baud */

    byte buf[4096];

    char mode[] = { '8','N','1',0 };


    if (RS232_OpenComport(cport_nr, bdrate, mode, 0)) {
        printf("Can not open comport\n");

        return;
    }

    int count = 1;
    byte part1 = 0;
    byte part2 = 0;

    bool start = false;

    float* outputBuf = new float[8 * 1024];
    int outputBufferCount = 0;

    while (true) {
        n = RS232_PollComport(cport_nr, buf, 4095);

        if (n > 0) {
            for (i = 0; i < n; i++) {

                if (start) {
                    if (count % 2 != 0) {
                        part1 = buf[i];
                    }
                    else {
                        part2 = buf[i];

                        short j;
                        j = part2;
                        j <<= 8;
                        j |= part1;

                        start = false;
                        float signal = j;

                        outputBuf[outputBufferCount] = signal;
                        outputBufferCount++;
                        printf("%f\n", signal);
                    }
                    count++;
                } else {
                    if (buf[i] == (byte)(-1)) {
                        start = true;
                    }
                }
            }

            circleBuffer->write(outputBuf, outputBufferCount);
            outputBufferCount = 0;
            //printf("received %i bytes\n", n);
        } else {
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
    }

}

std::thread ComPort::start() {
    std::thread t(&ComPort::read, this);
    return t;
}