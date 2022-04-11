#pragma once

#include "pch.h"

namespace mixi
{

class LogAdaptor
{
friend class Log;
public:

    using Ptr = std::shared_ptr<LogAdaptor>;

private:

    virtual void info(const std::string& content);
    virtual void warning(const std::string& content);
    virtual void error(const std::string& content);
    virtual void fatal(const std::string& content);

};

class Log
{
public:

    static void Info(const std::string& content);
    static void Warning(const std::string& content);
    static void Error(const std::string& content);
    static void Fatal(const std::string& content);

    static void SetAdaptor(const LogAdaptor::Ptr& adaptor);

private:

    static LogAdaptor::Ptr Adaptor_;

};

} // namespace mixi