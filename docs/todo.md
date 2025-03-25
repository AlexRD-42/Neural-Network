# Add a config structure like cfg.inputsize, cfg.numlayers, etc

# Figure out if it's worth it to use VLA to have layers be on stack
Probably not considering I won't be able to set the memory location for the layer struct

# Learn about cache locality
L1 (16~128 kb per core)
L2 (0.25~1mb per core)
L3 (2-32mb shared cores)

So, in our neural net case, will the address to each weight and bias be cached or just the starting address?

# Abandon the use of structs and use pure arrays for weights and biases
M and K variables induce padding from 24 bytes to 32

# Create AVX instrutions for thread safe RNG creation
This is going to be a pain. The pRNG functions are great, but they depend on state variables that need to be updated sequentially, completely violating thread safety.
Also, casting the output to a float seems to kill performance.
I see several solutions:
* RNG function takes more parameters that scramble the state, like `i` in a parallel for, or thread number
* Figure out how many threads `n` will be used and create `n` states (or `n/7` considering each state can generate 7 floats)
* Asynchronous state generator (**overengineered**)
	Almost a separate program, defined with a maximum amount of states (or dynamic state sizing depending on a running average of how many floats were requested)
	Input requests X amount of floats, program returns per Thread calls
	Each thread is responsible for keeping track of when it needs to shuffle (i.e. outputted 7 floats or custom rules like if outputted 1 float after a while reset)
	Each thread would automatically return an AVX vector of floats

# Change the RNG function to return 32 bit float
