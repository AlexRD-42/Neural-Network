#include "layer_init.h"

static inline void sigmoid(f32 *array, u32 size)
{
	for (u32 i = 0; i < size; i++)
	{
		array[i] = 1 / (1 + expf(-array[i]));
	}
}

// C = αAB + βC
// C (M x N) = A (M x K) * B (K x N)
void layer_pass(f32 *input, std_layer layer)
{
	for (u32 i = 0; i < layer.M; i++)
		memcpy(layer.output + i * layer.N, layer.bias, layer.N * sizeof(f32));

	cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, layer.M, layer.N, layer.K, 1.0f, input, layer.K, layer.weight, layer.N, 1.0f, layer.output, layer.N);
	sigmoid(layer.output, layer.N * layer.M);
}

void forward_pass(f32 *input, std_layer *network, network_cfg cfg)
{
	layer_pass(input, network[0]);
	for (u32 i = 1; i < cfg.numLayers; i++)
		layer_pass(network[i - 1].output, network[i]);
}

void backward_pass(std_layer layer)
{
	
}

void print_layer(std_layer layer, float *ptr)
{
	u64 numElements = layer.N * (layer.M + layer.K + 1);
	for (u32 i = 0; i < numElements; i++)
	{
		printf("%.3f,\t", ptr[i]);
		if ((i + 1) % layer.N == 0)
			printf("\n");
	}
}

int main()
{
	u32 lstruc[4] = {16, 8, 4, 2};
	u32 i = 0;
	network_cfg cfg = {.M = 10, .numLayers = 3, .layer = lstruc};
	std_layer *network = initialize_layer(cfg);

	f32 *input = malloc(16 * 10 * sizeof(f32));
	for (i = 0; i < 160; i++)
		input[i] = ((float) i)/1000;

	layer_pass(input, network[0]);
	layer_pass(network[0].output, network[1]);
	layer_pass(network[1].output, network[2]);
	print_layer(network[2], network[2].bias);
}