#include<stdio.h>
#include<math.h>
#include <opencv2/opencv.hpp>
#include <opencv/highgui.h>

int round(float x){
	if(fmod(x,1)<5)return (int)floor(x);
	else return (int)ceil(x);
}

void MatrixMultiply(float *co, float *out , float matrix[][3]){

	out[0]=0;
	out[1]=0;
	out[2]=0;
	for(int i=0;i<3;i++){
		for(int j=0;j<3;j++){
			out[i]+= matrix[j][i] * co[j];
		}		
	}
}

float caculateMatrix(float *su,float *sv , unsigned char B[][4]){

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

void transform(IplImage *image){
	
	IplImage *image_bic;
	int sa=0,ro=0,ta=0,sh=0;		
	int i,j,k,l;					
	int bicx,bicy;					
	float PI=3.141592653;			
	float px,py;					
	float coordinate[3]={1,1,1};
	float coorinateNew[3]={0};

	float deltax[4],deltay[4];		
	float su[4],sv[4];
	unsigned char BMat[4][4]={0};
	unsigned char GMat[4][4]={0};
	unsigned char RMat[4][4]={0};
	float Bv,Gv,Rv;	

	float radiox = 1,radioy = 1;
	float angle=0;	
	float distx=0,disty=0;
	float slopex=0,slopey=0;
	int originx=image->width/2 ,originy=image->height/2;

	sa = 1;
	radiox=5;
	radioy=5;
	float sint=sin(angle* ( PI / 180)),cost=cos(angle* ( PI / 180));
	float scalingMatrix[3][3]={{1/radiox , 0        , 0},
							   {0        , 1/radioy , 0},
							   {0        , 0        , 1}};	
	
	float rotateMatrix[3][3]={{cost ,-sint , 0},
							  {sint , cost , 0},
							  {0    , 0    , 1}};

	float translateMatrix[3][3]={ {1	  , 0	   , 0},
								  {0	  , 1	   , 0},
								  {-distx ,-disty  , 1}};	
	float shearMatrix[3][3]={{1		  , -slopey , 0},
							 {slopex , 1       , 0},
							 {0       , 0       , 1}};	

	int width_t = image->width*radiox;
	int height_t = image->height*radioy;

	CvSize ImageSize1 = cvSize(width_t , height_t);
	image_bic = cvCreateImage(ImageSize1,IPL_DEPTH_8U,3);


	unsigned char *B,*G,*R;
	int size=image->imageSize;
	int height = image->height;
	int width = image->width;
	B = (unsigned char*)malloc(sizeof(unsigned char)*size);
	G = (unsigned char*)malloc(sizeof(unsigned char)*size);
	R = (unsigned char*)malloc(sizeof(unsigned char)*size);

	for(j = 0;j<height;j++){
		for(i=0;i<width;i++){
			B[j*width+i]=image->imageData[ j*image->widthStep + i*3  ];
			G[j*width+i]=image->imageData[ j*image->widthStep + i*3+1];
			R[j*width+i]=image->imageData[ j*image->widthStep + i*3+2];
		}
	}

	
	for(j=0;j<height_t;j++){
		for(i=0;i<width_t;i++){
			px = (float)i;
			py = (float)j;
			coorinateNew[0]=coordinate[0];
			coorinateNew[1]=coordinate[1];			
//scaling
			if(sa==1){
				coordinate[0]=(float)px;
				coordinate[1]=(float)py;

				MatrixMultiply(coordinate,coorinateNew,scalingMatrix);

				px=coorinateNew[0];
				py=coorinateNew[1];	

				if(px > image->width-1 )px=(float)image->width-1;
				if(py > image->height-1)py=(float)image->height-1;
			}
			px -= (float)originx;
			py -= (float)originy;
//translate		
			if(ta==1){
				coordinate[0]=px;
				coordinate[1]=py;
				MatrixMultiply(coordinate,coorinateNew,translateMatrix);
				px=coorinateNew[0];
				py=coorinateNew[1];	
			}
//shear
			if(sh==1){
				coordinate[0]=px;
				coordinate[1]=py;

				MatrixMultiply(coordinate,coorinateNew,shearMatrix);
				px=coorinateNew[0];
				py=coorinateNew[1];	
			}
//rotation	
			if(ro==1){
				coordinate[0]=px;
				coordinate[1]=py;
				MatrixMultiply(coordinate,coorinateNew,rotateMatrix);
				px=coorinateNew[0];
				py=coorinateNew[1];	
			}

			px += (float)originx;
			py += (float)originy;
//interpolation
			if((int)floor(px)<image->width && (int)floor(py)< image->height && (int)floor(px)>=0 && (int)floor(py)>=0)
			{	

//bicubic
				bicx = (int)floor(px);
				bicy = (int)floor(py);
				if((int)floor(px)==0)bicx=1;
				if((int)floor(py)==0)bicy=1;
				if((int)floor(px)>=image->width-3)bicx=image->width-3;
				if((int)floor(py)>=image->height-3)bicy=image->height-3;


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
						BMat[k][l] = image->imageData[ (bicy+k-1) *image->widthStep + (bicx+l-1)*3   ];
						GMat[k][l] = image->imageData[ (bicy+k-1) *image->widthStep + (bicx+l-1)*3 +1];
						RMat[k][l] = image->imageData[ (bicy+k-1) *image->widthStep + (bicx+l-1)*3 +2];
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
					
				image_bic->imageData[j*image_bic->widthStep + i*3   ]=(int)Bv;
				image_bic->imageData[j*image_bic->widthStep + i*3 +1]=(int)Gv;
				image_bic->imageData[j*image_bic->widthStep + i*3 +2]=(int)Rv;
			}
		}
	}


	cvShowImage("bicubic",image_bic);	
}

int main(void)
{	
	IplImage *image=cvLoadImage("before.tif");	
	transform(image);
	cvShowImage("after",image);
	cvSaveImage("after.tif",image);
	cvWaitKey(0);
	return 0;
}
