
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/sco.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringSerial.h>

#include <blueteeth.hh>
#include <cereal.hh>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <string>

int main() {
    Blueteeth bt;
    Cereal c;

    bt.start();
    c.start();
    c.join();
    bt.join();
    return 0;
}

int t(int argc, char **argv) {
    char buf[1024] = {0};
    int bluetoothSocket, client, bytes_read;
    struct sockaddr_rc loc_addr = {0};
    struct sockaddr_rc client_addr = {0};

    socklen_t opt = sizeof(client_addr);

    bdaddr_t my_bdaddr_any = {0, 0, 0, 0, 0, 0};

    bluetoothSocket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    loc_addr.rc_family = AF_BLUETOOTH;
    loc_addr.rc_bdaddr = (bdaddr_t)my_bdaddr_any;
    loc_addr.rc_channel = (uint8_t)1;

    int ret = -1;

    if ((ret = bind(bluetoothSocket, (struct sockaddr *)&loc_addr,
                    sizeof(loc_addr))) == -1) {
        printf("Bluetooth bind failed. ERRNO=%d\n", errno);
        char *errorMessage = strerror_r(errno, buf, 1024);
        printf("%s\n", errorMessage);
        return 0;
    }

    ret = -1;

    if ((ret = listen(bluetoothSocket, 1)) == -1) {
        printf("Bluetooth listen failed. ERRNO=%d\n", errno);
        char *errorMessage = strerror_r(errno, buf, 1024);
        printf("%s\n", errorMessage);
        return 0;
    }
    bool stopThread = false;

    printf("Waiting for new client to connect.\n");

    client = accept(bluetoothSocket, (struct sockaddr *)&client_addr, &opt);

    if (client == -1) {
        printf("Bluetooth connect failed.\n");
        close(client);
        usleep(1000000);
        // delay(1000);
        return 0;
    }

    ba2str(&loc_addr.rc_bdaddr, buf);
    fprintf(stderr, "accepted connection from %s\n", buf);
    memset(buf, 0, sizeof(buf));

    printf("Bluetooth connection made.\n");

    while (stopThread == false) {
        // read data from the client
        bytes_read = read(client, buf, sizeof(buf));
        if (bytes_read > 0) {
            std::cout << buf << std::endl;
        }
    }

    close(client);
    close(bluetoothSocket);
    return 0;
}
