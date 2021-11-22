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
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "uart.h"
#include "uart_arginfo.h"

#include "src/serial.h"
#include "src/exception.h"

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
  ZEND_PARSE_PARAMETERS_START(0, 0) \
  ZEND_PARSE_PARAMETERS_END()
#endif

/* Class entry pointers (global resources) */
zend_class_entry *zceSerial;
zend_class_entry *zceException;

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(uart) {
  #if defined(ZTS) && defined(COMPILE_DL_UART)
    ZEND_TSRMLS_CACHE_UPDATE();
  #endif

  return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(uart) {
  /* Extension constants */
  REGISTER_STRING_CONSTANT("UART\\EXTENSION_VERSION", PHP_UART_VERSION, CONST_CS | CONST_PERSISTENT);

  /* Class Registration (from each *.h file) */
  zceSerial = registerSerialClass();
  zceException = registerExceptionClass();

  return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(uart) {
  php_info_print_table_start();
  php_info_print_table_header(2, "uart support", "enabled");
  php_info_print_table_row(2, "Extension version", PHP_UART_VERSION);
  php_info_print_table_end();
}
/* }}} */

/* {{{ uart_module_entry */
zend_module_entry uart_module_entry = {
  STANDARD_MODULE_HEADER,
  PHP_UART_EXTNAME, /* Extension name */
  NULL,             /* zend_function_entry */
  PHP_MINIT(uart),  /* PHP_MINIT - Module initialization */
  NULL,             /* PHP_MSHUTDOWN - Module shutdown */
  PHP_RINIT(uart),  /* PHP_RINIT - Request initialization */
  NULL,             /* PHP_RSHUTDOWN - Request shutdown */
  PHP_MINFO(uart),  /* PHP_MINFO - Module info */
  PHP_UART_VERSION, /* Version */
  STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_UART
  #ifdef ZTS
    ZEND_TSRMLS_CACHE_DEFINE()
  #endif
  ZEND_GET_MODULE(uart)
#endif
