Checkpoint 7 Writeup
====================

My name: Ian Chen

My SUNet ID: ianychen

I collaborated with: tt105241

I would like to thank/reward these classmates for their help: tt105241

This checkpoint took me about 2 hours to do. I did not attend the lab session.

Solo portion:

- Did your implementation successfully start and end a conversation with another copy of itself? 

Yes

- Did it successfully transfer a one-megabyte file, with contents identical upon receipt? 

Yes

- Please describe what code changes, if any, were necessary to pass these steps:

I was failing a new student test case in net_interface where I was not using
the incoming ARP requests to also learn about new mappings of Ethernet 
addresses. Thankfully, this was an easy fix as I just had to take a chunk
of code out of an else statement to apply to all cases.



Group portion:

- Who is your lab partner (and what is their SUNet ID, e.g. "winstein"?

tt105241

- Did your implementations successfully start and end a conversation with each other (with each implementation acting as ``client'' or as ``server'')?

Yes

- Did you successfully transfer a one-megabyte file between your two
  implementations, with contents identical upon receipt?

Yes.
Transcript of transfer:


cs144@cs144vm:~/minnow$ ./build/apps/endtoend server cs144.keithw.org 55000 < /tmp/big.txt
DEBUG: Network interface has Ethernet address 02:00:00:d4:15:d0 and IP address 172.16.0.1
DEBUG: Network interface has Ethernet address 02:00:00:25:82:af and IP address 10.0.0.172
DEBUG: Network interface has Ethernet address 2a:c3:df:27:df:66 and IP address 172.16.0.100
DEBUG: minnow listening for incoming connection...
DEBUG: minnow new connection from 192.168.0.50:49454.
DEBUG: minnow inbound stream from 192.168.0.50:49454 finished cleanly.
DEBUG: Inbound stream from 172.16.0.100 finished.
DEBUG: Outbound stream to 172.16.0.100 finished.
DEBUG: minnow waiting for clean shutdown... DEBUG: minnow outbound stream to 192.168.0.50:49454 finished (63000 seqnos still in flight).
DEBUG: minnow outbound stream to 192.168.0.50:49454 has been fully acknowledged.
DEBUG: minnow TCP connection finished cleanly.
done.
Exiting... done.
cs144@cs144vm:~/minnow$ sha256sum /tmp/big.txt
447e6a35abd9dfc391602ca16d34a8499a92249bed5b6a6ff66ecab9318b74d7  /tmp/big.txt
cs144@cs144vm:~/minnow$ </dev/null ./build/apps/endtoend client cs144.keithw.org 55001 > /tmp/big-received.txt
DEBUG: Network interface has Ethernet address 02:00:00:33:0c:75 and IP address 192.168.0.1
DEBUG: Network interface has Ethernet address 02:00:00:f9:08:f8 and IP address 10.0.0.192
DEBUG: Network interface has Ethernet address c6:1e:e9:b0:05:5f and IP address 192.168.0.50
DEBUG: Connecting from 192.168.0.50:37398...
DEBUG: minnow connecting to 172.16.0.100:1234...
DEBUG: minnow successfully connected to 172.16.0.100:1234.
DEBUG: Outbound stream to 172.16.0.100 finished.
DEBUG: minnow outbound stream to 172.16.0.100:1234 finished (0 seqnos still in flight).
DEBUG: minnow outbound stream to 172.16.0.100:1234 has been fully acknowledged.
DEBUG: minnow inbound stream from 172.16.0.100:1234 finished cleanly.
DEBUG: Inbound stream from 172.16.0.100 finished.
DEBUG: minnow waiting for clean shutdown... DEBUG: minnow TCP connection finished cleanly.
done.
Exiting... done.
cs144@cs144vm:~/minnow$ sha256sum /tmp/big-received.txt 
7ecf1ae6410bd7907b86f83e0791e68d0cd2048ebf1cfa3fb8042e96d765b356  /tmp/big-received.txt

Both sha256sums were the same for both of us.

- Please describe what code changes, if any, were necessary to pass
  these steps, either by you or your lab partner.

None, both of our code worked flawlessly.



Creative portion (if you did anything for our creative challenge,
                  please boast about it!)

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
