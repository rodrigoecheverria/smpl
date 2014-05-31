/**********************************************************************/
/*                                                                    */
/*                 File mm1q2.c:  M/M/1 Queueing Model                */
/*                                                                    */
/*  This is the "smpl" M/M/1 queue simulation   		      */
/*                                                                    */
/**********************************************************************/ 

#include "smpl.h"

main()
  {
    real Ta=200.0,Ts=100.0,te=200000.0;
    int customer=1,event,server;
    smpl(0,"M/M/1 Queue");
    server=facility("server",1);
    schedule(1,0.0,customer);
    while (time()<te)
      {
        cause(&event,&customer);
        switch(event)
          {
            case 1:  /* arrival */
              schedule(2,0.0,customer);
              schedule(1,expntl(Ta),customer);
              break;
            case 2:  /* request server */
              if (request(server,customer,0)==0) then
                schedule(3,expntl(Ts),customer);
              break;
            case 3:  /* release server */
              release(server,customer);
              break;
          }
      }
    report();
  }
