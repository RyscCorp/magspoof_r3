# Magspoof R3 Firmware
This firmware targets the [Magspoof R3 hardware](https://ryscc.com/products/magspoof) from [Rysc Corp](https://ryscc.com/). 

The sources include:

* variable swipe speed support that result in more reliable magstripe reads
* many small cleanups and convenience improvements
* build scripts that do not depend on arduino studio (build from command line)

These sources have not been compiled under Linux. If you run into trouble, please contact support at ryscc dot com or report an issue through github.

# Setup

## MacOS
Install [Crosspack AVR](https://www.obdev.at/products/crosspack/index.html) and avrdude.

```
$ brew cask install crosspack-avr
$ brew install avrdude

$ avrdude -v
avrdude: Version 6.3, compiled on Sep 17 2016 at 02:19:28

$ avr-gcc -v
  ...
gcc version 4.8.1 (GCC)
```

After installing Crosspack AVR, add /usr/local/CrossPack-AVR/bin (or wherever avr-gcc is located) to your shell path.

# Programming MagSpoof for the first time
Clone the git repository and run the following command to program the MagSpoof using
an AVR Pocket Programmer from SparkFun.

```
bash-3.2$ PROGRAMMER=usbtiny make firstprog
avrdude -p t85 -c usbtiny -U lfuse:w:0xe2:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m

avrdude: AVR device initialized and ready to accept instructions

Reading | ################################################## | 100% 0.00s

avrdude: Device signature = 0x1e930b (probably t85)
avrdude: reading input file "0xe2"
avrdude: writing lfuse (1 bytes):

Writing | ################################################## | 100% 0.00s

avrdude: 1 bytes of lfuse written
avrdude: verifying lfuse memory against 0xe2:
avrdude: load data lfuse data from input file 0xe2:
avrdude: input file 0xe2 contains 1 bytes
avrdude: reading on-chip lfuse data:

Reading | ################################################## | 100% 0.00s

avrdude: verifying ...
avrdude: 1 bytes of lfuse verified
avrdude: reading input file "0xdf"
avrdude: writing hfuse (1 bytes):

Writing | ################################################## | 100% 0.00s

avrdude: 1 bytes of hfuse written
avrdude: verifying hfuse memory against 0xdf:
avrdude: load data hfuse data from input file 0xdf:
avrdude: input file 0xdf contains 1 bytes
avrdude: reading on-chip hfuse data:

Reading | ################################################## | 100% 0.00s

avrdude: verifying ...
avrdude: 1 bytes of hfuse verified
avrdude: reading input file "0xff"
avrdude: writing efuse (1 bytes):

Writing | ################################################## | 100% 0.00s

avrdude: 1 bytes of efuse written
avrdude: verifying efuse memory against 0xff:
avrdude: load data efuse data from input file 0xff:
avrdude: input file 0xff contains 1 bytes
avrdude: reading on-chip efuse data:

Reading | ################################################## | 100% 0.00s

avrdude: verifying ...
avrdude: 1 bytes of efuse verified

avrdude: safemode: Fuses OK (E:FF, H:DF, L:E2)

avrdude done.  Thank you.

avr-gcc -g -std=c99 -Os -Wall -w -mcall-prologues -mmcu=attiny85 -DF_CPU=8000000UL magspoof.c -o magspoof
avr-objcopy -R .eeprom -O ihex magspoof magspoof.hex
rm -f magspoof
avrdude -p t85 -c usbtiny -U flash:w:magspoof.hex

avrdude: AVR device initialized and ready to accept instructions

Reading | ################################################## | 100% 0.00s

avrdude: Device signature = 0x1e930b (probably t85)
avrdude: NOTE: "flash" memory has been specified, an erase cycle will be performed
         To disable this feature, specify the -D option.
avrdude: erasing chip
avrdude: reading input file "magspoof.hex"
avrdude: input file magspoof.hex auto detected as Intel Hex
avrdude: writing flash (1170 bytes):

Writing | ################################################## | 100% 1.70s

avrdude: 1170 bytes of flash written
avrdude: verifying flash memory against magspoof.hex:
avrdude: load data flash data from input file magspoof.hex:
avrdude: input file magspoof.hex auto detected as Intel Hex
avrdude: input file magspoof.hex contains 1170 bytes
avrdude: reading on-chip flash data:

Reading | ################################################## | 100% 2.36s

avrdude: verifying ...
avrdude: 1170 bytes of flash verified

avrdude: safemode: Fuses OK (E:FF, H:DF, L:E2)

avrdude done.  Thank you.

avrdude -p t85 -c usbtiny -U flash:w:magspoof.hex

avrdude: AVR device initialized and ready to accept instructions

Reading | ################################################## | 100% 0.00s

avrdude: Device signature = 0x1e930b (probably t85)
avrdude: NOTE: "flash" memory has been specified, an erase cycle will be performed
         To disable this feature, specify the -D option.
avrdude: erasing chip
avrdude: reading input file "magspoof.hex"
avrdude: input file magspoof.hex auto detected as Intel Hex
avrdude: writing flash (1170 bytes):

Writing | ################################################## | 100% 1.69s

avrdude: 1170 bytes of flash written
avrdude: verifying flash memory against magspoof.hex:
avrdude: load data flash data from input file magspoof.hex:
avrdude: input file magspoof.hex auto detected as Intel Hex
avrdude: input file magspoof.hex contains 1170 bytes
avrdude: reading on-chip flash data:

Reading | ################################################## | 100% 2.36s

avrdude: verifying ...
avrdude: 1170 bytes of flash verified

avrdude: safemode: Fuses OK (E:FF, H:DF, L:E2)

avrdude done.  Thank you.
```

# Testing the MagSpoof
Clone the git repository and run the following command.

```
bash-3.2$ make test
./test.sh
1. Connect magstripe reader to PC
2. Turn on magspoof
3. Hold magspoof close to reader head
4. Push button on magspoof
%B4444444444444444^ABE/LINCOLN^291110100000931?;4444444444444444=29111010000093100000?+4444444444444444=29111010000093100000?
PASS
```
