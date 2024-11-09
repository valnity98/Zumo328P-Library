/*! \file Zumo328PPID.h */

#pragma once

#include <stdint.h>

/*! \brief Controls motor speed through PID Controller for a better line following */

class Zumo328PPID
{
  public:

     static inline void init()
    {
        static bool initialized = false;

        if (!initialized)
        {
            initialized = true;
            init2();
        }
    }

   

  private:
    static void init2();
};
