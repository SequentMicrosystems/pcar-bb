# pcar-bb
Interface to Emergency Vehicle Innovations Perimeter Patrol Controller

## Setup
The controller use I2C port to communicate with the Beagle bone Black SBC so you have to enable the port.
 ### Temporarly enable I2C1 on p9-17/18:
```bash
config-pin P9.17 i2c
config-pin P9.18 i2c
```
[Documentation](https://github.com/cdsteinkuehler/beaglebone-universal-io)

for some os versions also works:
```bash
config-pin p9-17 i2c
config-pin p9-18 i2c
```


  ### Permanently
 Edit the following file:
 ```bash
~$ sudo nano /etc/rc.local
```
put inside 
```bash
#!/bin/sh -e
#
# rc.local
#
# This script is executed at the end of each multiuser runlevel.
# Make sure that the script will "exit 0" on success or any other
# value on error.
#
# In order to enable or disable this script just change the execution
# bits.
#
# By default this script does nothing.

config-pin P9.17 i2c
config-pin P9.18 i2c

exit 0
```

## Command line Interface

### Install

Open a terminal and run the following commands:

```bash
git clone https://github.com/SequentMicrosystems/pcar-bb
cd pcar-bb
sudo make install
```

### Usage 

Display the list of the command options with:
```bash
pcar -h
```

Turn on output channel 1 with:
```bash
pcar outwr 1 1
```



