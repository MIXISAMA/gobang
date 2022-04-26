#include "core/log.h"

namespace mixi
{

void LogAdaptor::info(const std::string& content)
{
    std::cout << "[INFO] " << content << std::endl;
}

void LogAdaptor::warning(const std::string& content)
{
    std::cout << "[WARN] " << content << std::endl;
}

void LogAdaptor::error(const std::string& content)
{
    std::cout << "[ERRO] " << content << std::endl;
}

void LogAdaptor::fatal(const std::string& content)
{
    std::cout << "[FATA] " << content << std::endl;
}

LogAdaptor::Ptr Log::Adaptor_ = LogAdaptor::Ptr(new LogAdaptor());

void Log::Info(const std::string& content)
{
    Adaptor_->info(content);
}

void Log::Warning(const std::string& content)
{
    Adaptor_->warning(content);
}

void Log::Error(const std::string& content)
{
    Adaptor_->error(content);
}

void Log::Fatal(const std::string& content)
{
    Adaptor_->fatal(content);
}

void Log::SetAdaptor(const LogAdaptor::Ptr& adaptor)
{
    Adaptor_ = adaptor;
}

} // namespace mixi
