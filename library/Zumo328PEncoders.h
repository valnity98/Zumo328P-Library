/*! \file Zumo328PEncoders.h */

#pragma once

#include <stdint.h>

/*! \brief This class is ported to work on the Zumo 328P (Arduino UNO).*/

class Zumo328PEncoders
{
public:

    /*! This function initializes the encoders if they have not been initialized
     *  already and starts listening for counts.  This
     *  function is called automatically whenever you call any other function in
     *  this class, so you should not normally need to call it in your code. */
    static void init()
    {
        static bool initialized = 0;
        if (!initialized)
        {
            initialized = true;
            init2();
        }
    }

    /*! Returns the number of counts that have been detected from the left-side
     * encoder.  These counts start at 0.  Positive counts correspond to forward
     * movement of the left side of the Zumo, while negative counts correspond
     * to backwards movement.
     *
     * The count is returned as a signed 32-bit integer.  When the count goes
     * over 2,147,483,647, it will overflow down to -2,147,483,648 .  When the count goes below
     * -2,147,483,648 , it will overflow up to 2,147,483,647 . */
    static int32_t getCountsLeft();

    /*! This function is just like getCountsLeft() except it applies to the
     *  right-side encoder. */
    static int32_t getCountsRight();

    /*! This function is just like getCountsLeft() except it also clears the
     *  counts before returning.  If you call this frequently enough, you will
     *  not have to worry about the count overflowing. */
    static int32_t getCountsAndResetLeft();

    /*! This function is just like getCountsAndResetLeft() except it applies to
     *  the right-side encoder. */
    static int32_t getCountsAndResetRight();


private:

    static void init2();
};
