#pragma once
#include <Types.h>
#include <chrono>
using namespace std::chrono;

namespace Goi
{

    class Timer
    {
    public:
        Timer();
        ~Timer();

        void Tick();

        f32  DeltaNano() const;
        f32  DeltaMicro() const;
        f32  DeltaMilli() const;
        f32  DeltaSec() const;

        f32  TotalNano() const;
        f32  TotalMicro() const;
        f32  TotalMilli() const;
        f32  TotalSec() const;

    private:
        using TimeType = time_point<steady_clock>;

        TimeType m_t0;
        f32      m_deltaNano;
        f32      m_totalNano;
    };

}