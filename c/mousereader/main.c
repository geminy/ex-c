#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include <linux/input.h>

#define MOUSE_DEVICE "/dev/input/mouse0"
#define MOUSE_DEVICE2 "/dev/input/mice"
#define MOUSE_FRAME_LENGTH 3

int main(int argc, char **argv)
{
    printf("%s begins...\n", argv[0]);
    int fd = open(MOUSE_DEVICE, O_RDONLY);
    if (fd == -1) {
        printf("open %s error: %s\n", MOUSE_DEVICE, strerror(errno));
        return -1;
    }
    unsigned char data[MOUSE_FRAME_LENGTH] = { 0 };
    int size = 0;
    while (1) {
        size = read(fd, data, MOUSE_FRAME_LENGTH);
        if (size == -1) {
            printf("read %s error: %s\n", MOUSE_DEVICE, strerror(errno));
            close(fd);
            return -1;
        }
        else if (size == MOUSE_FRAME_LENGTH) {
            printf("%d %02x%02x%02x", size, data[0], data[1], data[2]);
            int i;
            for (i = 0; i < MOUSE_FRAME_LENGTH; ++i) {
                unsigned char d = data[i];
                int mask = 7;
                printf(" ");
                while (mask >= 0) {
                    printf("%d", (d & (1 << mask)) >> mask);
                    --mask;
                }
            }
            printf("\n");
        }
        else {
            printf("%d unknown error\n", size);
        }
    }
    close(fd);

    return 0;
}

/*
 * /dev/input/mouse0
 *
 * 1 frame = 3 bytes
 *
 * ^y
 * |
 * |
 * ---->x
 *
 * B1b7: 0
 * B1b6: 0
 * B1b5: mouse move direction of y 0-up    1-down
 * B1b4: mouse move direction of x 0-right 1-left
 * B1b3: 1
 * B1b2: middle key state 0-up 1-down
 * B1b1: right  key state 0-up 1-down
 * B1b0: left   key state 0-up 1-down
 *
 * Byte2: x offset two-complement
 * Byte3: y offset two-complement
 *
 * key    state         byte1    byte2    byte3
 * left   down 3 090000 00001001 00000000 00000000
 * left     up 3 080000 00001000 00000000 00000000
 * right  down 3 0a0000 00001010 00000000 00000000
 * right    up 3 080000 00001000 00000000 00000000
 * middle down 3 0c0000 00001100 00000000 00000000
 * middle   up 3 080000 00001000 00000000 00000000
 *
 **/
