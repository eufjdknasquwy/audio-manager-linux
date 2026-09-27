#pragma once
#include <memory>
#include <pulse/pulseaudio.h>
#include <string>
namespace Gui
{
    class MicrophoneWindow;
}
namespace Modules
{
    class PulseClient;
    class MicrophoneManager
    {
        public:
            MicrophoneManager(Gui::MicrophoneWindow *window, std::shared_ptr<Modules::PulseClient> pulse);
            ~MicrophoneManager();

            void UpdateScale();
            void ChangeSource();
            void ChangeVolume();
            void EntryVolume();
            void EntryVolumeTelegram();
            void ResetVolume();

        private:
            Gui::MicrophoneWindow *m_microphoneWindow = nullptr;
            std::shared_ptr<PulseClient> m_pulse;
            pa_mainloop *m_paMainloop = nullptr;
            pa_context *m_paContext = nullptr;
            static constexpr int DEFAULT_SOURCE_VOLUME = 100;
            static constexpr const char *STRING_TO_INT_ERR = "Failed to convert string to int\n";
            static constexpr const char *CREATE_OPERATION_ERR = "Failed to create operation\n";

            struct SourceOutputData
            {
                    uint32_t firstId = 0;
                    bool found = false;
                    bool done = false;
            };

            void SetDefaultSource(const std::string &sourceName);
            void SetSourceVolume(int targetVolume);
            int GetTelegramSourceOutputId();
            static void SourceOutputListCallback(pa_context *, const pa_source_output_info *info, int eol,
                                                 void *userdata);
    };
} // namespace Modules
