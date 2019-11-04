Modified the 'client' prg to 'client_any' : 
to accept hostname and port# as parameters.

So, now:

1. Use nmap (or it's GUI zenmap !) to do a port scan on some target website; it will show you open ports..
Eg.
# nmap bbc.com
Starting Nmap 5.21 ( http://nmap.org ) at 2012-06-02 08:30 PDT
Nmap scan report for bbc.com (212.58.241.131)
Host is up (0.31s latency).
Not shown: 997 closed ports
PORT     STATE    SERVICE
53/tcp   open     domain
80/tcp   open     http
8181/tcp filtered unknown

Nmap done: 1 IP address (1 host up) scanned in 8.42 seconds
# 

2. Try a port:

# ./client_any bbc.com 53
./client_any: connecting to 212.58.241.131
./client_any: received ''
# 

