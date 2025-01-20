Checkpoint 1 Writeup
====================

My name: Ian Chen

My SUNet ID: ianychen

I collaborated with: allenweng

I would like to thank/reward these classmates for their help: allenweng

This lab took me about 8 hours to do. I did not attend the lab session.

I was surprised by or edified to learn that: there is actually a big difference 
between amortized O(1) time operations on an unordered_map versus O(log n)
operations on a sorted map.

Report from the hands-on component of the lab checkpoint: [include
information from 2.1(4), and report on your experience in 2.2]


--- 10.144.0.8 ping statistics ---
4444 packets transmitted, 4436 received, 0.180018% packet loss, time 46134ms
rtt min/avg/max/mdev = 2.959/12.563/140.789/20.904 ms, pipe 12

2.1.4
a. As listed above, the average round-trip delay between "echo request" 
and "echo reply" was 12.563 milliseconds.

b. The delivery rate was 99.819982%. The loss rate was 0.180018%.

c. I did not see any duplicated datagrams.

d. done, my terminal has both echo reply and request: 
06:16:31.109660 IP 10.144.0.48 > 10.144.0.8: 
ICMP echo request, id 30316, seq 4444, length 64
06:16:31.115862 IP 10.144.0.8 > 10.144.0.48: 
ICMP echo reply, id 30316, seq 4444, length 64

e. After opening up the datagrams in wireshark, I found a couple of 
the same fields as in the Internet datagram fields from lecture.
First, the wireshark datagram started with the header length in byte 0,
then the explicit congestion notification in byte 1, then two bytes for 
total length, then two bytes for identification, and two bytes for fragment 
offset.

After that, time to live was in byte 8 and protocol number was in byte 9. 
Bytes 10 and 11 are header checksum. Bytes 12-15 are source address, and 
bytes 16-19 are destination address. After that were some other options 
fields, and the actual data only started on byte 44 and ended byte 83.

f. The identification and fragment offset were different, and as a 
result the header checksums were different which makes sense. Also,
the payload checksum was also different due to my sent-out datagram 
having a type of 8 in byte 20 while the received datagram had a type 
of 0. Also, the source and destination addresses were of course switched.


2.2 
My experience in 2.2 was mostly smooth. I used almost the same code as
as in lecture with the caveat of switching out the Address variable in the
sendto function of the socket to be 10.144.0.0 as we are using the cs144
network and not Keith's local machine. I received the pings from my teammate
and sent out my own pings containing "hi".



Describe Reassembler structure and design. [Describe data structures and
approach taken. Describe alternative designs considered or tested.
Describe benefits and weaknesses of your design compared with
alternatives -- perhaps in terms of simplicity/complexity, risk of
bugs, asymptotic performance, empirical performance, required
implementation time and difficulty, and other factors. Include any
measurements if applicable.]


Data structure:
- I decided to use an std::unordered_map<uint64t, char> for this lab.
The key is the address, of type uint64t, matching the first byte address 
given to the insert function. The value is a char, a way to represent a
singular byte. The reasoning behind choosing an unordered_map is that 
I can easily use map.erase to remove any key-addresses that I don't want
in amortized O(1) time. Additionally, adding key-addresses is also 
amortized O(1). I achieved a speed of 3.04 Gbit/s for Reassembler throughput 
(no overlap) and 1.62 Gbit/s for Reassembler throughput (10x overlap).

General approach:
- I used the approach from lecture where the reassembler looks for the 
next byte it needs, and I used an uint64t instance variable to account 
for this.
- I divided the code into three main parts: three different while loops
that would handle different scenarios of input data. The first while loop
handles if the next byte needed is within the input (not necessarily at
the start of the input). Then, if so, I'll skip adding the data to my map
and just immediately push them to the writer if there is space. 
- The second part is to handle the case where having possibly changed which
byte I next want, the next byte I want could be stored inside my map already.
Therefore, the second while loop looks through my map for the current byte I
need and pushes it to the writer if there is space. This ensures that my map
is always up to date and never contains data that it should have pushed.
- The third part is to handle storing the input data inside my map. The 
while loop checks if there is enough space, and if so, stores as much of the
input as it can inside the loop.
- Besides these three main parts, I also have additional code to check for 
the is_last_substring flag. I used a bool seen_finished_ and a uint64_t 
finished_byte_ as states for this. The finished_byte_ works similarly to the
lecture code for the finished flag, treating the finished as the next byte I 
need. Therefore, if the next byte I need is equal to the finished byte, then 
the reassembler is done and everything should be closed. 

Alternative designs considered or tested:
- I first started out with a regular std::map instead of an unordered_map.
This map functioned almost identically to my unordered_map, just with worse
time complexities for all operations: since normal maps are sorted by key,
all erasure and insertion operations are O(log n), while unordered_maps 
have O(1) erasure and insertion since they are not sorted by key. Also, 
empirical performance showed that the unordered_map was better since it 
actually passed the speed test.
- An additional design I briefly considered was to use a deque. Since the
deque has built in push, pop, and indexing operations, it seemed like a 
good choice as all of its operations are O(1) as well. However, I felt that
it would be much more difficult to implement since the indexing of the deque
could actually work against me since I am working with a separate set of 
addresses for the input data. The maps do not have this issue as keys can be
any kind of index or address. Also, I thought it would be more difficult to 
implement count_bytes_pending with the deque. With maps, a call to .size() is
sufficient, but deques would be more complicated as I would have to store 
empty indices in the deque to maintain its linearly ordered structure, thus 
making a deque.size() call insufficient for count_bytes_pending.


Implementation Challenges:
The reason I needed to add a bool seen_finished_ was that I originally had
just the finished_byte_ state. In my code, I would just check if the current 
input had is_last_substring and if the finished_byte was the needed next byte.
However, this doesn't account for the case where I might have already seen the
is_last_substring flag in a previous call to insert, in which case I would 
never be able to properly close the reasemmbler and writer. Adding additional
state resolved this.

Remaining Bugs:
This implementation does not contain any kind of checksums or other
data corruption prevention/checking, so it is still vulnerable to 
corrupted data.


- Optional: I had unexpected difficulty with: deciding on a data structure

- Optional: I think you could make this lab better by: 
    giving time complexity limits (such as: O(log n) is/is not fast enough)

- Optional: I'm not sure about: what the real solution might look like
