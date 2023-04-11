#include <Timer.h>

namespace Goi
{

    Timer::Timer()
    {
        m_t0 = high_resolution_clock::now();
    }

    Timer::~Timer()
    {
    }

    void Timer::Tick()
    {
        auto t1 = high_resolution_clock::now();

        auto elapsed = t1 - m_t0;
        m_deltaNano  = static_cast<f32>( elapsed.count() );
        m_totalNano += m_deltaNano;

        m_t0 = t1;
    }

    f32 Timer::DeltaNano() const
    {
        return m_deltaNano;
    }

    f32 Timer::DeltaMicro() const
    {
        return m_deltaNano * 1e-3;
    }

    f32 Timer::DeltaMilli() const
    {
        return m_deltaNano * 1e-6;
    }

    f32 Timer::DeltaSec() const
    {
        return m_deltaNano * 1e-9;
    }

    f32 Timer::TotalNano() const
    {
        return m_totalNano;
    }

    f32 Timer::TotalMicro() const
    {
        return m_totalNano * 1e-3;
    }

    f32 Timer::TotalMilli() const
    {
        return m_totalNano * 1e-6;
    }

    f32 Timer::TotalSec() const
    {
        return m_totalNano * 1e-9;
    }

}