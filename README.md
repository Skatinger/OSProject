# OSProject
This is the repository for the Operating Systems lecture in the Summer Semester of 2018.
Authors: Group 6 aka Yvonne Eisenring, Alex Nyffenegger & Remo Geissbuehler.

to deliver on May 30 2018:

* A runnable code with a makefile and testing scripts.
* A short two pages documentation per group.
* Preparation of a final presentation per group (3min/Person).
* One page report per participant (Approach chosen, Problems encountered, Lessons learned.).

Minimal Feature Set
* Ability to write keys with values through TCP sockets.
* Ability to read values through TCP sockets by providing a key. Graceful fallback (error
message) if key is nonexistent.
* Use multiple threads.
* Simultaneous safe access of the readers and the writers.
* Provide an automated testing in form of another program or script. (Showing all the
aforementioned functionalities including simultaneous and mixed read/write requests.)

Implementation decisions
* Read or Write priority: Which requests have priority, writes or reads?
* Data structure Size: How many entries can the KV store have? Can the size grow?
* Simultaneous access: How many connections to allow simultaneously?
* Synchronization: How to synchronize the threads?

Semaphores:
http://web.cs.wpi.edu/~claypool/courses/3013-B01/samples/share-sem.c
Fork with Semaphores:
http://cboard.cprogramming.com/c-programming/67202-simple-short-fork-join-parallelization.html#post477394
