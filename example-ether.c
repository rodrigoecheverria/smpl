/**********************************************************************/
/*                                                                    */
/*              File ether.c:  Ethernet Simulation Model              */
/*                                                                    */
/*  This is the Ethernet simulation model.  Note that                 */
/*  this model uses the "bmeans" module of Figure 4.7 to determine    */
/*  its run length;  the "bmeans" object file (as well as the "smpl"  */
/*  subsystem) should be linked with this program's object file to    */
/*  produce an executable model.                                      */
/*                                                                    */
/**********************************************************************/

#include "smpl.h"

#define busy  1
#define idle  0
#define Na    200        /* max. no. of active stations */
#define Tp    0.0250     /* propagation delay (ms.)-> Tr*/
#define Tif   0.0096     /* interframe delay (ms.)      */
#define Tslot 0.0512     /* slot time (ms.)             */
#define Tjam  0.0032     /* jam time (ms.)              */

struct request           /* trans. request descriptor   */
  {
    int attempt;         /* no. retransmission attempts */
    int bkf;             /* current backoff count       */
    int lnk;             /* avail/defer wait list link  */
    real tin;            /* request initiation time     */
    real txf;            /* request's transmission time */
  } desc[Na+1];

int
  N=200,                 /* no. of stations in network  */
  chnl=idle,             /* channel status (busy/idle)  */
  avl=1,                 /* avail. descriptor list head */
  dfr=0,                 /* defer wait list head        */
  end=0;                 /* run termination flag        */

real
  a=0.05,            /* propagation/transfer time ratio */
  G=0.5,            /* offerred load:  G=N*Tf/(Ti+Tf)  */
  Tf,                /* mean frame transmission time    */
  Ti,                /* mean inter-request time/station */
  tbs=0.0,           /* channel busy start time         */
  tis=0.0,           /* channel idle start time         */
  tfsum=0.0;         /* frame transmission time sum     */

main()
  {
    int event,stn,nb; real hw,Td;
    Tf=Tp/a; Ti=Tf*((real)N/G-1.0);
    printf("Tf: %f\n", Tf);
    printf("Ti: %f\n", Ti);
    smpl(0,"Ethernet Local Area Network");
    for (stn=1; stn<=Na; stn++) desc[stn].lnk=stn+1;
    desc[Na].lnk=0;
    init_bm(2000,2000); schedule(1,Tp,0);
    while (!end)
      { /* run until 10% Td accuracy achieved */
        cause(&event,&stn);
        switch (event)
          {
            case 1:  TransmitFrame();    break;
            case 2:  Defer(stn);         break;
            case 3:  StartTransmit(stn); break;
            case 4:  EndTransmit(stn);   break;
            case 5:  InitBackoff(stn);   break;
            case 6:  Deassert();         break;
          }
      }
    civals(&Td,&hw,&nb);
    printf("S = %.3f\n",tfsum/time());
    printf("D = %.3f +/- %.3f\n",Td/Tf,hw/Tf);
    printf("Td = %.3f, Tf = %.3f\n", Td, Tf);
  }

real dly()  {return(Tp*(1.0-sqrt(ranf())));}

real maxm(x,y)  real x,y;  {return(x>y? x:y);}

/*-------  EVENT 1:  INITIATE FRAME TRANSMISSION  ------*/
TransmitFrame()
  {
    int stn; struct request *p;
    if (avl) then
      { /* allocate & build request descriptor */
        stn=avl; p=&desc[stn]; avl=p->lnk;
        p->attempt=p->bkf=0; p->tin=time(); p->txf=Tf;
        schedule(2,0.0,stn); N--;
      }
      
    if (N) then {
     //printf ("Time to schedule: %f,N: %d\n",expntl(Ti/N),N);
     //printf("[TRANS]");
     schedule(1,expntl(Ti/N),0);
     }
  }

/*----------  EVENT 2:  DEFER IF CHANNEL BUSY  ---------*/
Defer(stn)
  int stn;
    {
      real dt=dly(); struct request *p=&desc[stn];
      if (chnl==busy && time()>(tbs+dt+Tif))
        then {p->lnk=dfr; dfr=stn;}
        else { //printf("[def]"); 
        schedule(3,maxm(tis+dt+Tif-time(),0.0),stn);}
    }

/*-------------  EVENT 3:  START TRANSMIT  -------------*/
StartTransmit(stn)
  int stn;
    {
      real tc,t=time(),dt; static int ncls;
      struct request *p=&desc[stn];
      if (chnl==idle)
        then
          { /* reserve channel & schedule EndTransmit */
            chnl=busy; tbs=t; ncls=0;
            //printf("[StartTRANS]");
            schedule(4,p->txf,stn);
          }
        else
          { /* collision will occur in tc ms. */
            dt=dly(); tc=maxm(tbs+dt-t,0.0);
            if (++ncls==1) then
              { /* cancel EndTransmit event */
                //printf("[StartTRANSe1]");
                schedule(5,Tjam+dt,cancel(4));
                //printf("[StartTRANSe2]");
                schedule(6,maxm(Tjam+dt,tbs+Tp-t),0);
              }
            //printf("[StartTRANS3]");
            schedule(5,Tjam+tc,stn);
          }
    }

/*--------------  EVENT 4:  END TRANSMIT  --------------*/
EndTransmit(stn)
  int stn;
    { /* end successful frame transmission */
      struct request *p=&desc[stn];
      tfsum+=p->txf; EndRequest(stn); schedule(6,0.0,0);
    }

/*------------  EVENT 5:  INITIATE BACKOFF  ------------*/
InitBackoff(stn)
  int stn;
    {
      int k; struct request *p=&desc[stn];
      if (++p->attempt>16)
        then EndRequest(stn);  /* abandon request */
        else
          { /* compute and schedule backoff delay */
            if (p->attempt==1)
              then p->bkf=2;
              else if (p->bkf<1024) then p->bkf*=2;
            if ((k=randomsmpl(0,p->bkf-1))==0)
              then {p->lnk=dfr; dfr=stn;}
              else { //printf("[initbak]");
              schedule(2,Tslot*k,stn);}
          }
    }

/*-------------  EVENT 6:  RELEASE CHANNEL  ------------*/
Deassert()
  {
    chnl=idle; tis=time();
    while (dfr)
      { /* activate requests on defer wait list */
        //printf("[deass]");
        schedule(3,dly()+Tif,dfr); dfr=desc[dfr].lnk;
      }
  }

/*--------------------  END REQUEST  -------------------*/
EndRequest(stn)
  int stn;
    { /* deallocate descriptor, reschedule next arrival */
      //real delay;
      struct request *p=&desc[stn];
      //delay = time() - p->tin;
      //if (delay > 1) printf("Delay: %f, time(): %f, init : %f\n", time() - p->tin, time(), p->tin);
      
      end=obs(time() - (p->tin));
      p->lnk=avl; avl=stn;
      N++; cancel(1);
      //printf("[end]");
      schedule(1,expntl(Ti/N),0);
    }
