#include "helper.h"

std_layer init_std_layer(u32 K, u32 M)
{
	u32 i = 0, j = 0;
	f32 *weight = (f32 *) malloc (M * K * sizeof(f32));
	f32 *bias = (f32 *) malloc(M * sizeof(f32));

	if (weight == NULL || bias == NULL)
	{
		if (weight != NULL) free (weight);
		if (bias != NULL) free (bias);
		return (std_layer)
		{
			.weight = NULL,
			.bias = NULL,
			.M = 0,
			.K = 0
		};
	}

	for (i = 0; i < M ; i++)
	{
		bias[i] = 0;
		for (j = 0; j < K; j++)
			weight[i * K + j] = ft_rand() * 2 - 1;
	}

	return (std_layer)
	{
		.weight = weight,
		.bias = bias,
		.M = M,
		.K = K
	};
}

void initialize_layer(std_layer *layers, u32 numLayers, u32 *layerStructure)
{
	u32 i = 0;
	u32 K = 0, M = 0;

	for (i = 0; i < numLayers; i++)
	{
		K = layerStructure[i];
		M = layerStructure[i + 1];
		layers[i] = init_std_layer(K, M);
	}
}

int main()
{
	u32 numLayers = 3;
	u32 layerStructure[] = {784, 100, 50, 10};
	std_layer network[numLayers];

	initialize_layer(network, numLayers, layerStructure);

	std_layer *layer1 = &network[0];  // First layer
	std_layer *layer2 = &network[1];  // Second layer

	printf("Memory distance: %td bytes\n", (char*)layer2 - (char*)layer1);
}