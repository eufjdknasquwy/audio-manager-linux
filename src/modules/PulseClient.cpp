#include "modules/PulseClient.h"
#include <iostream>
#include <string>
#include <vector>
Modules::PulseClient::PulseClient()
{
    m_ml = pa_mainloop_new();
    m_api = pa_mainloop_get_api(m_ml);
    m_ctx = pa_context_new(m_api, "audio-manager");
    if (pa_context_connect(m_ctx, nullptr, PA_CONTEXT_NOFLAGS, nullptr) < 0)
    {
        std::cerr << "PulseAudio connect failed: " << pa_strerror(pa_context_errno(m_ctx)) << "\n";
        pa_context_unref(m_ctx);
        pa_mainloop_free(m_ml);
        m_ctx = nullptr;
        m_ml = nullptr;
        return;
    }
    if (!WaitForReady())
    {
        std::cerr << "PulseAudio context not ready\n";
        return;
    }
    pa_context_set_subscribe_callback(m_ctx, &PulseClient::SubscribeCb, this);
    pa_operation *op = pa_context_subscribe(m_ctx, PA_SUBSCRIPTION_MASK_SERVER, nullptr, nullptr);
    if (op)
        pa_operation_unref(op);
}
Modules::PulseClient::~PulseClient()
{
    if (m_ctx)
    {
        pa_context_disconnect(m_ctx);
        pa_context_unref(m_ctx);
    }
    if (m_ml)
        pa_mainloop_free(m_ml);
}
bool Modules::PulseClient::IsReady() const
{
    return m_ctx && pa_context_get_state(m_ctx) == PA_CONTEXT_READY;
}
bool Modules::PulseClient::WaitForReady()
{
    if (!m_ctx)
        return false;
    while (true)
    {
        pa_mainloop_iterate(m_ml, DEFAULT_ML_ITERATE_BLOCK, nullptr);
        auto state = pa_context_get_state(m_ctx);
        if (state == PA_CONTEXT_READY)
            return true;
        if (state == PA_CONTEXT_FAILED || state == PA_CONTEXT_TERMINATED)
            return false;
    }
}
void Modules::PulseClient::SubscribeCb(pa_context *, pa_subscription_event_type_t t, uint32_t /*idx*/, void *userdata)
{
    if (!userdata)
        return;

    auto *self = static_cast<PulseClient *>(userdata);

    auto facility = t & PA_SUBSCRIPTION_EVENT_FACILITY_MASK;
    auto type = t & PA_SUBSCRIPTION_EVENT_TYPE_MASK;

    if (facility == PA_SUBSCRIPTION_EVENT_SERVER && type == PA_SUBSCRIPTION_EVENT_CHANGE)
    {
        self->m_signal_default_changed.emit();
    }
}
namespace Modules
{
    struct SinksListData
    {
            std::vector<std::string> sinks;
            bool done = false;
    };
    struct SourcesListData
    {
            std::vector<std::string> sources;
            bool done = false;
    };
    struct SinkVolumeData
    {
            int volume = -1;
            bool done = false;
    };
    struct SourceVolumeData
    {
            int volume = -1;
            bool done = false;
    };
} // namespace Modules
// списки устройств
void Modules::PulseClient::SinksListCb(pa_context *, const pa_sink_info *info, int eol, void *userdata)
{
    auto *data = static_cast<SinksListData *>(userdata);
    if (eol < 0)
    {
        data->done = true;
        return;
    }
    if (eol > 0)
    {
        data->done = true;
        return;
    }
    if (info && info->name)
        data->sinks.emplace_back(info->name);
}
std::vector<std::string> Modules::PulseClient::GetSinks()
{
    SinksListData data;
    pa_operation *op = pa_context_get_sink_info_list(m_ctx, SinksListCb, &data);

    if (!op)
        return data.sinks;

    while (!data.done)
        pa_mainloop_iterate(m_ml, DEFAULT_ML_ITERATE_BLOCK, nullptr);

    pa_operation_unref(op);
    return data.sinks;
}
void Modules::PulseClient::SourcesListCb(pa_context *, const pa_source_info *info, int eol, void *userdata)
{
    auto *data = static_cast<SourcesListData *>(userdata);
    if (eol < 0)
    {
        data->done = true;
        return;
    }
    if (eol > 0)
    {
        data->done = true;
        return;
    }
    if (info && info->name)
        data->sources.emplace_back(info->name);
}
std::vector<std::string> Modules::PulseClient::GetSources()
{
    SourcesListData data;
    pa_operation *op = pa_context_get_source_info_list(m_ctx, SourcesListCb, &data);

    if (!op)
        return data.sources;

    while (!data.done)
        pa_mainloop_iterate(m_ml, DEFAULT_ML_ITERATE_BLOCK, nullptr);

    pa_operation_unref(op);
    return data.sources;
}
// громкость устройств
void Modules::PulseClient::SinkVolumeCb(pa_context *, const pa_sink_info *info, int eol, void *userdata)
{
    auto *data = static_cast<SinkVolumeData *>(userdata);
    if (eol != 0 || !info)
    {
        data->done = true;
        return;
    }
    pa_volume_t avg = pa_cvolume_avg(&info->volume);
    data->volume = static_cast<int>(avg * 100.0 / PA_VOLUME_NORM + 0.5);
    data->done = true;
}
int Modules::PulseClient::GetDefaultSinkVolume()
{
    SinkVolumeData data;
    pa_operation *op = pa_context_get_sink_info_by_name(m_ctx, "@DEFAULT_SINK@", SinkVolumeCb, &data);

    if (!op)
        return data.volume;

    while (!data.done)
        pa_mainloop_iterate(m_ml, DEFAULT_ML_ITERATE_BLOCK, nullptr);

    pa_operation_unref(op);
    return data.volume;
}
void Modules::PulseClient::SourceVolumeCb(pa_context *, const pa_source_info *info, int eol, void *userdata)
{
    auto *data = static_cast<SourceVolumeData *>(userdata);
    if (eol != 0 || !info)
    {
        data->done = true;
        return;
    }
    pa_volume_t avg = pa_cvolume_avg(&info->volume);
    data->volume = static_cast<int>(avg * 100.0 / PA_VOLUME_NORM + 0.5);
    data->done = true;
}
int Modules::PulseClient::GetDefaultSourceVolume()
{
    SourceVolumeData data;
    pa_operation *op = pa_context_get_source_info_by_name(m_ctx, "@DEFAULT_SOURCE@", SourceVolumeCb, &data);

    if (!op)
        return data.volume;

    while (!data.done)
        pa_mainloop_iterate(m_ml, DEFAULT_ML_ITERATE_BLOCK, nullptr);

    pa_operation_unref(op);
    return data.volume;
}
