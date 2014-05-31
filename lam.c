/**********************************************************************/
/*                                                                    */
/*             File lam.c:  CSMA/CD Mean Frame Delay TIme             */
/*                                                                    */
/*  This program comprises a simple input processor ("main()") to-    */
/*  gether with the delay computation function of Figure 6.5.  It     */
/*  computes the mean normalized frame delay in a CSMA/CD network,    */
/*  given the normalized throughput S and the propagation time/mean   */
/*  frame transmission time ratio a.                                  */
/*                                                                    */
/**********************************************************************/

#include <stdio.h>
#include <math.h>

typedef double real;
#define then

main()
  {
    int dis; real a,r,S,Smx,tau=1.0,Td,Tf,delay(); char s[4],ss;
    double atof();
    while (1)
      { /* get parameters: distribution type, a, and, iteratively, S; */
        /* compute Lam's bound on throughput, Smax, from equation 6.6 */
        printf("\n");
        printf("enter frame distr. type C(onstant) or E(xponential): ");
        scanf("%c",&ss);
        dis=(ss=='c'| ss=='C')?  1:2;
        printf("enter prop. time/transfer time ratio a: ");
	scanf("%f",&a);
        Smx=1.0/(1.0+a*(1.0+2.0*exp(1.0)));
        printf("   note:  Smax = %.3f\n",Smx);
        printf("%f, %c",a, ss);
        while (1)
          { /* assume unit propagation delay (tau), compute Tf from a */
            /* and compute the arrival rate r from S.  Call 'delay()' */
            /* to the compute mean frame delay, and compute and print */
            /* the normalized mean delay. */
            printf("enter normalized throughput S: ");
            scanf("%f",S); 
            Tf=tau/a;  r=S/Tf; Td=delay(tau,r,Tf,dis);
            printf("   normalized mean delay = %.3f\n",Td/Tf);
          }
      }
  }

real delay(t,r,Tf,dis)
  real t,r,Tf; int dis;
    {
      real Tf2,B,f1,f2,f3,f4,f5,e=exp(1.0);
      if (dis==1)
        then {Tf2=Tf*Tf;     B=exp(-r*Tf);}
        else {Tf2=2.0*Tf*Tf; B=1.0/(1.0+r*Tf);}
      f1=Tf+t*(4.0*e+1.0)/2.0;
      f2=Tf2+t*Tf*(4.0*e+2.0)+t*t*(5.0+4.0*e*(2.0*e-1.0));
      f2*=r;
      f3=2.0*(1.0-r*(Tf+t*(2.0*e+1.0)));
      f4=(1.0-exp(-2.0*r*t))*(e+r*t-3.0*r*t*e);
      f5=r*e*(B*exp(-(1.0+r*t))+exp(-2.0*r*t)-1.0);
      return(f1+f2/f3-f4/f5);
    }

