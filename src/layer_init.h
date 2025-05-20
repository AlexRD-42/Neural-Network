#include "helper.h"

// f32 (*weight_ptr)[N] = (f32 (*)[N]) weight;
typedef struct s_network_cfg
{
	u32 M; // Batch Size
	u32 numLayers;
	u32 *layer;

} network_cfg;

typedef struct s_std_layer
{
	//           Input        Weights       Output    +   Biases 
	//        ( M  x  K ) * ( K  x  N ) = ( M  x  N ) + ( N  x  1 )
	// MNIST: (600 x 784) * (784 x 100) = (600 x 100) + (100 x  1 )
	f32 *weight, *bias, *output;
	u32 M; 	// Batch size
	u32 N; 	// Parameter count of current Layer (L)
	u32 K; 	// Parameter count of previous Layer (L - 1)

} std_layer;

std_layer init_std_layer(u32 M, u32 N, u32 K, f32 *parameters)
{
	u32 i = 0, j = 0;

	f32 *bias = parameters; // Size N x 1
	f32 *weight = &parameters[N]; // Size N x K
	f32 *output = &parameters[N * (K + 1)]; // Size M x N

	for (i = 0; i < N; i++)
		bias[i] = 0.0f;

	float limit = sqrtf(6.0f / (K + N));
	for (i = 0; i < K; i++)
		for (j = 0; j < N; j++)
			weight[i * N + j] = ft_rand() * limit;
	
	for (i = 0; i < M * N; i++)
		output[i] = 0.0f;

	return (std_layer) {.weight = weight, .bias = bias, .output = output, .M = M, .N = N, .K = K};
}

std_layer *initialize_layer(network_cfg cfg)
{
	u32 i = 0;
	u32 K = 0, N = 0;
	u64 totalMemory = 0, offset = 0;

	// Calculates total memory required for ALL of the networks parameters
	for (i = 0; i < cfg.numLayers; i++)
	{
		K = cfg.layer[i];
		N = cfg.layer[i + 1];
		totalMemory += N * (cfg.M + K + 1) * sizeof(f32);
	}

	std_layer *network = (std_layer *) malloc(cfg.numLayers * sizeof(std_layer));
	if (network == NULL)
		return (NULL);

	// To allocate everything contiguously in memory
	f32 *parameters = (f32 *) malloc(totalMemory);
	if (parameters == NULL)
	{
		free(network); 
		return (NULL);
	}

	for (i = 0; i < cfg.numLayers; i++)
	{
		K = cfg.layer[i];
		N = cfg.layer[i + 1];
		network[i] = init_std_layer(cfg.M, N, K, &parameters[offset]);
		offset += N * (cfg.M + K + 1); // Offset is the parameter count of the previous layer
	}
	return(network);
}