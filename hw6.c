#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
using namespace std;

//FUNCTION HEADERS HERE
//   FUNCTIONS FOR READING AND WRITING
double **readpicture(ifstream &infile, int &width, int &height);
char readchar(ifstream& infile, int &num);
int readint(ifstream& infile, int numbytes, int &num);
int char2int(char thechar);
int char2int(char *thechar, int numbytes);
void readrgb(ifstream& infile, double *rgb, int &num);
void writepicture(ofstream& outfile, double **f, int width, int height);
char int2char(unsigned long theint);
void int2char(char *thechar, int theint);
void writechar(ofstream &outfile, char thechar);
void writeint(ofstream &outfile, int theint, int numbytes);
void writergb(ofstream& outfile, double r, double g, double b);
//   FUNCTIONS FOR CREATING AND REMOVING MEMORY
double **matrix(int width, int height);
void free_matrix(double **f, int width, int height);
//   YOUR FUNCTIONS HERE
void steadyheat( int inten, double **f,int width, int height);
double getmax(double **f, char x, int width, int height);
double CH(double ** f, double intensity,int height, int width);
double **setimg(int width, int height, int xlow, int xup, int ylow, int yup);
//MAIN BODY HERE

 int main()
{
   int width = 500;
   int height = 500;
   double **f,**newf,**f0;
   double x, y;
   int i, j;

// if reading in picture
ifstream infile("trainsnoisy.bmp", ios::in | ios::binary);
 f = readpicture(infile,width,height);
   infile.close();

   ifstream infile1("trainsnoisy.bmp", ios::in | ios::binary);
 f0 = readpicture(infile1,width,height);
   infile.close();
//f = setimg(width,height,-1,1,-1,1
   //f = setimg(500,500,-1,1,-1,1); 
   newf = matrix(width,height);
   /*double **D = matrix(width,height);
   for (int i = 0; i<width; i++){
            for (int j = 0; j < height; j++){
                  if (i<=50&&j<=50){
                        D[i][j] = 1.0;
                        f[i][j] = 0.5;

                  }
                  else{
                        D[i][j] = 0;
                  }
            }
      }*/
   ofstream outfile1("initial.bmp", ios::out | ios::binary);
   writepicture(outfile1,f,width,height);
   outfile1.close();
 
   char notdone = 1;
  double fx,fy,fxx,fyy,fxy;
  double themax ;
  int step = 1; 
  double h = .1;
  double lamb = 25;
  double eps = .1;
  printf("%lf",eps);
  double  s = pow(h,3)/(4+lamb*h*h*h);
 // printf("%lf\n",s);
   while (step<=1000)
   {
      for (int i = 0; i<width;i++){
            for (int j = 0; j<height;j++){
            newf[i][j] = f[i][j];
                //  if(D[i][j] ==1){
            int li,ui,lj,uj;
                  if(i-1<0)
                    li = i;
                  else 
                    li = i-1;
                  
                  if(i+1>=width)
                    ui = width-1;
                  else
                    ui = i+1;
                  
                  if(j-1<0)
                    lj = j;
                  else 
                    lj = j-1;
                  
                  if(j+1>=height)
                    uj = height-1;
                  else
                    uj = j+1;
                    
                        fx = (f[ui][j]-f[li][j])/(2*h);
                        fy = (f[i][uj]-f[i][lj])/(2*h);
                        fxx =(f[ui][j]-2*f[i][j]+f[li][j])/(h*h);
                        fyy =(f[i][uj]-2*f[i][j]+f[i][lj])/(h*h);
                        fxy = (f[ui][uj]-f[ui][lj]-f[li][uj]+f[li][lj])/(4*h*h); 
                         newf[i][j] = f[i][j]+s*((fxx+fyy)/sqrt(fx*fx+fy*fy+eps*eps)-(fx*(fxx*fx+fxy*fy) +fy*(fxy*fx+fyy*fy))/sqrt(pow(fx*fx+fy*fy+eps*eps,3)) - 2*lamb*(f[i][j] - f0[i][j])); 
                        
                        //printf("%lf\n", newf[i][j]);
                 // }
            }
      }
     
      //printf("%lf\n", newf[17][17]);
       themax = 0;
       for(int i = 0; i<width; i++){
            for (int j = 0; j <height;j++){ 
                  
                 // if(D[i][j] == 1)
                   if(fabs(newf[i][j]-f[i][j])>themax)
                    themax = fabs(newf[i][j]-f[i][j]);
                  f[i][j] = newf[i][j];
            }
       }
     //  printf("%lf\n", themax);
        cout << step << " themax = " << themax << endl;
       if (themax < 1.0e-6)
         notdone = 0;
      step++;     

   }

  
// if creating picture
//   width = 100;
//   height = 100;
//   f = matrix(width,height);

// writing picture
printf("1717: %lf\n",f[17][17]);

   ofstream outfile("result.bmp", ios::out | ios::binary);
   writepicture(outfile,f,width,height);
   outfile.close();

   free_matrix(f,width,height);
}

//FUNCTION CONTENT HERE

//**************************************************************************
//FUNCTIONS FOR READING AND WRITING
//**************************************************************************
double **readpicture(ifstream &infile, int &width, int &height)
{
   int i, j, k;
   char junk, theformat[2];
   double rgb[3], **f;
   int num = 0;
   int upsidedown = 0;
   cout << "reading picture" << endl;

   for (i = 0; i < 2; i++)
      theformat[i] = readchar(infile,num);
   int filesize = readint(infile,4,num);
   cout << "   numbytes = " << filesize << endl;
   for (i = 0; i < 4; i++)
      junk = readchar(infile,num);
   int offset = readint(infile,4,num);
   int headerstart = num;
   int headersize = readint(infile,4,num);
   if (headersize == 12)
   {
      width = readint(infile,2,num);
      height = readint(infile,2,num);
   }
   else
   {
      width = readint(infile,4,num);
      height = readint(infile,4,num);
   }
   if (height < 0)
   {
      height = abs(height);
      upsidedown = 1;
   }
   cout << "   width = " << width << endl;
   cout << "   height = " << height << endl;
   int numcolorpanes = readint(infile,2,num);
   int bitsperpixel = readint(infile,2,num);
   if (bitsperpixel != 24)
   {
      cout << "ERROR: this program is only built for 1 byte per rgb, not a total of " 
           << bitsperpixel << " bits" << endl;
      exit(1);
   }
   for (i = num-headerstart; i < headersize; i++)
      junk = readchar(infile,num);

   f = matrix(width,height);

   if (upsidedown)
      for (j = height-1; j >= 0; j--)
      {
         for (i = 0; i < width; i++)
         {
            readrgb(infile,rgb,num);
            f[i][j] = 0.0;
            for (k = 0; k < 3; k++)
               f[i][j] += rgb[k]*rgb[k];
            f[i][j] = sqrt(f[i][j])/sqrt(3.0);
         }
         for (i = 0; i < (4-(3*width)%4)%4; i++)
            junk = readchar(infile,num);
      }
   else
      for (j = 0; j < height; j++)
      {
         for (i = 0; i < width; i++)
         {
            readrgb(infile,rgb,num);
            f[i][j] = 0.0;
            for (k = 0; k < 3; k++)
               f[i][j] += rgb[k]*rgb[k];
            f[i][j] = sqrt(f[i][j])/sqrt(3.0);
         }
         for (i = 0; i < (4-(3*width)%4)%4; i++)
            junk = readchar(infile,num);
      }

   return f;
}

int readint(ifstream& infile, int numbytes, int &num)
{
   char *temp;
   int i;
   int theint;

   temp = new char[numbytes];

   for (i = 0; i < numbytes; i++)
      infile.read(reinterpret_cast<char *>(&(temp[i])),sizeof(char));
   theint = char2int(temp,numbytes);

   delete[] temp;
   num += numbytes;

   return theint;
}

int char2int(char thechar)
{
   int i, theint, imask;
   char cmask;

   cmask = 1;
   imask = 1;
   theint = 0;
   for (i = 0; i < 8; i++)
   {
      if (thechar & cmask)
      {
         theint += imask;
      }
      cmask = cmask << 1;
      imask *= 2;
   }

   return theint;
}

int char2int(char *thechar, int numbytes)
{
   int i;
   int theint, power;

   power = 1;
   theint = 0;
   for (i = 0; i < numbytes; i++)
   {
      theint += char2int(thechar[i])*power;
      power = power*256;
   }

   return theint;
}

char readchar(ifstream& infile, int &num)
{
   char thechar;

   infile.read(reinterpret_cast<char *>(&thechar),sizeof(char));
   num++;

   return thechar;
}

void readrgb(ifstream& infile, double *rgb, int &num)
{
   char ctemp;
   int i, itemp, ijunk;

   for (i = 0; i < 3; i++)
   {
//      ctemp = readchar(infile);
//      itemp = char2int(ctemp);
      itemp = readint(infile,1,ijunk);
      rgb[i] = static_cast<double>(itemp)/255.0;
      num++;
   }
}

void writepicture(ofstream& outfile, double **f, int width, int height)
{
   int numbytes;
   int i, j;
   cout << "writing picture" << endl;

   writechar(outfile,66);
   writechar(outfile,77);

// number of bytes
   numbytes = 54+height*(3*width+(4-(3*width)%4)%4);
   cout << "   numbytes = " << numbytes << endl;
   writeint(outfile,numbytes,4);

   writeint(outfile,0,2);
   writeint(outfile,0,2);
   writeint(outfile,54,4);
   writeint(outfile,40,4);

// width
   cout << "   width = " << width << endl;
   writeint(outfile,width,4);
// height
   cout << "   height = " << height << endl;
   writeint(outfile,height,4);

   writeint(outfile,1,2);
   writeint(outfile,24,2);
   writeint(outfile,0,4);
   writeint(outfile,16,4);
   writeint(outfile,2835,4);
   writeint(outfile,2835,4);
   writeint(outfile,0,4);
   writeint(outfile,0,4);

   for (j = 0; j < height; j++)
   {
      for (i = 0; i < width; i++)
         writergb(outfile,f[i][j],f[i][j],f[i][j]);
      for (i = 0; i < (4-(3*width)%4)%4; i++)
         writechar(outfile,0);
   }
}

char int2char(unsigned long theint)
{
   char thechar, mask;
   int i;

   mask = 1;
   thechar = 0;
   for (i = 0; i < 8; i++)
   {
      if (theint%2)
         thechar += mask;
      theint /= 2;
      mask = mask << 1;
   }

   return thechar;
}

void int2char(char *thechar, int theint, int numbytes)
{
   int temp = theint;
   int i;

   for (i = 0; i < numbytes; i++)
   {
      thechar[i] = int2char(temp%256);
      temp = temp/256;
   }
}

void writechar(ofstream& outfile, char thechar)
{
   outfile.write(reinterpret_cast<char *>(&thechar),sizeof(char));
}

void writeint(ofstream &outfile, int theint, int numbytes)
{
   char *temp = new char[numbytes];
   int i;

   int2char(temp,theint,numbytes);
   for (i = 0; i < numbytes; i++)
      outfile.write(reinterpret_cast<char *>(&(temp[i])),sizeof(char));

   delete[] temp;
}

void writergb(ofstream& outfile, double r, double g, double b)
{
   int i, irgb;
   char temp;
   char a;
   double rgb[3];

   rgb[0] = r;
   rgb[1] = g;
   rgb[2] = b;
   for (i = 0; i < 3; i++)
   {
      irgb = static_cast<int>(floor(255.0*rgb[i]+0.5));
      if (irgb < 0)
         irgb = 0;
      else if (irgb > 255)
         irgb = 255;

      temp = int2char(irgb);
      outfile.write(reinterpret_cast<char *>(&temp),sizeof(char));
   }
}

//**************************************************************************
//FUNCTIONS FOR CREATING AND REMOVING MEMORY
//**************************************************************************

double **matrix(int width, int height)
{
   double **f;
   int i;

   f = new double*[width];
   for (i = 0; i < width; i++)
      f[i] = new double[height];

   return f;
}

void free_matrix(double **f, int width, int height)
{
   int i;

   for (i = 0; i < width; i++)
      delete f[i];
   delete f;
}

//**************************************************************************
//YOUR FUNCTIONS HERE
//**************************************************************************
void steadyheat( int inten, double **f,int width, int height){
      double **D = matrix(width,height);
      for (int i = 0; i<width; i++){
            for (int j = 0; j < height; j++){
                  if (width/3<=i&&i<=width*4/6&&height/3<=j&&j<=height*4/6){
                        D[i][j] = 1.0;
                        f[i][j] = 0.5;

                  }
                  else{
                        D[i][j] = 0;
                  }
            }
      }
      printf("%lf\n",D[250][250]);

      int *row, *col;
      double *val;
      int N = 0, **k;
      
      row = new int[5*width*height];
      col = new int[5*width*height];
      val = new double[5*width*height];
      int ** theindex;
      double *b = new double[5*width*height];
      k = new int*[width];
      for (int i = 0; i<width; i++){
            k[i] = new int[height];
      }
      int *kinvi, *kinvj;
      kinvi = new int[width*height];
      kinvj = new int[width*height];

      int M = 0;
      for (int i = 0; i<width;i++){
            for (int j = 0; j<height;j++){
                  if(D[i][j]==1.0){
                        k[i][j] = M;
                        kinvi[M]=i;
                        kinvj[M]=j;
                        M++;
                  }
            }
      }
      printf("k[250][250] = %d\nM: %d\n",k[250][250],M);
      N=0;

      for(int i = 0; i<width;i++){
            for (int j = 0; j<width;j++){
                  if(D[i][j]==1.0){
                        b[k[i][j]]=0.0;

                        row[N]=k[i][j];
                        col[N]=k[i][j];
                        val[N]=-4.0;

                        if(i-1<0||i+1>=width){
                              val[N] +=1.0;
                        }
                        if(j-1<0||j+1>=height){
                              val[N]+=1.0;
                              
                        }
                        N++;
                        if(j-1>=0){
                              if(D[i][j-1]==1.0){
                                    row[N]=k[i][j];
                                    col[N]=k[i][j-1];
                                    val[N]=1.0;
                                    N++;
                              }
                        }
                        else{
                              b[k[i][j]]-=f[i][j-1];
                              //printf("b:%lf\n",b[k[i][j]]);
                        }

                        if(j+1<height){
                              if(D[i][j+1]==1.0){
                                    row[N]=k[i][j];
                                    col[N]=k[i][j+1];
                                    val[N]=1.0;
                                    N++;
                              }
                        }
                        else{
                              b[k[i][j]]-=f[i][j+1];
                              //printf("b:%lf\n",b[k[i][j]]);
                        }

                        if(i-1>=0){
                              if(D[i-1][j]==1.0){
                                    row[N]=k[i][j];
                                    col[N]=k[i-1][j];
                                    val[N]=1.0;
                                    N++;
                              }
                        }
                        else{
                              b[k[i][j]]-=f[i-1][j];
                             // printf("b:%lf\n",b[k[i][j]]);
                        }

                        if(i+1<width){
                              if(D[i+1][j]==1.0){
                                    row[N]=k[i][j];
                                    col[N]=k[i+1][j];
                                    val[N]=1.0;
                                    N++;
                              }
                        }
                        else{
                              b[k[i][j]]-=f[i+1][j];
                              //printf("b:%lf\n",b[k[i][j]]);
                        }
                  }

            }
      }
      printf("N : %d\n",N);

      char notdone=1;
      int r;
      double *xx,*newx,*diag;
      xx = new double[width*height];
      newx = new double[width*height];
      diag - new double[width*height];
      double themax = 0;
     // cout << "process"<<endl;
      for(r=0; r<M;r++){
            xx[r] = 0.5;
      }
      int step = 1;
      while(notdone){
            for( r = 0; r<M;r++){
                  newx[r] = b[r]; 
                  //printf("newx: %lf\n",newx[r]);
            }
           
            for( r = 0; r<N;r++){
                  if(row[r]!=col[r]){
                        newx[row[r]]-=val[r]*xx[col[r]];
                  }
                  else{
                        diag[row[r]] = val[r];
                        //printf("diag: %lf\n",val[r]);
                  }
            }

           
            for( r = 0; r<M;r++){
                  newx[r] /=diag[r];
                  if(fabs(newx[r]-xx[r])>themax)
                        themax = fabs(newx[r]-xx[r]);
                  xx[r] = newx[r];
            }
            if(themax< 1.0e-4)
                  notdone = 0;
            step++;
      }

      r = 0;
      for( int i = 0; i<width;i++)
            for(int j = 0; j<height;j++)
                  if(D[i][j] == 1.0){
                        f[i][j] = xx[r];
                        r++;
                  }

      printf("iteration: %d\nerror: %lf\n",step,themax);
     
}

double CH(double ** f, double intensity,int height, int width){
      int count = 0;
      for (int i = 0; i<width;i++){
            for (int j = 0; j<height;j++){

                  if(f[i][j]<=intensity)
                  count++;
            }
      }
   return count;

}
double getmax(double ** f, char x, int width, int height){
      if (x == 'm'){
         double min = f[0][0];
         for (int i = 0; i<width; i++){
             for (int j = 0; j<height;j++){
                   if (f[i][j]<min){
                         min = f[i][j];
                   }
            }
         }
         return min;
      }
      else if (x == 'M'){
        double max = f[0][0];
         for (int i = 0; i<width; i++){
             for (int j = 0; j<height;j++){
                   if (f[i][j]>max){
                         max = f[i][j];
                   }
            }
         }
         return max;

      }


}
double **setimg(int width, int height, int xlow, int xup, int ylow, int yup){
      double **f = matrix(width,height);
      for (int i = 0; i < width;i++){
            for (int j = 0; j < height; j++){
                  if (i>width/2){
                        f[i][j]=0.75;
                  }
                  else// if (width/3<=i&&i<=width*2/3){
                        f[i][j] =0.25;
                 //}
                 // else
                 //       f[i][j] = .5;
            }
      }
      return f;
}