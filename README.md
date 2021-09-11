# Embedded-Systems-2
This project is part of the "Embedded and Real-Time Systems" at Aristole University of Thessaloniki.
The aim of the code is to simulate tracking close contacts with their MAC Addresses during COVID-19, with Pthreads using a Raspberry Pi zero. 
1. ` v0 ` uses polling to measure time (while loops)
2. ` v1 ` uses pthread_cond_wait and a timer thread to reduce cpu usage

## Code usage
* type ` make v0` to compile v0
* type ` make v1` to compile v1

The results are saved in *Times.bin* and the close contacts in the files *Close_DAY_MONTH_YEAR_TIME.bin* 




The code production was unsupervised
