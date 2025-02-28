Checkpoint 6 Writeup
====================

My name: Ian Chen

My SUNet ID: ianychen

I collaborated with: 

I would like to thank/reward these classmates for their help: 

This checkpoint took me about 4 hours to do. I did not attend the lab session.

Program Structure and Design of the Router [Describe data
structures and approach taken. Describe alternative designs considered
or tested.  Describe benefits and weaknesses of your design compared
with alternatives -- perhaps in terms of simplicity/complexity, risk
of bugs, asymptotic performance, empirical performance, required
implementation time and difficulty, and other factors. Include any
measurements if applicable.]: 

My approach is very simple, using a vector<Route> to store all route info.
The Route is a custom struct that simply holds the route_prefix, prefix_length,
next_hop, and interface_num. I believed this was by far the easiest way
in terms of implementation difficulty as my add_route function is 
just filling in the blanks of a Route and adding it to the vector. The
downside of this is that when doing my route() function, I must look through
every single route for every single datagram, leading to O(n) time to 
send a single datagram. In all, my code ran in 0.04 seconds on the router test.
This implementation is also very bug free as it is difficult to mess up 
iterating through a vector compared to the alternate implementation I tried.

My first alternate implementation tried to use an unordered_map with key int
and value vector<Route>. The key represents the prefix_length. This is helpful
because the ultimate goal is to return the Route with largest prefix length,
so this speeds up iteration as we don't need to go through every route; we
can instead start at prefix length 32 and work our way downwards. While in 
terms of asymptotic complexity this is still O(n) per route, it does fewer
operations overall. However, running this resulted in 0.05 seconds. This
could be due to simply not having an appropriate test that works well with 
this strategy (some kind of adversarial input). This wasn't all that 
difficult to implement since I could retain almost all of my previous code.

The other alternate approach that I did not try to implement was to take
a reverse approach: instead of storing routes themselves, we can instead 
store final destination addresses that map to routes. So, the unordered_map
would have 2^32 keys, one for each address, all initially mapped to NULL.
For each route we add, we would update all addresses in the range of the route
if the route has a longer prefix length that the current route mapped to that 
address. This makes the actual route() for each datagram O(1) time, and 
adding routes is also O(1) time as the max number of operations is 2^32. 
This is more difficult to implement that my current approach, however, and I 
doubt that the performance would be good at all since 2^32 operations, 
despite being O(1), is going to take an extremely longe time to complete.
As a result, I decided not to fully implement this approach.

I also decided to make a match(dgram, route) helper function to 
make the implementation cleaner inside route().


Implementation Challenges:
I forgot that decrementing the TTL means that I also need to recompute 
the checksum which was difficult to debug.

Remaining Bugs:
None

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
