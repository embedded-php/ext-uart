/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Flavio Heleno <https://github.com/flavioheleno>              |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "serial.h"
#include "uart.h"
#include "uart_arginfo.h"
#include "zend_exceptions.h"
#include "zend_interfaces.h"
#include "zend_object_handlers.h"

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/file.h>

/* zend_object wrapper to ensure the serial device is handled properly */
typedef struct _serialObject {
  int fd;
  zend_object zendObject;
} serialObject;

static zend_object_handlers serialObjectHandlers;

/********************************/
/* Extension's Internal Methods */
/********************************/

/* retrieve serialObject pointer from a zend_object pointer */
static inline serialObject *getSerialObject(zend_object *obj) {
  return (serialObject *)((char *)(obj) - XtOffsetOf(serialObject, zendObject));
}

/* handle serialObject release */
static void serialFreeObject(zend_object *obj) {
  serialObject *self = getSerialObject(obj);

  /* failure to retrieve serial instance (edge case?) */
  if (self == NULL) {
    return;
  }

  /* if the fd was open, close it */
  if (self->fd >= 0) {
    close(self->fd);
    self->fd = -1;
  }

  /* release self->zendObject */
  zend_object_std_dtor(&self->zendObject);
}

/* custom unset($inst->prop) handler */
static void unsetPropertyObjectHandler(zend_object *object, zend_string *offset, void **cache_slot) {
  zend_throw_error(NULL, "Cannot unset UART\\Serial::$%s property", ZSTR_VAL(offset));
}

/********************************/
/* Extension's External Methods */
/********************************/

zend_class_entry* registerSerialClass(void) {
  zend_class_entry ce, *classEntry;

  INIT_CLASS_ENTRY(ce, "UART\\Serial", class_UART_Serial_methods);
  classEntry = zend_register_internal_class(&ce);
  /* Final class / Objects of this class may not have dynamic properties */
  classEntry->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NO_DYNAMIC_PROPERTIES;
  /* intercept object creation to change object handlers */
  classEntry->create_object = serialCreateObject;

  /* disable serialization/unserialization */
  #ifdef ZEND_ACC_NOT_SERIALIZABLE
    classEntry->ce_flags |= ZEND_ACC_NOT_SERIALIZABLE;
  #else
    classEntry->serialize = zend_class_serialize_deny;
    classEntry->unserialize = zend_class_unserialize_deny;
  #endif

  /* initialize serialObjectHandlers with standard object handlers */
  memcpy(&serialObjectHandlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

  /* disable object cloning */
  serialObjectHandlers.clone_obj = NULL;
  /* Handler for objects that cannot be meaningfully compared. */
  /* serialObjectHandlers.compare = zend_objects_not_comparable; */
  /* not yet sure what this sets */
  serialObjectHandlers.offset   = XtOffsetOf(serialObject, zendObject);
  /* hook the object release */
  serialObjectHandlers.free_obj = serialFreeObject;
  /* hook the object property unset */
  serialObjectHandlers.unset_property = unsetPropertyObjectHandler;

  zval propDefaultValue;
  zend_string *propName;
  /* default property value (undefined) */
  ZVAL_UNDEF(&propDefaultValue);
  propName = zend_string_init("device", sizeof("device") - 1, false);
  /* private int $device */
  zend_declare_typed_property(
    classEntry,
    propName,
    &propDefaultValue,
    ZEND_ACC_PRIVATE,
    NULL,
    (zend_type)ZEND_TYPE_INIT_MASK(MAY_BE_STRING)
  );
  zend_string_release(propName);

  propName = zend_string_init("baudRate", sizeof("baudRate") - 1, false);
  /* private int $baudRate */
  zend_declare_typed_property(
    classEntry,
    propName,
    &propDefaultValue,
    ZEND_ACC_PRIVATE,
    NULL,
    (zend_type)ZEND_TYPE_INIT_MASK(MAY_BE_LONG)
  );
  zend_string_release(propName);

  return classEntry;
}

zend_object *serialCreateObject(zend_class_entry *zceClass) {
  serialObject *self = zend_object_alloc(sizeof(serialObject), zceClass);

  zend_object_std_init(&self->zendObject, zceClass);
  object_properties_init(&self->zendObject, zceClass);

  /* set object handlers */
  self->zendObject.handlers = &serialObjectHandlers;

  return &self->zendObject;
}

/********************************/
/* PHP Visible Methods          */
/********************************/

/* {{{ UART\Serial::__construct(string $device, int $baudRate): void */
PHP_METHOD(UART_Serial, __construct) {
  char *device;
  size_t deviceLen;
  zend_long baudRate;
  ZEND_PARSE_PARAMETERS_START(2, 2)
    Z_PARAM_STRING(device, deviceLen)
    Z_PARAM_LONG(baudRate)
  ZEND_PARSE_PARAMETERS_END();

  serialObject *self = getSerialObject(Z_OBJ_P(ZEND_THIS));

  self->fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
  if (self->fd < 0) {
    zend_throw_exception_ex(zceException, 0, "Failed to open the serial device '%s'", device);

    RETURN_THROWS();
  }

  if (flock(self->fd, LOCK_EX | LOCK_NB) < 0) {
    zend_throw_exception_ex(zceException, 0, "Failed to obtain a lock of the serial device '%s'", device);
    close(self->fd);

    RETURN_THROWS();
  }

  fcntl(self->fd, F_SETFL, O_RDWR);

  struct termios tty;
  tcgetattr(self->fd, &tty);

  speed_t speedBaud;

  switch (baudRate) {
    case 50:
      speedBaud = B50;
      break;
    case 75:
      speedBaud = B75;
      break;
    case 110:
      speedBaud = B110;
      break;
    case 134:
      speedBaud = B134;
      break;
    case 150:
      speedBaud = B150;
      break;
    case 200:
      speedBaud = B200;
      break;
    case 300:
      speedBaud = B300;
      break;
    case 600:
      speedBaud = B600;
      break;
    case 1200:
      speedBaud = B1200;
      break;
    case 1800:
      speedBaud = B1800;
      break;
    case 2400:
      speedBaud = B2400;
      break;
    case 4800:
      speedBaud = B4800;
      break;
    case 9600:
      speedBaud = B9600;
      break;
    case 19200:
      speedBaud = B19200;
      break;
    case 38400:
      speedBaud = B38400;
      break;
    case 57600:
      speedBaud = B57600;
      break;
    case 115200:
      speedBaud = B115200;
      break;
    case 230400:
      speedBaud = B230400;
      break;
    case 460800:
      speedBaud = B460800;
      break;
    case 500000:
      speedBaud = B500000;
      break;
    case 576000:
      speedBaud = B576000;
      break;
    case 921600:
      speedBaud = B921600;
      break;
    case 1000000:
      speedBaud = B1000000;
      break;
    case 1152000:
      speedBaud = B1152000;
      break;
    case 1500000:
      speedBaud = B1500000;
      break;
    case 2000000:
      speedBaud = B2000000;
      break;
    case 2500000:
      speedBaud = B2500000;
      break;
    case 3000000:
      speedBaud = B3000000;
      break;
    case 3500000:
      speedBaud = B3500000;
      break;
    case 4000000:
      speedBaud = B4000000;
      break;
    default:
      zend_throw_exception_ex(zceException, 0, "Invalid baud rate speed '%d'", baudRate);
      close(self->fd);

      RETURN_THROWS();
  }

  cfmakeraw(&tty);
  cfsetispeed(&tty, speedBaud);
  cfsetospeed(&tty, speedBaud);

  // Turn on READ & ignore ctrl lines (CLOCAL = 1)
  tty.c_cflag |= (CLOCAL | CREAD);
  // Clear parity bit, disabling parity (most common)
  tty.c_cflag &= ~PARENB;
  // Clear stop field, only one stop bit used in communication (most common)
  tty.c_cflag &= ~CSTOPB;
  // Clear all bits that set the data size
  tty.c_cflag &= ~CSIZE;
  // 8 bits per byte (most common)
  tty.c_cflag |= CS8;
  // Disable echo / erasure
  tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
  // Prevent special interpretation of output bytes (e.g. newline chars)
  tty.c_oflag &= ~OPOST;

  tty.c_cc [VMIN]  =   0;
  // Wait for up to 10s (100 deciseconds), returning as soon as any data is received.
  tty.c_cc [VTIME] = 100;

  tcsetattr(self->fd, TCSANOW, &tty);

  int status;
  ioctl(self->fd, TIOCMGET, &status);

  status |= TIOCM_DTR;
  status |= TIOCM_RTS;

  ioctl(self->fd, TIOCMSET, &status);







  // int err;
  // errno = 0;
  // err = ioctl(self->fd, UART_SLAVE, baudRate);
  // if (err < 0) {
  //   close(self->fd);
  //   zend_throw_exception_ex(zceException, errno, "Unable to select the serial device: %s", strerror(errno));

  //   RETURN_THROWS();
  // }

  /* update class property with constructor argument values */
  zend_update_property_stringl(zceSerial, Z_OBJ_P(ZEND_THIS), "device", sizeof("device") - 1, device, deviceLen);
  zend_update_property_long(zceSerial, Z_OBJ_P(ZEND_THIS), "baudRate", sizeof("baudRate") - 1, baudRate);
}
/* }}} */

/* {{{ UART\Serial::getDevice(): int */
PHP_METHOD(UART_Serial, getDevice) {
  ZEND_PARSE_PARAMETERS_NONE();

  zval rv;
  zval *device = zend_read_property(zceSerial, Z_OBJ_P(ZEND_THIS), "device", sizeof("device") - 1, true, &rv);

  RETURN_STR(zval_get_string(device));
}
/* }}} */

/* {{{ UART\Serial::getBaudRate(): int */
PHP_METHOD(UART_Serial, getBaudRate) {
  ZEND_PARSE_PARAMETERS_NONE();

  zval rv;
  zval *baudRate = zend_read_property(zceSerial, Z_OBJ_P(ZEND_THIS), "baudRate", sizeof("baudRate") - 1, true, &rv);

  RETURN_LONG(zval_get_long(baudRate));
}
/* }}} */


/* {{{ UART\Serial::putString(string $data): void */
PHP_METHOD(UART_Serial, putString) {
  char *data;
  size_t dataLen;
  ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_STRING(data, dataLen)
  ZEND_PARSE_PARAMETERS_END();

  serialObject *self = getSerialObject(Z_OBJ_P(ZEND_THIS));
  write(self->fd, data, dataLen);
}
/* }}} */

/* {{{ UART\Serial::getString(int $maxLen): string */
PHP_METHOD(UART_Serial, getString) {
  zend_long maxLen;
  ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_LONG(maxLen)
  ZEND_PARSE_PARAMETERS_END();


}
/* }}} */

/* {{{ UART\Serial::flush(): void */
PHP_METHOD(UART_Serial, flush) {
  ZEND_PARSE_PARAMETERS_NONE();

  serialObject *self = getSerialObject(Z_OBJ_P(ZEND_THIS));
  tcflush (self->fd, TCIOFLUSH);
}
/* }}} */
























/*

int main() {
  // Open the serial port. Change device path as needed (currently set to an standard FTDI USB-UART cable type device)
  int serial_port = open("/dev/ttyUSB0", O_RDWR);

  if (flock(serial_port, LOCK_EX | LOCK_NB) == -1) {
    // another process has the lock
  }

  // Create new termios struc, we call it 'tty' for convention
  struct termios tty;

  // Read in existing settings, and handle any error
  if(tcgetattr(serial_port, &tty) != 0) {
      printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
      return 1;
  }

  tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
  tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
  tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size 
  tty.c_cflag |= CS8; // 8 bits per byte (most common)
  tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
  tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

  tty.c_lflag &= ~ICANON;
  tty.c_lflag &= ~ECHO; // Disable echo
  tty.c_lflag &= ~ECHOE; // Disable erasure
  tty.c_lflag &= ~ECHONL; // Disable new-line echo
  tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
  tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

  tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
  tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
  // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
  // tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

  tty.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
  tty.c_cc[VMIN] = 0;

  // Set in/out baud rate to be 9600
  cfsetispeed(&tty, B9600);
  cfsetospeed(&tty, B9600);

  // Save tty settings, also checking for error
  if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
      printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
      return 1;
  }

  // Write to serial port
  unsigned char msg[] = { 'H', 'e', 'l', 'l', 'o', '\r' };
  write(serial_port, "Hello, world!", sizeof(msg));

  // Allocate memory for read buffer, set size according to your needs
  char read_buf [256];

  // Normally you wouldn't do this memset() call, but since we will just receive
  // ASCII data for this example, we'll set everything to 0 so we can
  // call printf() easily.
  memset(&read_buf, '\0', sizeof(read_buf);

  // Read bytes. The behaviour of read() (e.g. does it block?,
  // how long does it block for?) depends on the configuration
  // settings above, specifically VMIN and VTIME
  int num_bytes = read(serial_port, &read_buf, sizeof(read_buf));

  // n is the number of bytes read. n may be 0 if no bytes were received, and can also be -1 to signal an error.
  if (num_bytes < 0) {
      printf("Error reading: %s", strerror(errno));
      return 1;
  }

  // Here we assume we received ASCII data, but you might be sending raw bytes (in that case, don't try and
  // print it to the screen like this!)
  printf("Read %i bytes. Received message: %s", num_bytes, read_buf);

  close(serial_port)
  return 0; // success
}
*/
