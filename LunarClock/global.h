
#ifndef _GLOBAL_H
#define _GLOBAL_H


#define DEBUG(x) myprintf(F(x));

#ifndef M_PI
#define M_PI 3.141592653589793238462643
#endif

// maximum time to try and set the phase
// If we reach this without success then halt the system.
#define PHASE_SET_TIME_LIMIT  60000


// Both these should be true for normal operation
#define TEST_WITHOUT_PHASE  false
#define DISABLE_TILT_ON_TOO_MANY_RETRY  false
 


#endif
