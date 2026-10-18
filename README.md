# I2CAddressScanner

Here is a bare metal program for ATmega328p to scan all addresses on the I2C communication bus.

## Ready to start

> [!WARNING]
> _by default the communication with the Atmega328p is done with this port /dev/ttyACM0_

```concole
$ make all
$ make dude
$ screen /dev/ttyACM0 1000000 
```
## result

``` console
$ NACK Address: 0x00
$ NACK Address: 0x01
$ NACK Address: 0x02
$ ...
$ NACK Address: 0x65
$ NACK Address: 0x66
$ NACK Address: 0x67
$ PASS avec: 0x68
```
