# Multithread-Base
Multithread base for beginners who are starting to code in C++ and are
willing to learn the basics about threads and how threads work with your
workloads

# What's included?
Simple as it can be, default console app template from Visual Studio.
Difference is that now everything you run runs on separate threads and is
appropriately split on your_amount_of_cpu_cores number of threads. Some gamehacking
stuff is included in here for some really basic understanding on both external and internal
game hacking. 

# Feature list
- Simple interface with automated multithreading (with custom CPU affinity support)
- Memory library (attaching/detaching from a process, WPM/RPM  and finding signatures externally)
- Threading library
- Injection of DLL files (loadlibrary and manualmap, shellcode injection included with only flaw being hardcoded shellcode)
- Logging system 
- Overlay for drawing stuff (borderless/fullscreen windowed)
- Simple rendering functions to use with overlay (line, rect, filled rect)
- Some simplified function wrappers (mostly easy learning stuff for people like function pointers etc.)

# Future updates
- Addition of a simple driver communication method
- Simple driver source (no certificate so you will have to disable some stuff in Windows)
- Example on how these two things actually work

# But, this isn't complete and there are things missing to this base?
This was made for a school-like lessons. It's just a stripped base for basic 
threading operations and isn't made for any complex threading sorting of workloads
etc. If you have time, you can modify it to your liking.


