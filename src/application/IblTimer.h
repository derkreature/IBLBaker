//------------------------------------------------------------------------------------//
//                                                                                    //
//    ._____________.____   __________         __                                     //
//    |   \______   \    |  \______   \_____  |  | __ ___________                     //
//    |   ||    |  _/    |   |    |  _/\__  \ |  |/ // __ \_  __ \                    //
//    |   ||    |   \    |___|    |   \ / __ \|    <\  ___/|  | \/                    //
//    |___||______  /_______ \______  /(____  /__|_ \\___  >__|                       //
//                \/        \/      \/      \/     \/    \/                           //
//                                                                                    //
//    IBLBaker is provided under the MIT License(MIT)                                 //
//    IBLBaker uses portions of other open source software.                           //
//    Please review the LICENSE file for further details.                             //
//                                                                                    //
//    Copyright(c) 2014 Matt Davidson                                                 //
//                                                                                    //
//    Permission is hereby granted, free of charge, to any person obtaining a copy    //
//    of this software and associated documentation files(the "Software"), to deal    //
//    in the Software without restriction, including without limitation the rights    //
//    to use, copy, modify, merge, publish, distribute, sublicense, and / or sell     //
//    copies of the Software, and to permit persons to whom the Software is           //
//    furnished to do so, subject to the following conditions :                       //
//                                                                                    //
//    1. Redistributions of source code must retain the above copyright notice,       //
//    this list of conditions and the following disclaimer.                           //
//    2. Redistributions in binary form must reproduce the above copyright notice,    //
//    this list of conditions and the following disclaimer in the                     //
//    documentation and / or other materials provided with the distribution.          //
//    3. Neither the name of the copyright holder nor the names of its                //
//    contributors may be used to endorse or promote products derived                 //
//    from this software without specific prior written permission.                   //
//                                                                                    //
//    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      //
//    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        //
//    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE      //
//    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          //
//    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   //
//    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN       //
//    THE SOFTWARE.                                                                   //
//                                                                                    //
//------------------------------------------------------------------------------------//

#ifndef INCLUDED_TIMER
#define INCLUDED_TIMER

#include <IblPlatform.h>

namespace Ibl
{
class Timer
{
  public:
    Timer();
    ~Timer();

  protected:
    //----------------------
    // initializes the timer 
    //----------------------
    bool                       initialize();

    void                       lockFrameCounter();
    void                       preciseSleep(double seconds);

  public:
    static size_t              frame (double time, double rate = 24.0);
    static double              time (size_t frame, double rate = 24.0);

    //------------------
    // Updates the timer
    //------------------
    bool                        update (bool stop = false);

    void                        reset();
    void                        startTimer();
    void                        stopTimer();
    const double&               absoluteTime() const;
    const double&               elapsedTime() const;
    const double&               demoTime() const;

    const double&               normalizedTime() const;
    uint32_t                    lockFrameCounter() const;
    void                        setLockFrameCounter(uint32_t rate);

    uint32_t                    frameRate() const;

  protected:
    double                      elapsedSeconds();

    uint64_t                   _startTick;
    long                       _zeroClock;
    unsigned long              _timerMask;

    bool                       _timerInitialized;

    LONGLONG                   _llQPFTicksPerSec;
    LONGLONG                   _llStopTime; 
    LONGLONG                   _llLastElapsedTime;
    LONGLONG                   _llBaseTime;

    double                     _lastElapsedTime;
    double                     _elapsedTime;
    double                     _absTime;
    double                     _appTime;
    double                     _normalizedTime;

    double                     _time;
    LARGE_INTEGER              _qwTime;
    LARGE_INTEGER              _startTime;
    bool                       _timerStarted;

    HANDLE                     _timeEvent;
    double                     _lastSeconds;

    uint32_t                   _lockFrameCounter;

    uint32_t                   _frameRate;
    uint32_t                   _frameRateCounter;
    float                      _frameRateTimeElapsed;

};
}
#endif