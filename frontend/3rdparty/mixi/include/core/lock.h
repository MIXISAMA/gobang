#pragma once
#include "pch.h"

namespace mixi
{



template <typename T>
class ReadFirstBuffer
{
    // friend class ReadFirstBuffer::R;
public:

    template<typename... Args>
    ReadFirstBuffer(Args&&... args) :
        buffer_{T(std::forward<Args>(args)...), T(std::forward<Args>(args)...)}
        // read_id_(0),
        // r(*this),
        // w(*this)
    {

    }

    class R
    {
    public:
        R(ReadFirstBuffer& rfb) :
            rfb_(rfb)
        {
            mutex_.lock();
        }
        ~R()
        {
            mutex_.unlock();
        }
        const T* operator -> () const noexcept
        {
            return &rfb_.buffer_[rfb_.read_id_];
        }
    private:
        ReadFirstBuffer& rfb_;
    };

    class W
    {
    public:
        W(ReadFirstBuffer& rfb) :
            rfb_(rfb)
        {
            rfb_.buffer_[rfb_.read_id_ ^ 1] = rfb_.buffer_[rfb_.read_id_];
        }
        ~W()
        {
            rfb_.mutex_.lock();
            rfb_.read_id_ ^= 1;
            rfb_.mutex_.unlock();
        }
        T* operator -> () const noexcept
        {
            return &rfb_.buffer_[rfb_.read_id_ ^ 1];
        }
    private:
        ReadFirstBuffer& rfb_;
    };

    R r()
    {
        return R(*this);
    }

    W w()
    {
        return W(*this);
    }

protected:

    std::mutex mutex_;
    int read_id_;
    T buffer_[2];

};

} // namespace mixi