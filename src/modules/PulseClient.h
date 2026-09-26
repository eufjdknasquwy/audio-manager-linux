#pragma once
#include <pulse/pulseaudio.h>
#include <string>
#include <vector>
namespace Modules
{
    class PulseClient
    {
        public:
            PulseClient();
            ~PulseClient();

            PulseClient(const PulseClient &) = delete;
            PulseClient &operator=(const PulseClient &) = delete;

            bool IsReady() const;

            pa_mainloop *GetMainloop() const
            {
                return m_ml;
            }
            pa_context *GetContext() const
            {
                return m_ctx;
            }
            template <typename Predicate> void WaitFor(Predicate pred)
            {
                while (!pred() && IsReady())
                    pa_mainloop_iterate(m_ml, 0, nullptr);
            }
            // списки устройств
            static void SinksListCb(pa_context *, const pa_sink_info *info, int eol, void *userdata);
            static void SourcesListCb(pa_context *, const pa_source_info *info, int eol, void *userdata);
            std::vector<std::string> GetSinks();
            std::vector<std::string> GetSources();

            // громкость устройств
            static void SinkVolumeCb(pa_context *, const pa_sink_info *, int eol, void *userdata);
            static void SourceVolumeCb(pa_context *, const pa_source_info *, int eol, void *userdata);
            int GetDefaultSinkVolume();
            int GetDefaultSourceVolume();

        private:
            static constexpr int DEFAULT_ML_ITERATE_BLOCK = 1;
            pa_mainloop *m_ml = nullptr;
            pa_mainloop_api *m_api = nullptr;
            pa_context *m_ctx = nullptr;

            bool WaitForReady();
    };
} // namespace Modules
