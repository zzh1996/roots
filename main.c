/*程序功能：求解所有指定次数的、系数为-1、1的多项式，并把复数解保存到r[0-7].data中
输出格式：二进制文件，顺序排列每个解(double)的实部和虚部*/

#include <stdio.h>
#include <gsl/gsl_poly.h> //GNU science library 多项式
#include <sys/wait.h>
#include <unistd.h>

#define deg 24 //多项式次数

int main(){
  int i,j,p,total;
  double a[deg+1],z[deg*2]; //a:多项式系数 z:求解结果
  FILE *fp;
  char fn[10];
  a[deg]=1.0; //减少一半运算
  
  for(p=0;p<8;p++){
    if(fork()==0){
      sprintf(fn,"r%d.data",p);
      fp=fopen(fn,"wb");
      for(i=0;i<3;i++) //该进程的三个确定系数
        a[deg-1-i]=(p&(1<<i))?1.0:-1.0;
      total=(1<<(deg-3));
      for(i=0;i<total;i++){
        for(j=0;j<deg-3;j++)
          a[j]=(i&(1<<j))?1.0:-1.0;
        gsl_poly_complex_workspace *w=gsl_poly_complex_workspace_alloc(deg+1);
        gsl_poly_complex_solve(a,deg+1,w,z); //求解
        gsl_poly_complex_workspace_free(w);
        fwrite(z,sizeof(double),deg*2,fp);

        if(i%10000==0)printf("Process%d : %.2f%%\n",p,i*100.0/total);
      }
      printf("Process%d : Completed\n",p);
      exit(0);
    }
  }
  while(wait(NULL)>0); //等待其他进程
  return 0;
}
