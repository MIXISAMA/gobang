#pragma once
#include "pch.h"

namespace mixi
{



template <typename T>
class ReadFirstBuffer
{
    template <typename U>
    friend class RfbReader;
    template <typename U>
    friend class RfbWriter;
public:

    template<typename... Args>
    ReadFirstBuffer(Args&&... args) :
        buffer_{T(std::forward<Args>(args)...), T(std::forward<Args>(args)...)},
        read_id_(0) {}

    ReadFirstBuffer(const ReadFirstBuffer&) = delete;
    ReadFirstBuffer(ReadFirstBuffer&&) = delete;

protected:

    std::shared_mutex mutex_;
    int read_id_;
    T buffer_[2];

};

template <typename T>
class RfbReader
{
public:
    RfbReader(const ReadFirstBuffer<T>& rfb) :
        rfb_(rfb)
    {
        rfb_.mutex_.lock_shared();
    }
    ~RfbReader()
    {
        rfb_.mutex_.unlock_shared();
    }
    const T* operator -> () const noexcept
    {
        return &rfb_.buffer_[rfb_.read_id_];
    }
private:
    const ReadFirstBuffer<T>& rfb_;
};

template <typename T>
class RfbWriter
{
public:
    RfbWriter(ReadFirstBuffer<T>& rfb) :
        rfb_(rfb)
    {
        rfb_.mutex_.lock_shared();
        rfb_.buffer_[rfb_.read_id_ ^ 1] = rfb_.buffer_[rfb_.read_id_];
        rfb_.mutex_.unlock_shared();
    }
    ~RfbWriter()
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
    ReadFirstBuffer<T>& rfb_;
};


template <typename T>
class ReadTryQueue
{
    template <typename U>
    friend class RtqReader;
    template <typename U>
    friend class RtqWriter;
public:

    ReadTryQueue() = default;
    ReadTryQueue(const ReadTryQueue&) = delete;
    ReadTryQueue(ReadTryQueue&&) = delete;

protected:
    std::mutex mutex_;
    std::queue<T> queue_;
};


template <typename T>
class RtqReader
{
public:
    RtqReader(ReadTryQueue<T>& rtq) :
        rtq_(rtq)
    {
        locked_ = rtq_.mutex_.try_lock();
    }
    ~RtqReader()
    {
        if (locked_) {
            rtq_.mutex_.unlock();
        }
    }
    bool locked()
    {
        return locked_;
    }
    bool empty() {
        return rtq_.queue_.empty();
    }
    T pop()
    {
        if (!locked_) {
            throw std::runtime_error("pop but have not locked");
        }
        T item = rtq_.queue_.front();
        rtq_.queue_.pop();
        return item;
    }
private:
    bool locked_;
    ReadTryQueue<T>& rtq_;
};

template <typename T>
class RtqWriter
{
public:
    RtqWriter(ReadTryQueue<T>& rtq) :
        rtq_(rtq)
    {
        rtq_.mutex_.lock();
    }
    ~RtqWriter()
    {
        rtq_.mutex_.unlock();
    }
    void push(const T& item)
    {
        rtq_.queue_.push(item);
    }
    template <typename... Args>
    void emplace(Args&&... args)
    {
        rtq_.queue_.emplace(std::forward<Args>(args)...);
    }
private:
    ReadTryQueue<T>& rtq_;
};


} // namespace mixi