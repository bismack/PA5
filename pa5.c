#include <stdio.h>
#include <cuda.h>
#include "ppmFile.h"

#define XBLOCK 32
#define YBLOCK 32
#define CUDA_CHECK_RETURN(value) {                                          \
    cudaError_t _m_cudaStat = value;                                        \
    if (_m_cudaStat != cudaSuccess) {                                       \
        fprintf(stderr, "Error %s at line %d in file %s\n",                 \
                cudaGetErrorString(_m_cudaStat), __LINE__, __FILE__);       \
        exit(1);                                                            \
    } }

using namespace std;

struct INPUT {
	int blurRadius;
	char *inputFilename;
	char *outputFilename;
} userParam;

__global__ void comp(unsigned char *image, int rad, int width, int height) {
	//__shared__ Image *imgcp;
	/*int myID = 	( blockIdx.y * gridDim.x +
			blockIdx.x ) * blockDim.x * blockDim.y +  
			threadIdx.y * blockDim.x +
			threadIdx.x;*/
	int xLoc=threadIdx.x+XBLOCK*blockIdx.x;
	int yLoc=threadIdx.y+YBLOCK*blockIdx.y;
	int minWidth = ((xLoc-rad)>0)?(xLoc-rad):0;
	int maxWidth = ((xLoc+rad)<width)?(xLoc+rad):width;
	int minHeight = ((yLoc-rad)>0)?(yLoc-rad):0;
	int maxHeight = ((yLoc+rad)<height)?(yLoc+rad):height;	
	if (xLoc<=maxWidth&yLoc<=maxHeight) {
	int RGB[3]={0,0,0};
	int count=0;
	for (int j=minHeight; j<=maxHeight; j++) {
		for (int i=minWidth; i<=maxWidth; i++) {
			RGB[0] += image[(j * width + i) * 3];
			RGB[1] += image[(j * width + i) * 3+1];
			RGB[2] += image[(j * width + i) * 3+2];
			count++;
		}
	}
	int offset = (yLoc * width + xLoc) * 3;
	image[offset] = RGB[0]/count;
	image[offset+1] = RGB[1]/count;
	image[offset+2] = RGB[2]/count;
	}
	//printf("---R:%i G:%i B:%i\n", image[offset], image[offset+1], image[offset+2]);

	//printf("x:%i y:%i ||| minW:%i maxW:%i minH:%i maxH:%i ||| R:%i G:%i B:%i\n", xLoc, yLoc, minWidth, maxWidth, minHeight,maxWidth,RGB[0],RGB[1],RGB[2]);
}
int main(int argc, char *argv[]) {
	printf("----Program start\n");

	userParam.blurRadius = atoi(argv[1]);
	userParam.inputFilename = argv[2];
	userParam.outputFilename = argv[3];

	Image *image = ImageRead(userParam.inputFilename);
	int size=image->width*image->height*3;
	Image *image_d = (Image *) malloc(sizeof(Image));
	image_d->data = (unsigned char*) malloc(size);

	int x=image->width/XBLOCK; 
	int y=image->height/YBLOCK;
	dim3 b(XBLOCK,YBLOCK);
	dim3 g(x+1,y+1);

	CUDA_CHECK_RETURN(cudaMalloc((void **) &image_d->data, size));
	CUDA_CHECK_RETURN(cudaMemcpy(image_d->data, image->data, size, cudaMemcpyHostToDevice));
	//printf("---R:%i G:%i B:%i\n", image->data[0], image->data[1], image->data[2]);
	comp<<<g,b>>>(image_d->data,userParam.blurRadius,image->width,image->height);

	CUDA_CHECK_RETURN(cudaDeviceSynchronize());
	CUDA_CHECK_RETURN(cudaGetLastError());
	CUDA_CHECK_RETURN(cudaMemcpy (image->data, image_d->data, size, cudaMemcpyDeviceToHost));
	
	ImageWrite(image, userParam.outputFilename);
	CUDA_CHECK_RETURN (cudaFree((void *) image_d->data));
	delete image;
	return 1;
}
