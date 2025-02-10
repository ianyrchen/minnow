Checkpoint 3 Writeup
====================

My name: Ian Chen

My SUNet ID: ianychen

I collaborated with: 

I would like to thank/reward these classmates for their help: tt105241

This checkpoint took me about 16 hours to do. I did not attend the lab session.

Program Structure and Design of the TCPSender [Describe data
structures and approach taken. Describe alternative designs considered
or tested.  Describe benefits and weaknesses of your design compared
with alternatives -- perhaps in terms of simplicity/complexity, risk
of bugs, asymptotic performance, empirical performance, required
implementation time and difficulty, and other factors. Include any
measurements if applicable.]: 

For my data structure to store outstanding segments, I decided to use a 
std::deque containing a custom struct, Segment, and each segment contains
a TCPSenderMessage and the time_left_ for the RTO value of the Segment. I
originally considered using a queue, but implementing the sequence_numbers_
in_flight() method was much simpler using a deque due to the deque's support
of iteration. Also, it makes sense to use a first-in-first-out data structure
as our segments are pushed to our input byte stream in chronological order,
and we need to retransmit them in appropriate order.

My push() method is overcomplicated, incorporating states that are probably
redundant, such as a sent_syn and sent_fin flag. I also considered implementing
an alternate design using an unordered_map that mapped individual bytes since
it seemed like it could simplify some of my push() code, but I rejected the idea
since adding up individual bytes would have likely caused large performance issues.
Specifically, my implementation will run in about O(bytes_buffered / window_size)
versus O(bytes_buffered) for the individual byte implementation.
The complicated approach does cause potential bug issues down the line.

However, despite the complexity of my push() method, the time taken for each test
seems to still be pretty reasonable: 

      Start 28: send_connect
27/37 Test #28: send_connect .....................   Passed    0.01 sec
      Start 29: send_transmit
28/37 Test #29: send_transmit ....................   Passed    0.66 sec
      Start 30: send_window
29/37 Test #30: send_window ......................   Passed    0.11 sec


Report from the hands-on component: 

sha256sum /tmp/big.txt
e1f875c8df1ff00364b3f22784ae3a7e984e6ba291db017e4bebdf8083b41e74  /tmp/big.txt
cs144@cs144vm:~/minnow$ sha256sum /tmp/big-received.txt
e1f875c8df1ff00364b3f22784ae3a7e984e6ba291db017e4bebdf8083b41e74  /tmp/big-received.txt
cs144@cs144vm:~/minnow$ ls -l /tmp/big.txt
-rw-rw-r-- 1 cs144 cs144 1000000 Feb 10 18:23 /tmp/big.txt
cs144@cs144vm:~/minnow$ ls -l /tmp/big-received.txt
-rw-rw-r-- 1 cs144 cs144 1000000 Feb 10 18:32 /tmp/big-received.txt

The one-megabyte challenge was passed relatively easily, taking under 5 seconds to run 
to completion and send without any loss to my partner. I was also able to send a 100 
megabyte file to myself which also took just over 5 seconds to run. Interestingly, when 
sending to myself, the 1-MB file and the 100-MB file had a very minor difference in run
time, suggesting that the bulk of the delay is not actually due to the transmission rate
but due to overhead in setting up and tearing down the connection. 

cs144@cs144vm:~/minnow$ cmake --build build --target check_webget
Test project /home/cs144/minnow/build
    Start 1: compile with bug-checkers
1/2 Test #1: compile with bug-checkers ........   Passed    0.33 sec
    Start 2: t_webget
2/2 Test #2: t_webget .........................   Passed    0.71 sec

100% tests passed, 0 tests failed out of 2

Total Test time (real) =   1.04 sec

The check_webget portion was relatively simple. I also decided to test 
the original webget function for speed. It looks like both programs run
in about exactly the same time. This is likely because my webget program 
has a short payload, just "hi", and both programs likely have about the 
same setup time.

cmake --build build --target check_webget
Test project /home/cs144/minnow/build
    Start 1: compile with bug-checkers
1/2 Test #1: compile with bug-checkers ........   Passed    0.36 sec
    Start 2: t_webget
2/2 Test #2: t_webget .........................   Passed    0.70 sec

100% tests passed, 0 tests failed out of 2

Total Test time (real) =   1.06 sec

Implementation Challenges:
It was difficult to implement the push function elegantly.

Remaining Bugs:
Several of the send_extra tests still do not pass, but I was able to 
send extremely large files during the hands-on-component with 0 loss.
Specifically, my code is not correctly sending the FIN flag in all cases.
This is also why I considered switching to an individual byte approach.

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]

- Optional: I made an extra test I think will be helpful in catching bugs: [describe where to find]
