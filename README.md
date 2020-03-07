# WatchDog
This app makes sure that the user-process stays alive. If the user-process falls - it revives it.
Including an implementation of a scheduler based on a heap which based on a dynamic vector.

Written in C and uses IPC, multi-threading, environment variables and a makefile.

# Abilities:
Mutual guarding - the watched process can revive the WD as well.
Simple API - all the user should do is call the 'WDKeepMeAlive()' function, and at the end of the 
protected piece of code call 'WDLetMeDie()'. 
Efficient - scheduled checking on the guarded process (no busy waiting).
Code reuse - the main loop of both processes is common via a shared object.

# How to use:
1. run 'make'
2. copy into the folder of the user program the next files:
wd_outer.out, libwd.a, libwd.so, wd_api.h.
3. call WDKeepMeAlive() & WDLetMeDie() from within the user program.
