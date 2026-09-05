#ifndef IOCTL_LED_H
#define IOCTL_LED_H

#include<linux/ioctl.h>

#define LED_MAGIC 'L'
#define LED_ON  _IO(LED_MAGIC, 1)
#define LED_OFF  _IO(LED_MAGIC, 2)
#define LED_GET  _IOR(LED_MAGIC, 3, int)

#endif
