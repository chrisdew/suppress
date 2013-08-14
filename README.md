suppress
========

Suppress is an HA component.

I wrote it so that I could have script, which sent alert emails, running on two boxes (for resiliance), while only sending customers one copy of the alert email (in almost all cases).

Suppress is a wrapper program which suppresses the execution of all instances of the client program except for the one with the lowest IP address and PID.

When the wrapper has received no suppressant packets (from a lower IP, or pid, if IPs are identical) for 3 seconds it will (re)spawn the wrapped program.

When the wrapper receives a suppressant packet, it will SIGTERM and SIGKILL the wrapped program.

Status
------

This is the first, very hacky, C implementation of the idea.  Use at your own risk.

See issues on GitHub for some of the problems you may have...


