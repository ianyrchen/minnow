Checkpoint 2 Writeup
====================

My name: Ian Chen

My SUNet ID: ianychen

I collaborated with: tt105241

I would like to thank/reward these classmates for their help: tt105241

This lab took me about 9 hours to do. I did attend the lab session.

Describe Wrap32 and TCPReceiver structure and design. [Describe data
structures and approach taken. Describe alternative designs considered
or tested.  Describe benefits and weaknesses of your design compared
with alternatives -- perhaps in terms of simplicity/complexity, risk
of bugs, asymptotic performance, empirical performance, required
implementation time and difficulty, and other factors. Include any
measurements if applicable.]

Wrap32:
The wrap function is simple, using n % ( 1ULL << 32 ) to compute the 
remainder of n when divided by 2^32 to keep the result within the range
of uint32_t. Then, this result is added to zero_point. This is O(1). 
This design is as simple as possible and is easy to implement.

The unwrap function is divided into two cases:
If the wrapped value is more than 2^31 away from the checkpoint, then
I must add/subtract 2^32 depending on if checkpoint is greater than the
absolute seqno. Otherwise, I can return the absolute seqno plus the 
bits of the checkpoint >= 2^32. This also runs in O(1). While this 
design has good asymptotic performance, it was also a little difficult 
to implement and account for all boundary cases, such as when the checkpoint
is near 0 or UINT64_MAX.

Alternate implementations: I also tried an implementation inspired by the
"dumb" approaches hinted at by Keith: I used a for loop and added 2^32 
to my absolute seqno until it was as close as possible to the checkpoint. 
However, this is clearly too slow as it runs in O(n / 2^32) = O(n) time;
for checkpoints near 2^64, this would take at least 2^32 operations which 
is unfeasible and timed out on the recv_reorder_more test.

TCPReceiver:
For the state of my TCPReceiver, besides the given reassembler_, I
included an optional<Wrap32> isn_, storing the initial sequence number.
I also originally had an additional state, bool rst_err_, to check 
if msg.RST was true, but I realized that this is redundant due to the 
reader/writer having get_error() and set_error(). I also originally had
an optional<Wrap32> ackno_ state, but I realized I could replace this 
entirely with the writer.bytes_pushed() function as I always need the
byte after the last one I have pushed so far. The last redundant state 
I was able to get rid of was a bool fin_ checker that I replaced by 
just checking if writer.is_closed() in send(). In general, removing
these redundant states cleaned up my code significantly and reduced
the amount of code necessary while also handling appropriate edge cases,
reducing the risk of bugs.

My implementation passed recv_reorder_more in 3.22 seconds. Previously,
I was timing out with a previous version of reassembler. I changed to 
Keith's code from lab which was fast enough to pass and result in the 
current time. (My reassembler previously was ~1 gbit/s, now reaching 47).


Implementation Challenges:
My reassembler expects the first data byte to be index 0, so I had to convert
absolute seqno to stream index when inserting to the reassembler.

I also initially struggled with how the RST flag worked.

I also timed out on recv_reorder_more many times until I realized that I 
needed to take out my debug() calls.

Remaining Bugs:
none

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]

- Optional: I made an extra test I think will be helpful in catching bugs: [describe where to find]
