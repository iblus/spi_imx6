/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 */

#define LOG_TAG "BlusSpiHal"

//#include <hardware/hardware.h>

//#include <fcntl.h>
//#include <errno.h>

//#include <cutils/log.h>
//#include <cutils/atomic.h>
//#include <android/log.h>

//#include <sys/types.h>
//#include <sys/stat.h>
//#include <fcntl.h>
//#include <sys/ioctl.h>
//#include <unistd.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <linux/types.h>
//#include <linux/spi/spidev.h>
#include <spi.h>

#define BUF_MAX_SIZE	0x1000
#define DEV_SPI1	"/dev/spidev0.0"

#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__) 
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG , LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO  , LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN  , LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , LOG_TAG, __VA_ARGS__)

struct spi_ioc_transfer {
	unsigned long long		tx_buf;
	unsigned long long		rx_buf;

	unsigned int 		len;
	unsigned int		speed_hz;

	unsigned short		delay_usecs;
	unsigned char		bits_per_word;
	unsigned char		cs_change;
	unsigned int		pad;

	/* If the contents of 'struct spi_ioc_transfer' ever change
	 * incompatibly, then the ioctl number (currently 0) must change;
	 * ioctls with constant size fields get a bit more in the way of
	 * error checking than ones (like this) where that field varies.
	 *
	 * NOTE: struct layout is the same in 64bit and 32bit userspace.
	 */
};

static unsigned char mode = 0;
static unsigned char bits_per_word = 8;
static unsigned int speed = 100000;
//the buffer of receive zone
//static char RBuffer[BUF_MAX_SIZE];

#define SPI_IOC_MESSAGE_1			1075866368
#define SPI_IOC_WR_MODE				1073834753
#define SPI_IOC_RD_MODE				-2147390719
#define SPI_IOC_WR_BITS_PER_WORD	1073834755
#define SPI_IOC_RD_BITS_PER_WORD	-2147390717
#define SPI_IOC_WR_MAX_SPEED_HZ		1074031364
#define SPI_IOC_RD_MAX_SPEED_HZ		-2147194108

/*****************************************************************************/
int spi_transfer(int fd, char *tbuf, char *rbuf, int bytes)
{
	int ret;
	//rbuf = RBuffer;
	memset(rbuf, 0, BUF_MAX_SIZE);

	struct spi_ioc_transfer tr =
		{ .tx_buf = (unsigned long) tbuf, .rx_buf = (unsigned long) rbuf, .len = bytes, };

	ret = ioctl(fd, SPI_IOC_MESSAGE_1, &tr);
	if (ret == 1)
		LOGE("can't send spi message");
	LOGI("Send data : %s\n", (char *)tbuf);
	LOGI("receive len : %d\n", ret);
	LOGI("receive data : %s\n", (char *)rbuf);
	return ret;
}
int spi_config(int fd, int _speed, int _bits_per_word, int _mode)
{
	int res = 0;
	int _fd = fd;

	if (_fd < 0)
	{
		LOGE("The SPI device is unable ");
		return -1;
	}
	LOGI("spi mode: %d\n", mode);
	
	res = ioctl(_fd, SPI_IOC_WR_MODE, &mode);
	if (res == -1)
	{
		LOGE("can't set spi mode");
		goto exit;
	}

	res = ioctl(_fd, SPI_IOC_RD_MODE, &mode);
	if (res == -1)
	{
		LOGE("can't set spi mode");
		goto exit;
	}
	/*
	 * bits per word
	 */
	res = ioctl(_fd, SPI_IOC_WR_BITS_PER_WORD, &bits_per_word);
	if (res == -1)
	{
		LOGE("can't set bits per word");
		goto exit;
	}

	res = ioctl(_fd, SPI_IOC_RD_BITS_PER_WORD, &bits_per_word);
	if (res == -1)
	{
		LOGE("can't get bits per word");
		goto exit;
	}

	/*
	 * max speed hz
	 */
	res = ioctl(_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (res == -1)
	{
		LOGE("can't set max speed hz");
		goto exit;
	}

	res = ioctl(_fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (res == -1)
	{
		LOGE("can't get max speed hz");
		goto exit;
	}

	LOGI("spi mode: %d\n", mode);
	LOGI("bits per word: %d\n", bits_per_word);
	LOGI("max speed: %d Hz (%d KHz)\n", speed, speed / 1000);
	return 0;
	exit: close(_fd);
	return 1;
}
int spi_open(int no)
{
	char *rbuf;
	int res = 0;
	int fd = -1;

	switch (no)
	{
		case 1:
		{
			fd = open(DEV_SPI1, O_RDWR);
			if (fd < 0)
			{
				LOGE("the spi open fail ");
				LOGE("Failed to open device file /dev/freg -- %s.", strerror(errno));
				return -1;
			}
			break;
		}
		default:
			fd = open(DEV_SPI1, O_RDWR);
			if (fd < 0)
			{
				LOGE("the spi open fail");
				return -1;
			}
			break;
	}

	LOGI("the spi fd is %d",fd);
	return fd;
}
int spi_close(int fd)
{
	if (fd < 0)
		return 1;
	close (fd);
	LOGI("the spi is close");
	return 0;
}
static int spi_device_close(struct hw_device_t* device)
{
	struct spi_control_device_t* ctx = (struct spi_control_device_t*) device;
	if (ctx)
	{
		free(ctx);
	}
	return 0;
}
static int spi_device_open(const struct hw_module_t* module, const char* name, struct hw_device_t** device)
{
	struct spi_control_device_t *dev;
	LOGI("spi device starting open in HAL");

	dev = (struct spi_control_device_t *) malloc(sizeof(*dev));
	memset(dev, 0, sizeof(*dev));

	dev->common.tag = HARDWARE_DEVICE_TAG;
	dev->common.version = 0;
	dev->common.module = (struct hw_module_t*)module;
	dev->common.close = spi_device_close;

	dev->transfer = spi_transfer;
	dev->open = spi_open;
	dev->close = spi_close;
	dev->config = spi_config;

	*device = &dev->common;
	LOGI("spi device open in HAL");

	success: return 0;
}

static struct hw_module_methods_t spi_module_methods =
	{ open: spi_device_open };

struct spi_module_t HAL_MODULE_INFO_SYM =
	{ common:
		{ tag : HARDWARE_MODULE_TAG, version_major : 1, version_minor : 0, id : SPI_HARDWARE_MODULE_ID, name : "SPI Stub", author : "The blus Open Source Project", methods
				: &spi_module_methods, }
	/* supporting APIs go here */
	};

