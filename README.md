# UART PHP Extension

The UART PHP Extension provides an object-oriented wrapper to the UART Linux Kernel driver that gives users the ability
to write code that interacts with external hardware using the
[UART](https://en.wikipedia.org/wiki/Universal_asynchronous_receiver-transmitter) asynchronous serial communication
protocol.

## Acknowledgements

This project takes a lot of inspiration from [WiringPi](http://wiringpi.com).

## Requirements

- PHP >= 8.0 (either NTS or TS version);

> **Note:** if you are using this extension on Raspbian or Raspberry Pi OS, the user executing the script **must**
> belong to the **dialout** group or you may run into "Permission denied" errors.

On **Raspbian** or **Raspberry Pi OS**:

**PHP 8.0**:
```bash
sudo apt install php8.0-cli php8.0-dev php8.0-phpdbg
```

**PHP 8.1**:
```bash
sudo apt install php8.1-cli php8.1-dev php8.1-phpdbg
```

## Building

```bash
phpize
./configure
make
make test
```

## Stubs

After changing any of the [stubs](stubs/), the main [stub file](phpuart.stub.php) must be updated by running the command below.

```bash
php stubs/update-main-stup.php
```

Once the stub is updated, the regular build process can be executed.

**Note:** The main stub file ([phpuart.stub.php](phpuart.stub.php)) is a generated file, edit the files on the [stubs](stubs/) folder instead.

## Classes

Class                                 | Description
--------------------------------------|------------
[UART\Serial](stubs/Serial.php)       | Represents a Serial Port.
[UART\Exception](stubs/Exception.php) | Base exception.

## Resources

- https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/

## License

This library is licensed under the [PHP License](LICENSE).
