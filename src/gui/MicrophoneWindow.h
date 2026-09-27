#pragma once
#include "gtkmm/comboboxtext.h"
#include "gtkmm/entry.h"
#include "gtkmm/scale.h"
#include "gui/AudioWindow.h"
#include "modules/PulseClient.h"
#include <memory>
#include <sigc++/connection.h>

namespace Modules
{
    class MicrophoneManager;
}
namespace Gui
{
    class MicrophoneWindow
    {
        public:
            MicrophoneWindow(AudioWindow *window, Gtk::Box *microPage, std::shared_ptr<Modules::PulseClient> pulse);
            ~MicrophoneWindow();
            void RefreshVolume();
            void RefreshDevices();
            Gtk::Scale *GetScale()
            {
                return m_scale;
            };
            Gtk::ComboBoxText *GetCombo()
            {
                return m_combo;
            };
            Gtk::Entry *GetEntry()
            {
                return m_entryVolume;
            };
            Gtk::Entry *GetEntryTelegram()
            {
                return m_entryVolumeTelegram;
            };
            sigc::connection m_scaleConn;

        private:
            std::unique_ptr<Modules::MicrophoneManager> m_microphoneManager;
            std::shared_ptr<Modules::PulseClient> m_pulse;
            AudioWindow *m_audioWindow = nullptr;
            void (*m_setMarginAll)(Gtk::Widget &, int) = nullptr;
            Gtk::Box *m_microPage = nullptr;

            Gtk::Label *m_label = nullptr;
            Gtk::ComboBoxText *m_combo = nullptr;
            Gtk::Label *m_separator = nullptr;
            Gtk::Scale *m_scale = nullptr;
            Gtk::Entry *m_entryVolume = nullptr;
            Gtk::Entry *m_entryVolumeTelegram = nullptr;
            Gtk::Button *m_resetButton = nullptr;

            void CreateLabel();
            void CreateScale();
            void FillCombo();
            void CreateCombo();
            void CreateSeparator();
            void CreateEntryVolume();
            void CreateEntryVolumeTelegram();
            void CreateResetVolume();
    };
} // namespace Gui
