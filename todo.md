# Add a config structure like cfg.inputsize, cfg.numlayers, etc

# Make the weights and biases close to each other in memory
This would require performing one huge memory allocation for all the weights and biases, 
then passing this memory address for the layer init function to return the appropriate pointer

Example:
Structure is {784, 100, 50, 10}, `numLayers` = 3
`Weights`: 100x784, 50x100, 10x50 	(Total Memory: `83900`)
`Biases`: 100, 50, 10 				(Total Memory: `160`)
`B1` = PTR
`B2` = (PTR + 100)
`B3` = (PTR + 150) or (B2 + 50)

# Figure out if it's worth it to use VLA to have layers be on stack