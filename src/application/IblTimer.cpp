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

#include <IblTimer.h>
#include <IblLog.h>

namespace Ibl
{
Timer::Timer() :
    _timerInitialized(false),
    _llQPFTicksPerSec(0),
    _llStopTime(0),
    _llLastElapsedTime(0),
    _llBaseTime(0),
    _lastElapsedTime(0),
    _elapsedTime(0),
    _appTime (0),
    _absTime (0),
    _normalizedTime (0),
    _timerStarted(0),
    _lockFrameCounter (0),
    _lastSeconds(0),
    _frameRate(0),
    _frameRateCounter(0),
    _frameRateTimeElapsed(0.0),
    _timerMask(0)
{
    _timeEvent = CreateEvent(nullptr,TRUE,FALSE,nullptr);
    ResetEvent(_timeEvent);    
    initialize();
}

Timer::~Timer()
{
    CloseHandle(_timeEvent);
}

const double&
Timer::normalizedTime() const
{
    return _normalizedTime;
}

void 
Timer::preciseSleep(double seconds)
{ 
    const unsigned int milliseconds= (unsigned int)(seconds*1000.0);
    HANDLE thread = GetCurrentThread();
    if (milliseconds == 0)
        return;

    // Set affinity to the first core
    intptr_t oldMask = SetThreadAffinityMask(thread, _timerMask);
	WaitForSingleObject(_timeEvent, milliseconds);
    SetThreadAffinityMask(thread, oldMask);
}

size_t
Timer::frame (double time, double rate)
{
    return size_t(time * rate);
}

double
Timer::time (size_t frame, double rate)
{
    return double(frame) * (1.0f / rate);
}

bool 
Timer::initialize()
{
    if(_timerInitialized  == FALSE)
    {
        _timerInitialized = TRUE;
        LARGE_INTEGER qwTicksPerSec;

        DWORD_PTR procMask;
        DWORD_PTR sysMask;
        GetProcessAffinityMask(GetCurrentProcess(), &procMask, &sysMask);

        if (procMask == 0)
            procMask = 1;

        // Find the lowest core that this process uses
        if( _timerMask == 0 )
        {
            _timerMask = 1;
            while( ( _timerMask & procMask ) == 0 )
            {
                _timerMask <<= 1;
            }
        }

        HANDLE thread = GetCurrentThread();

        // Set affinity to the first core
        DWORD_PTR oldMask = SetThreadAffinityMask(thread, _timerMask);

        // Assert if we cannot use query performance counter.
        qwTicksPerSec.QuadPart = 0;
        if (QueryPerformanceFrequency( &qwTicksPerSec ) == 0)
        {
            LOG ("Failed to get performance frequency.");
        }


        _llQPFTicksPerSec = qwTicksPerSec.QuadPart;

        QueryPerformanceCounter(&_startTime);
        _startTick = GetTickCount();
        
        // Reset affinity
        SetThreadAffinityMask(thread, oldMask);
        _zeroClock = clock();
               
        return true;
    }
    return false;
}

uint32_t
Timer::lockFrameCounter() const
{
    return _lockFrameCounter;
}

void
Timer::setLockFrameCounter(uint32_t rate)
{
    _lockFrameCounter = rate;
}

void
Timer::lockFrameCounter()
{    
    const double timediff_limit= 1.f/(double)_lockFrameCounter;

    // Work out the elapsed time to now.    
    double elapsedTime = elapsedSeconds() - _lastSeconds;

    if (elapsedTime < timediff_limit)    
    {
        preciseSleep(timediff_limit - (elapsedTime));
    }
}

double
Timer::elapsedSeconds()
{
    LARGE_INTEGER time = _qwTime;
    HANDLE thread = GetCurrentThread();
    DWORD_PTR oldMask = SetThreadAffinityMask(thread, _timerMask);
    QueryPerformanceCounter(&time);
    SetThreadAffinityMask(thread, oldMask);

    // scale by 1000 for milliseconds
    LONGLONG newTime = time.QuadPart - _startTime.QuadPart;
    unsigned long newTicks = (unsigned long) (1000 * newTime / _llQPFTicksPerSec);

    // detect and compensate for performance counter leaps
    // (surprisingly common, see Microsoft KB: Q274323)
    uint64_t check = GetTickCount() - _startTick;
    signed long msecOff = (signed long)(newTicks - check);
    if (msecOff < -100 || msecOff > 100)
    {
        // We must keep the timer running forward :)
        LONGLONG adjust = (std::min)((LONGLONG)(msecOff * _llQPFTicksPerSec / 1000), (LONGLONG)(time.QuadPart - _qwTime.QuadPart));
        _startTime.QuadPart += adjust;
        newTime -= adjust;

        // Re-calculate milliseconds
        newTicks = (unsigned long) (1000 * newTime / _llQPFTicksPerSec);
    }

    _qwTime = time;
    return (double)(newTicks) / 1000.0;
}

bool     
Timer::update (bool stop)
{
    if (!_timerStarted)
    {
       startTimer();
    }

    if (_lockFrameCounter > 0)
    {
        lockFrameCounter();
    }
    
    _llLastElapsedTime = _qwTime.QuadPart;
    
    double seconds = elapsedSeconds();
    _elapsedTime = seconds - _lastSeconds;
    _lastSeconds = seconds;
    
    _appTime  = (double)( _qwTime.QuadPart - _startTime.QuadPart ) / 
            (double) _llQPFTicksPerSec;
    _absTime  = _qwTime.QuadPart / (double) _llQPFTicksPerSec;
    _normalizedTime  = -(floor (_appTime) - _appTime);

    _frameRateCounter++;
    _frameRateTimeElapsed += (float)(_elapsedTime);

    if (_frameRateTimeElapsed > 1.0)
    {
        _frameRateTimeElapsed = 0.0f;
        _frameRate = _frameRateCounter;
        _frameRateCounter = 0;
    }

    return true;
}

void
Timer::reset()
{
    update();

    _llBaseTime        = _qwTime.QuadPart;
    _llLastElapsedTime = _qwTime.QuadPart;

    _appTime = 0;
    _absTime = 0;
    _elapsedTime = 0;
}

void
Timer::startTimer()
{
    _timerStarted = true;
    update();

    _llBaseTime = _llBaseTime + _qwTime.QuadPart - _llStopTime;
    _llStopTime = 0;
    _llLastElapsedTime = _qwTime.QuadPart;
}

void
Timer::stopTimer()
{
    _llLastElapsedTime = _qwTime.QuadPart;
}

uint32_t
Timer::frameRate() const
{
    return _frameRate;
}

const double& 
Timer::absoluteTime() const 
{
    return _absTime;
}

const double&   
Timer::elapsedTime() const
{
    return _elapsedTime;
}

const double&
Timer::demoTime() const
{
    return _appTime;
}
}