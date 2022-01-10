dnl config.m4 for extension uart

PHP_ARG_ENABLE(
  uart,
  whether to enable uart support,
  [
    AS_HELP_STRING(
      [ --enable-uart ],
      [Include uart support]
    )
  ],
  no
)

if test "$PHP_UART" != "no"; then
  PHP_VERSION=$($PHP_CONFIG --vernum)
  AC_MSG_CHECKING([PHP version]);
  if test $PHP_VERSION -lt 80000; then
    AC_MSG_ERROR([uart requires PHP 8.0+]);
  fi
  AC_MSG_RESULT([$PHP_VERSION])

  AC_DEFINE(HAVE_UART, 1, [ Have uart support ])

  PHP_NEW_EXTENSION(uart, uart.c src/serial.c src/exception.c, $ext_shared)
fi
