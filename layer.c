#include "helper.h"

// f32 (*weight_ptr)[M] = (f32 (*)[M]) weight;
typedef struct s_std_layer
{
	//          Weights        Input           Output     +    Biases 
	//          (M x K)   *   (K x N)     =   (M x N)     +   (M x 1)
	// MNIST: (100 x 784) * (784 x 60000) = (100 x 60000) + (100 x 1)
	f32 *weight, *bias, *output;
	u32 M, K;

} std_layer;

std_layer init_std_layer(u32 K, u32 M, f32 *parameters)
{
	f32 *weight = &parameters[M];
	f32 *bias = parameters;
	u32 i = 0, j = 0;

	for (i = 0; i < M ; i++)
	{
		bias[i] = 0;
		for (j = 0; j < K; j++)
			weight[i * K + j] = 1; // ft_rand() * 2 - 1;
	}

	return (std_layer) {.weight = weight, .bias = bias, .M = M, .K = K};
}

std_layer *initialize_layer(u32 numLayers, u32 *layerStructure)
{
	u32 i = 0;
	u32 K = 0, M = 0;
	u64 totalMemory = 0, offset = 0;

	for (i = 0; i < numLayers; i++)
	{
		K = layerStructure[i];
		M = layerStructure[i + 1];
		totalMemory += M * (K + 1) * sizeof(f32);
	}

	std_layer *network = (std_layer *) malloc(numLayers * sizeof(std_layer));
	if (network == NULL)
		return (NULL);
	f32 *parameters = (f32 *) malloc(totalMemory);
	if (parameters == NULL)
	{
		free(network); 
		return (NULL);
	}
	for (i = 0; i < numLayers; i++)
	{
		K = layerStructure[i];
		M = layerStructure[i + 1];
		network[i] = init_std_layer(K, M, &parameters[offset]);
		offset += M * (K + 1);
	}
	return(network);
}

// N here is the previous layers K
// Check to see if blas adds or replaces the array passed by reference (beta = 1)
f32 *layer_pass(f32 *input, u32 N, std_layer layer)
{
	layer.output = layer.bias;
	cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
		layer.M, N, layer.K, 1.0,
		input, layer.K, layer.weight, N, 0.0, layer.output, N);
}

int main()
{
	u32 layerStructure[] = {16, 8, 4, 2};
	u32 numLayers = 3;
	std_layer *network = initialize_layer(numLayers, layerStructure);

	std_layer *layer1 = &network[1];  // First layer
	std_layer *layer2 = &network[2];  // First layer
	// f32 *layer2 = network[0].bias;  // Second layer

	printf("Memory distance: %td bytes\n", (char*)layer2 - (char*)layer1);
}