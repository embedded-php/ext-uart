/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: c25ad04e9e70d81d2650288be2629c6766ad7e2b */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_UART_Serial___construct, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, device, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, baudRate, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_UART_Serial_putString, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_UART_Serial_getString, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, maxLen, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_UART_Serial_flush, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()


ZEND_METHOD(UART_Serial, __construct);
ZEND_METHOD(UART_Serial, putString);
ZEND_METHOD(UART_Serial, getString);
ZEND_METHOD(UART_Serial, flush);


static const zend_function_entry class_UART_Exception_methods[] = {
	ZEND_FE_END
};


static const zend_function_entry class_UART_Serial_methods[] = {
	ZEND_ME(UART_Serial, __construct, arginfo_class_UART_Serial___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(UART_Serial, putString, arginfo_class_UART_Serial_putString, ZEND_ACC_PUBLIC)
	ZEND_ME(UART_Serial, getString, arginfo_class_UART_Serial_getString, ZEND_ACC_PUBLIC)
	ZEND_ME(UART_Serial, flush, arginfo_class_UART_Serial_flush, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
