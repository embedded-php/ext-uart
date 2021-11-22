<?php

namespace UART;

final class Serial {
  /**
   * Open a serial device at $device at $baudRate Hz.
   *
   * @param string $device   Serial device path
   * @param int    $baudRate Baud rate in Hz
   *
   * @return void
   */
  public function __construct(string $device, int $baudRate) {}

  /**
   * Return the serial device path.
   *
   * @return string
   */
  public function getDevice(): string {}

  /**
   * Return the baud rate speed in Hz.
   *
   * @return int
   */
  public function getBaudRate(): int {}

  /**
   * Sent a string to the serial device.
   *
   * @param string $data Data to be sent
   *
   * @return void
   */
  public function putString(string $data): void {}

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
  public function flush(): void {}
}
