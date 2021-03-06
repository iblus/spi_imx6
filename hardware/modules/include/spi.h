/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <hardware/hardware.h>

#include <fcntl.h>
#include <errno.h>

#include <cutils/log.h>
//#include <cutils/atomic.h>

/*****************************************************************************/

struct spi_module_t {
   struct hw_module_t common;
};

struct spi_control_device_t {
   struct hw_device_t common;
   /* supporting control APIs go here */
   int (*transfer)(int fd, char *tbuf, char *rbuf, int bytes);
   int (*open)(int no);
   int (*close)(int fd);
   int (*config)(int fd, int _speed, int _bits_per_word, int _mode);

};

/*****************************************************************************/

struct spi_control_context_t {
	struct spi_control_device_t device;
};

#define SPI_HARDWARE_MODULE_ID "spi"

