/**********************************************************************/
/*                                                                    */
/*                 File mm1q1.c:  M/M/1 Queueing Model                */
/*                                                                    */
/*  This is the "standalone" simulation model of an M/M/1 queue, with */
/*  instrumentation, For completeness, it inck¡ludes 		      */
/*         its own random number generator.  (This 16-bit generator   */
/*  has a limited period and is not recommended for general use.)     */
/*                                                                    */ 
/**********************************************************************/

#include <math.h>
#include <stdio.h>


main()
  {
    double Ta=200.0,Ts=100.0,te=200000.0,t1,t2,time;
    double b,c,l,s,tn,tb,u,w,x,expntl();
    int n;
    n=0; t1=0.0; t2=te; time=0.0;
    b=s=0.0; c=0; tn=time;
    while (time<te)
      { printf("time= %.3f\n",time);
        if (t1<t2)
          { /* event 1:  arrival */
            time=t1; s+=n*(time-tn); n++; tn=time;
            t1=time+expntl(Ta);
            if (n==1) {tb=time; t2=time+expntl(Ts);}
          }
        else
          { /* event 2:  completion */
            time=t2; s+=n*(time-tn); n--; tn=time; c++;
            if (n>0) t2=time+expntl(Ts);
            else {t2=te; b+=time-tb;}
          }
      }
    x=c/time; printf("throughput rate = %.3f\n",x);
    u=b/time; printf("server utilization = %.3f\n",u);
    l=s/time; printf("mean no. in system = %.3f\n",l);
    w=l/x;    printf("mean residence time = %.3f\n",w);
  }

double expntl(x)
  double x;
    {
      double random(),a;
      a = -x*log(random());   
	return(a);
       
    }

double random()
  {
    static long x=1L;
    do x=(32517L*x+6925L)%32768L; while (x==0L);
    return((double)x/32768.0);
  }

