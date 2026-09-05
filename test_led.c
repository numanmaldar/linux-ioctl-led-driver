#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "ioctl_led.h"

int main(void) {
    int fd, state;

    fd = open("/dev/myled", O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    printf("Turning LED ON...\n");
    ioctl(fd, LED_ON);

    ioctl(fd, LED_GET, &state);
    printf("LED state: %d\n", state);

    printf("Turning LED OFF...\n");
    ioctl(fd, LED_OFF);

    ioctl(fd, LED_GET, &state);
    printf("LED state: %d\n", state);

    close(fd);
    return 0;
}
