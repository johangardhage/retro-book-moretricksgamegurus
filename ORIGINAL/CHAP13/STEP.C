    //
    // Standard include files
    //
    #include <stdio.h>
    #include <stdlib.h>

    //
    // Define some useful constants
    //
    #define MOVE_UP    0x01
    #define MOVE_DOWN  0x02
    #define MOVE_LEFT  0x03
    #define MOVE_RIGHT 0x04

    //
    // Simple function with a switch statement.  Simulates an
    // event processing loop in a game.
    //
    void EventLoop(int Message)
    {
      switch(Message)
      {
      case MOVE_UP:
        printf("Moving up\n");
        break;
      case MOVE_DOWN:
        printf("Moving down\n");
        break;
      case MOVE_LEFT:
        printf("Moving right\n");
        break;
      case MOVE_RIGHT:
        printf("Moving right\n");
      default:
        printf("Unknown message: %d\n",Message);
      }
    }

    //
    //  Main program entry point
    //
    void main(void)
    {
      EventLoop(MOVE_RIGHT);
    }
