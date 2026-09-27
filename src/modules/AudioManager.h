#pragma once
#include <memory>
#include <pulse/pulseaudio.h>
#include <string>
namespace Gui
{
    class AudioWindow;
}
namespace Modules
{
    class PulseClient;
    class AudioManager
    {
        public:
            AudioManager(Gui::AudioWindow *window, std::shared_ptr<Modules::PulseClient> pulse);
            ~AudioManager();
            void UpdateScale();
            void ChangeSink();
            void ChangeVolume();
            void ResetVolume();

        private:
            static constexpr int DEFAULT_ML_ITERATE_BLOCK = 1;
            Gui::AudioWindow *m_audioWindow = nullptr;
            std::shared_ptr<PulseClient> m_pulse;
            pa_mainloop *m_paMainloop = nullptr;
            pa_context *m_paContext = nullptr;
            static constexpr int DEFAULT_SINK_VOLUME = 50;
            static constexpr const char *CREATE_OPERATION_ERR = "Failed to create operation\n";

            void SetDefaultSink(const std::string &sinkName);
            void SetSinkVolume(int targetVolume);
    };
} // namespace Modules
