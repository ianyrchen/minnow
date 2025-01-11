Checkpoint 0 Writeup
====================

My name: Ian Chen

My SUNet ID: ianychen

I collaborated with: [list sunetids here]

I would like to credit/thank these classmates for their help: [allenweng]

This lab took me about [6] hours to do. I [did not] attend the lab session.

My secret code from section 2.1 was: 654022

I was surprised by or edified to learn that: Anyone can impersonate an email address with little oversight.

Describe ByteStream implementation. [Describe data structures and
approach taken. Describe alternative designs considered or tested.
Describe benefits and weaknesses of your design compared with
alternatives -- perhaps in terms of simplicity/complexity, risk of
bugs, asymptotic performance, empirical performance, required
implementation time and difficulty, and other factors. Include any
measurements if applicable.]

I used a deque of characters for my ByteStream. Characters make sense because they are each one byte. I initially used a queue due to its ability to have first-in-first-out pushing and popping, but this did not allow for easy peeking of multiple elements. Deques offer iteration, still with O(1) FIFO push/pop operations. Initially, my peek() function also iterated through the entire deque, but I set a const limit to the peek() length to pass the speed tests. Overall, my push() and pop() operations are both O(n) where n is the length of the input, and my peek() runs in O(1). All of my other functions are also O(1) as they just check the value of some instance variables. I also made sure to account for several edge cases: my code limits the push/pop length to the size of the ByteStream, so no extra pushes or pops will be done. In addition, if the ByteStream is smaller than my peek length constant, my code will not trigger an index out of bounds error. My ByteStream was able to hit a minimum speed of 1.33 Gbit/s. 


- Optional: I had unexpected difficulty with: Figuring out what the various functions of the socket did (such as bind, listen, etc), turns out I didn't need them.

- Optional: I think you could make this lab better by: [describe]

- Optional: I'm not sure about: [describe]

- Optional: I contributed a new test case that catches a plausible bug
  not otherwise caught: [provide Pull Request URL]
