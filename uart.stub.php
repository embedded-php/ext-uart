<?php

/** @generate-function-entries */

namespace UART;

/**
 * Base exception
 */
class Exception extends \Exception {}

class Serial {
  /**
   * Open a serial device at $device running at $baudRate.
   *
   * @param string $device   Serial device path
   * @param int    $baudRate Baud rate in Hz
   *
   * @return void
   */
  public function __construct(string $device, int $baudRate) {}

  /**
   * Sent a string to the serial device.
   *
   * @param string $string String to be sent
   *
   * @return void
   */
  public function putString(string $string): void {}

  /**
   * Return a string from the serial device with up to $maxLen characters.
   *
   * @param int $maxLen Max length
   *
   * @return string
   */
  public function getString(int $maxLen): string {}

  /**
   * Flush the Tx/Rx buffers.
   *
   * @return void
   */
  public function flush() : void {}
}

