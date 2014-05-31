/**********************************************************************/
/*                                                                    */
/*         File csqm.c:  Central Server Queueing Network Model        */
/*                                                                    */
/*  This is the queueing network simulation program                   */
/*  (with a call to "reportf()" added).                               */
/*                                                                    */
/**********************************************************************/

#include "smpl.h"

#define n0 6         /* no. class 0 tasks  */
#define n1 3         /* no. class 1 tasks  */
#define nt n0+n1     /* total no. of tasks */
#define nd 4         /* no. of disk units  */
#define qd 1         /* queued req. return */

struct token
  {
    int cls;         /* task's class (& priority) */
    int un;          /* disk for current IO req.  */
    real ts;         /* tour start time stamp     */
  } task[nt+1];

int
  disk[nd+1],        /* disk facility descriptors */
  cpu,               /* cpu facility descriptor   */
  nts=10000;         /* no. of tours to simulate  */

real
  tc[2]={10.0,5.0},  /* class 0,1 mean cpu times  */
  td=30.0, sd=7.5;   /* disk time mean, std. dev. */

main()
  {
    int i,j,event,n[2]; real t,s[2]; struct token *p;
    n[0]=n[1]=0; s[0]=s[1]=0.0;
    for (i=1; i<=nt; i++) task[i].cls=(i>n0)? 1:0;
    smpl(0,"central server model");
    cpu=facility("CPU",1);
    for (i=1; i<=nd; i++) disk[i]=facility("disk",1);
    for (i=1; i<=nt; i++) schedule(1,0.0,i);
    while (nts)
      {
        cause(&event,&i); p=&task[i];
        switch(event)
          {
            case 1:  /* begin tour */
              p->ts=time(); schedule(2,0.0,i);
              break;
            case 2:  /* request cpu */
              j=p->cls;
              if (preempt(cpu,i,j)!=qd) then
                schedule(3,expntl(tc[j]),i);
              break;
            case 3:  /* release cpu, select disk */
              release(cpu,i); p->un=random(1,nd);
              schedule(4,0.0,i);
              break;
            case 4:  /* request disk */
              if (request(disk[p->un],i,0)!=qd) then
                schedule(5,erlang(td,sd),i);
              break;
            case 5:  /* release disk, end tour */
              release(disk[p->un],i); j=p->cls;
              t=time(); s[j]+=t-p->ts; p->ts=t; n[j]++;
              schedule(1,0.0,i); nts--;
              break;
          }
      }
    reportf(); printf("\n\n");
    printf("class 0 tour time = %.2f\n",s[0]/n[0]);
    printf("class 1 tour time = %.2f\n",s[1]/n[1]);
  }


