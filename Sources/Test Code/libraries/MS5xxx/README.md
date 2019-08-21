arduino-ms5xxx
==============

Arduino Library with examples for using digital pressure sensors MS5607/MS5611 (and MS56xx, MS57xx, MS58xx) based on AN520 by manufacturer and the datasheets. This library has been tested extensively (over almost a year of datataking) with MS5607 and also tested newer MS5611 sensors. 
Note the different I2C-addresses, calculations and correction functions between the different chips. One generic MS5xxx-class as well as a derived class MS5611 with the new I2C address and the new calculations have been implemented.

The examples nicely show how to use this library with the standard MS5xxx-class. For usage with MS5611 chips just use the class MS5611 in the same way.
"Standard units" are Pascals for pressure and 0.01 °C for temperature as calculations are done exactly as shown in the manufacturers datasheet.

<a href="http://dx.doi.org/10.5281/zenodo.11966"><img src="https://zenodo.org/badge/6485/Schm1tz1/arduino-ms5xxx.png"><a>
