#include <System/Utility/Logger/Logger.hh>

#include <string>
#include <string_view>
#include <vector>

#include <System/Utility/Logger/MessageInternal.hh>
#include <System/Utility/Logger/ProcessorInternal.hh>


namespace System::Utility::Logger
{


//! @TODO: refactor this facilities stuff.
struct facility_info
{
    std::string     name;
    priority        minPriority;
};

static priority g_minPriority = priority::info;

static std::vector<facility_info> g_facilities;


facility registerFacility(const char* name_ptr, std::size_t name_len)
{
    std::string_view name(name_ptr, name_len);
    g_facilities.emplace_back(name, priority::all);
    return static_cast<facility>(g_facilities.size());
}

void setMinimumPriority(priority p)
{
    g_minPriority = p;
}

void setFacilityMinimumPriority(facility f, priority p)
{
    auto& info = g_facilities.at(static_cast<std::size_t>(f) - 1);
    info.minPriority = p;
}

priority minimumLogPriority(facility f)
{
    if (f == DefaultFacility)
        return g_minPriority;

    return g_facilities.at(static_cast<std::size_t>(f) - 1).minPriority;
}

void log(facility f, priority p, const char* msg, std::size_t msg_len, const log_variable_raw* vars, std::size_t nvars)
{
    if (p < g_minPriority)
        return;

    std::string_view facility_name;
    if (f != DefaultFacility)
    {
        const auto& info = g_facilities.at(static_cast<std::uint32_t>(f) - 1);
        if (p < info.minPriority)
            return;
        facility_name = info.name;
    }

    auto message = message_internal::create(facility_name, p, {msg, msg_len});
    for (std::size_t v = 0; v < nvars; ++v)
    {
        const auto& var = vars[v];
        message->addVariable({var.first.first, var.first.second}, var_type::string, {reinterpret_cast<const std::byte*>(var.second.first), var.second.second});
    }

    message->finalise();

    processor_base::handleMessage(message);
}


} // namespace System::Utility::Logger