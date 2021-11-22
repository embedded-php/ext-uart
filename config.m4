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
  yes,
  yes
)

if test "$PHP_UART" != "no"; then
  AC_DEFINE(HAVE_UART, 1, [ Have uart support ])

  PHP_NEW_EXTENSION(uart, uart.c src/serial.c src/exception.c, $ext_shared)
fi
