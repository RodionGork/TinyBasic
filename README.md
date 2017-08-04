# TinyBasic

Simple implementation of Basic to controln Arduino Micro or STM32F103 mcus


### Speed test

(current implementation optimizes interpretation of tokens,
but does not hard link variables and labels yet)

simple code used:

    10 x = 100000
    20 if x%5000=0;print x
    30 x = x-1
    40 if x>=0; goto 20

i.e. about 4 statements per iteration

- on STM32 with -O0: 10 sec
- on STM32 with -O1: 6 sec
- on STM32 with -O2: 5 sec
- on STM32 with -O3: 4 sec
- on Arduino Micro (x=30000): 7 sec (i.e. 23 for 100000)

i.e. roughly 100000 statements per second on STM32 achievable
