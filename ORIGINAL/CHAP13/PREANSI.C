    /*
     Standard include files
    */
    #include <stdio.h>

    /*
     Define some useful constants
    */
    #define MOVE_UP    0x01
    #define MOVE_DOWN  0x02
    #define MOVE_LEFT  0x03
    #define MOVE_RIGHT 0x04

    /*
     Simple function with a switch statement, using K&R style.  Simulates
     an event processing loop in a game.
    */
    EventLoop(Message)
    int Message;
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
        printf("Moving left\n");
        break;
      case MOVE_RIGHT:
        printf("Moving right\n"); 
      default:
        printf("Unknown message: %d\n",Message); 
        break;
      }
      return 0;
    }

    /*
      Main program entry point
    */
    main()
    {
      EventLoop(MOVE_RIGHT);
      return 0;
    }
