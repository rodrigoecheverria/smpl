//========================================================= file = mm1a.c =====
//=  A simple M/M/1 queue simulation using SMPL with bmeans for stopping      =
//=============================================================================
//=  Build:                                                                   =
//=         gcc mm1a.c smpl.c bmeans.c rand.c -lm                             =
//============================================================================= 

//----- Include files ---------------------------------------------------------
#include <stdio.h>      // Needed for printf()
#include "smpl.h"       // Needed for SMPL

//===== Main program ==========================================================
void main(void)
{
  real Ta = 1.0;        // Mean interarrival time (seconds)
  real Ts = 0.9;        // Mean service time
  int customer = 1;     // Customer id (always '1' for this simulation)
  int event;            // Event (1 = arrival, 2 = request, 3 = completion)
  int server;           // Handle for server facility

  int num_deletions;    // Number of deletions for batch means
  int batch_size;       // Batch size (in number of observations)
  int done;             // Simulation done flag (0 = false, 1 = true)

  real mean;            // Mean values returned by civals();
  real half_width;      // Half width returned by civals();
  int  num_batches;     // Number of batches returned by civals();

  // Initialize SMPL subsystem
  smpl(0, "M/M/1 Queue");

  // Initialize server facility (single server)
  server=facility("server", 1);

  // Initialize batch means with deletion count and batch size
  num_deletions = 0;
  batch_size = 1000;
  init_bm(num_deletions, batch_size);

  // Schedule arrival event at time 0 to kick-off simulation
  schedule(1, 0.0, customer);

  // Loop until done is true
  done = 0;
  while (done == 0)
  {
    // "Cause" the next event on the event list
    cause(&event,&customer);

    // Process the event
    switch(event)
    {
      case 1:  // *** Arrival
        schedule(2, 0.0, customer);
        schedule(1, expntl(Ta), customer);
        done = obs((real) (inq(server) + status(server)));   // <--- NOTE
        break;

      case 2:  // *** Request Server
        if (request(server, customer, 0) == 0)
          schedule(3, expntl(Ts), customer);
        break;

      case 3:  // *** Release server
        release(server, customer);
        break;
    }
  }

  // Output standard SMPL report
  report();

  // Output mean number of customers in the system from SMPL queury functions
  // and from civals()
  civals(&mean, &half_width, &num_batches);
  printf("================================================================\n");
  printf("=  Mean number in system = %f \n", (Lq(server) + U(server)));
  printf("=  Mean = %f  Half-width = %f  Number batches = %d \n",
    mean, half_width, num_batches);
  printf("================================================================\n");
}
