#include<stdio.h>
#include<math.h>
#include <opencv2/opencv.hpp>
#include <time.h>

//variable for cuda
unsigned char *d_orgImage,*d_bicImage;
int *d_orgWidthStep,*d_bicWidthStep;
int *d_width_t,*d_height_t,*d_width,*d_height;
int *d_ratiox,*d_ratioy;

__device__ float caculateMatrix(float *su,float *sv , unsigned char B[][4]){
	float BC[4]={0};
	float ABC=0;
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			BC[i] += ((float)B[i][j])*su[j];			
		}
	}
	for(int i=0;i<4;i++){
		ABC+= sv[i]*BC[i];
	}
	return ABC;
}

__global__ void scaleInterpolate(unsigned char *image, unsigned char *image_bic, int *pwidthStep, int *pwidthStep_bic, int *pwidth_t, int *pheight_t, int *pwidth, int *pheight, int *pratiox, int *pratioy){
	int part = blockDim.x * blockIdx.x + threadIdx.x;
	int widthStep=*pwidthStep, widthStep_bic=*pwidthStep_bic;
	int width_t=*pwidth_t, height_t=*pheight_t;
	int width=*pwidth, height=*pheight;
	int ratiox=*pratiox, ratioy=*pratioy;
	int i,j,k,l;
	int bicx,bicy;
	float px,py;
	float deltax[4],deltay[4];
	float su[4],sv[4];
	unsigned char BMat[4][4]={0};
	unsigned char GMat[4][4]={0};
	unsigned char RMat[4][4]={0};
	float Bv,Gv,Rv;
	if(part < width_t*height_t)
	{
	i = part%width_t;
	j = part/width_t;
	px = (float)i;
	py = (float)j;
//scaling
	px=px/(float)ratiox;
	py=py/(float)ratioy;
	if(px > width-1 )px=(float)width-1;
	if(py > height-1)py=(float)height-1;			
//interpolation
	if((int)floor(px)<width && (int)floor(py)< height && (int)floor(px)>=0 && (int)floor(py)>=0)
	{
//bicubic
		bicx = (int)floor(px);
		bicy = (int)floor(py);
		if((int)floor(px)==0)bicx=1;
		if((int)floor(py)==0)bicy=1;
		if((int)floor(px)>=width-3) bicx=width-3;
		if((int)floor(py)>=height-3) bicy=height-3;
		deltax[0] = fabs(px- (bicx-1) );
		deltax[1] = fabs(px-bicx);
		deltax[2] = fabs(px-(bicx+1));
		deltax[3] = fabs(px-(bicx+2));
		deltay[0] = fabs(py-(bicy-1));
		deltay[1] = fabs(py-bicy);
		deltay[2] = fabs(py-(bicy+1));
		deltay[3] = fabs(py-(bicy+2));
		for(k=0;k<4;k++){
			if(  (int)floor(deltax[k])==0  ){
				su[k]=1-2*pow(deltax[k],2)+pow(deltax[k],3);
			}
			else if(  (int)floor(deltax[k])==1  ){
				su[k]=4- 8*deltax[k]+ 5*pow(deltax[k],2) - pow(deltax[k],3);
			}
			else su[k]=0;
			if(  (int)floor( deltay[k])==0  ){
				sv[k]=1-2*pow(deltay[k],2)+pow(deltay[k],3);
			}
			else if(  (int)floor( deltay[k])==1  ){
				sv[k]=4- 8*deltay[k]+ 5*pow(deltay[k],2) - pow(deltay[k],3);
			}
			else sv[k]=0;
		}
		for(k=0;k<4;k++){
			for(l=0;l<4;l++){
				BMat[k][l] = image[ (bicy+k-1) *widthStep + (bicx+l-1)*3   ];
				GMat[k][l] = image[ (bicy+k-1) *widthStep + (bicx+l-1)*3 +1];
				RMat[k][l] = image[ (bicy+k-1) *widthStep + (bicx+l-1)*3 +2];
			}
		}
		Bv = caculateMatrix(su,sv,BMat);
		Gv = caculateMatrix(su,sv,GMat);
		Rv = caculateMatrix(su,sv,RMat);
		if(Bv > 255)Bv=255;
		else if(Bv<0)Bv=0;
		if(Gv > 255)Gv=255;
		else if(Gv<0)Gv=0;
		if(Rv > 255)Rv=255;
		else if(Rv<0)Rv=0;
		image_bic[j*widthStep_bic + i*3   ]=(int)Bv;
		image_bic[j*widthStep_bic + i*3 +1]=(int)Gv;
		image_bic[j*widthStep_bic + i*3 +2]=(int)Rv;
	}
	}
}

void transform(IplImage *image){
	IplImage *image_bic;
	int ratiox = 2,ratioy = 2;
	int width_t = image->width*ratiox;
	int height_t = image->height*ratioy;
	CvSize ImageSize1 = cvSize(width_t , height_t);
	image_bic = cvCreateImage(ImageSize1,IPL_DEPTH_8U,3);
	
	//allocate cuda memory
	cudaMalloc((void**)&d_orgImage, image->imageSize);
	cudaMalloc((void**)&d_bicImage, image_bic->imageSize);
	cudaMalloc((void**)&d_orgWidthStep, sizeof(int));
	cudaMalloc((void**)&d_bicWidthStep, sizeof(int));
	cudaMalloc((void**)&d_width_t, sizeof(int));
	cudaMalloc((void**)&d_height_t, sizeof(int));
	cudaMalloc((void**)&d_width, sizeof(int));
	cudaMalloc((void**)&d_height, sizeof(int));
	cudaMalloc((void**)&d_ratiox, sizeof(int));
	cudaMalloc((void**)&d_ratioy, sizeof(int));
	//copy memory from cpu to cuda
	cudaMemcpy(d_orgImage, image->imageData, image->imageSize, cudaMemcpyHostToDevice);
	cudaMemcpy(d_orgWidthStep, &image->widthStep, sizeof(int), cudaMemcpyHostToDevice);
	cudaMemcpy(d_bicWidthStep, &image_bic->widthStep, sizeof(int), cudaMemcpyHostToDevice);
	cudaMemcpy(d_width_t, &width_t, sizeof(int), cudaMemcpyHostToDevice);
	cudaMemcpy(d_height_t, &height_t, sizeof(int), cudaMemcpyHostToDevice);
	cudaMemcpy(d_width, &image->width, sizeof(int), cudaMemcpyHostToDevice);
	cudaMemcpy(d_height, &image->height, sizeof(int), cudaMemcpyHostToDevice);
	cudaMemcpy(d_ratiox, &ratiox, sizeof(int), cudaMemcpyHostToDevice);
	cudaMemcpy(d_ratioy, &ratioy, sizeof(int), cudaMemcpyHostToDevice);
	// Invoke kernel
    int threadsPerBlock = 256;
    int blocksPerGrid = (width_t*height_t + threadsPerBlock - 1) / threadsPerBlock;
    scaleInterpolate<<<blocksPerGrid, threadsPerBlock>>>(d_orgImage, d_bicImage, d_orgWidthStep, d_bicWidthStep, d_width_t, d_height_t, d_width, d_height, d_ratiox, d_ratioy);
	//copy result from cuda to cpu
	cudaMemcpy(image_bic->imageData, d_bicImage, image_bic->imageSize, cudaMemcpyDeviceToHost);

	cvShowImage("bicubic",image_bic);
	cvSaveImage("bicubic.tif",image_bic);
}

int main(void){	
	clock_t clockbegin,clockend;
	clockbegin = clock();
	IplImage *image=cvLoadImage("test4.tif");
	transform(image);
	clockend = clock();
	printf("time spend: %ld ms\n",clockend - clockbegin);
	//cvShowImage("QQ",image);
	//cvSaveImage("QQ.tif",image);
	cvWaitKey(0);
	return 0;
}
