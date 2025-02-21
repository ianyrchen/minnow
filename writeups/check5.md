Checkpoint 5 Writeup
====================

My name: Ian Chen

My SUNet ID: ianychen

I collaborated with: 

I would like to thank/reward these classmates for their help: daxiong

This checkpoint took me about [6] hours to do. I [did not] attend the lab session.

Program Structure and Design of the NetworkInterface [Describe data
structures and approach taken. Describe alternative designs considered
or tested.  Describe benefits and weaknesses of your design compared
with alternatives -- perhaps in terms of simplicity/complexity, risk
of bugs, asymptotic performance, empirical performance, required
implementation time and difficulty, and other factors. Include any
measurements if applicable.]:

I decided to use several unordered_maps as my data structure of choice.
This makes a lot of sense to store the mappings from IP addresses to 
Ethernet addresses with average O(1) insertion and deletion. This
fits the requirements of only storing the IP/Ethernet mappings for 30 
seconds as the data structure must support many possible deletions and 
insertions. The value of my unordered_map was a pair of {Ethernet address,
time left in ms}. This allows for much easier alteration of both values
in the same location versus having two separate containers. 

One interesting test I did was having regular sorted maps instead of 
unordered_maps. Theoretically, this should be quite a bit slower when
erasing and inserting than unordered_maps since sorted maps are O(log n)
in both operations, but after running, both had the exact same total
running time (0.05 sec for net_interface and 0.01 for no_skip). This is
likely because the test cases are not large enough to show a large difference
in runtimes. In a real world use case, normal computers probably also 
do not have enough IP mappings to mandate unordered_maps over normal maps
as they forward most things to their router.

The other states I included were two more unordered_maps. One, not_sent_, has
key uint32_t for the IP address and value as a queue of InternetDatagrams,
storing the datagrams that we don't know the destination Ethernet address of yet.
The other also has key uint32_t for IP addresses and size_t value for the amount
of time left, representing the ARP request timer of 5 seconds to not send more 
requests about the same IP within 5 seconds. 

One design choice I considered was to have all of these things in the same 
unordered_map as they are all dependent on the same IP addresses. An 
implementation that I considered was making a struct and having all four of 
these values (Ethernet, expiration, queue, ARP timeout) in the struct. However
I judged the implementation of this to be more complicated that it was worth
having less memory usage. Also, the memory saved is minimal anyways since as 
mentioned above, most computers probably don't have many IP mappings anyways.

Implementation Challenges:
Initially, my unordered_map had the Address as a key. However,
this wouldn't compile as Addresses are not hashable, so I had to
switch to uint32_t, the raw format of the Address as the hashable key.

I also got confused by the handout on the part about the 5 second 
ARP request timeout (they are supposed to be dropped if we don't
get a response), but Ed responses helped me.

Remaining Bugs:
None

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
