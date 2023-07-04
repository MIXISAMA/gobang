#pragma once
#include "pch.h"

namespace mixi
{



template <typename T>
class ReadFirstBuffer : public std::exception
{
    friend class ReadFirstBuffer::R;
public:

    class R
    {
    public:
        R(ReadFirstBuffer& rfb) : rfb_(rfb) {}
        T* operator -> () const noexcept
        {
            return rfb_.buffer_[read_id_];
        }
    private:
        ReadFirstBuffer& rfb_;
    };

    class W
    {
        T* operator -> () const noexcept
        {
            return &buffer_[read_id_ ^ 1];
        }
    };
    
    template <typename ...Args>
    ReadFirstBuffer(Args&&... args) :
        buffer_(new T[2](args...)),
        read_id_(0),
        r(*this),
        w(*this)
    {
    
    }

    ~ReadFirstBuffer()
    {
        delete [] buffer_;
    }

    R r;
    W w;

    
    class r_lock_guard
    {
    public:
        r_lock_guard()
        {
            mutex_.lock();
        }
        ~r_lock_guard()
        {
            mutex_.unlock();
        }
    };

protected:

    std::mutex mutex_;
    std::atomic_int read_id_;
    T* buffer_;

};

} // namespace mixi