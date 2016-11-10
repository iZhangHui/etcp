![](https://cloud.githubusercontent.com/assets/6889915/20165860/c4bac6b6-a74b-11e6-87e5-ff01a5511236.jpg)

Effective TCP/IP Programming
============================

## Source code

- The source code of the Book [Effective TCP/IP Programming](http://home.netcom.com/~jsnader/)
by Jon Snader imported from http://home.netcom.com/~jsnader/etcpsrc.zip.

## Build
In order to compile the code, first change into the subdirectory that
most closely describes your system (bsd, solaris, linux, win), and run
make.  For UNIX systems, this can be either the system make or gmake.
For windows, use nmake.  This will perform whatever initializations are
appropriate for your system. Next, change back up to the parent (source)
directory and run make again. That will build the libraries and test programs.

Take linux system for example:
```
cd linux
make
cd ../
make
```

## Contents

### Basics

- Tip 1 :  Understand the Difference between Connected and Connectionless Protocols
- Tip 2 :  Understand Subnets and CIDR
- Tip 3 :  Understand Private Addresses and NAT
- Tip 4 :  Develop and Use Application ``Skeletons''
- Tip 5 :  Prefer the Sockets Interface to XTI/TLI
- Tip 6 :  Remember That TCP Is a Stream Protocol
- Tip 7 :  Don't Underestimate the Performance of TCP
- Tip 8 :  Avoid Reinventing TCP
- Tip 9 :  Realize That TCP Is a Reliable Protocol, Not an Infallible Protocol
- Tip 10 :  Remember That TCP/IP Is Not Polled
- Tip 11 :  Be Prepared for Rude Behavior from a Peer
- Tip 12 :  Don't Assume That a Successful LAN Strategy Will Scale to a WAN
- Tip 13 :  Learn How the Protocols Work
- Tip 14 :  Don't Take the OSI Seven-Layer Reference Model Too Seriously

### Building Effective and Robust Network Programs

- Tip 15 :  Understand the TCP Write Operation
- Tip 16 :  Understand the TCP Orderly Release Operation
- Tip 17 :  Consider Letting inetd Launch Your Application
- Tip 18 :  Consider Letting tcpmux ``Assign'' Your Server's Well-known Port
- Tip 19 :  Consider Using Two TCP Connections
- Tip 20 :  Consider Making Your Applications Event Driven (1)
- Tip 21 :  Consider Making Your Applications Event Driven (2)
- Tip 22 :  Don't Use TIME-WAIT Assassination to Close a Connection
- Tip 23 :  Servers Should Set the SO_REUSEADDR Option
- Tip 24 :  When Possible, Use One Large Write Instead of Multiple Small Writes
- Tip 25 :  Understand How to Time Out a connect Call
- Tip 26 :  Avoid Data Copying
- Tip 27 :  Zero the sockaddr_in Structure Before Use
- Tip 28 :  Don't Forget About Byte Sex
- Tip 29 :  Don't Hardcode IP Addresses or Port Numbers in Your Application
- Tip 30 :  Understand Connected UDP Sockets
- Tip 31 :  Remember That All the World's Not C
- Tip 32 :  Understand the Effects of Buffer Sizes

### Tools and Resources

- Tip 33 :  Become Familiar with the ping Utility
- Tip 34 :  Learn to Use tcpdump or a Similar Tool
- Tip 35 :  Learn to Use traceroute
- Tip 36 :  Learn to Use ttcp
- Tip 37 :  Learn to Use lsof
- Tip 38 :  Learn to Use netstat
- Tip 39 :  Learn to Use Your System's Call Trace Facility
- Tip 40 :  Build and Use a Tool to Capture ICMP Messages
- Tip 41 :  Read Stevens
- Tip 42 :  Read Code
- Tip 43 :  Visit the RFC Editor's Page
- Tip 44 :  Frequent the News Groups