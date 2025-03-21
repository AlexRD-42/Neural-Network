#include <stdlib.h>
#include <stdio.h>
#include <cblas.h>

typedef unsigned char u8;
typedef unsigned int u32;
typedef int i32;
typedef float f32;
typedef long long i64;
typedef unsigned long long u64;

u8 *read_file(char *filename, i64 *size, i64 start)
{
	u8 *str = NULL;
	FILE *fp;

	*size = 0;
	fp = fopen(filename, "rb");
	if (fp == NULL) 
		return (NULL);
	fseek(fp, 0, SEEK_END);
	*size = ftell(fp);
	if (*size > start)
	{
		*size -= start;
		fseek(fp, start, SEEK_SET);
		str = (u8 *) malloc(*size);
		if (str != NULL)
			fread(str, 1, *size, fp);
	}
	fclose(fp);
	return (str);
}

int main()
{
	i64 tdata_size = 0;
	u8 *tdata = read_file("./data/mnist-digits/train-images.idx3-ubyte", &tdata_size, 16);

	i64 vdata_size = 0;
	u8 *vdata = read_file("./data/mnist-digits/t10k-images.idx3-ubyte", &vdata_size, 16);

	i64 tlabel_size = 0;
	u8 *tlabel = read_file("./data/mnist-digits/train-labels.idx1-ubyte", &tlabel_size, 8);

	i64 vlabel_size = 0;
	u8 *vlabel = read_file("./data/mnist-digits/t10k-labels.idx1-ubyte", &vlabel_size, 8);

	// u8 (*str_array)[28][28] = (u8 (*)[28][28]) str;
}