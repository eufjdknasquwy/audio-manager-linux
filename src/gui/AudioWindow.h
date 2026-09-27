#pragma once
#include "modules/PulseClient.h"
#include <gtkmm.h>
#include <sigc++/connection.h>
#include <sys/types.h>
namespace Modules
{
    class AudioManager;
    class Tray;
    class Parser;
    class GetAudio;
} // namespace Modules
namespace Gui
{
    class MicrophoneWindow;
    class CssHelper;
    class AudioWindow
    {
        public:
            AudioWindow();  // конструктор
            ~AudioWindow(); // деструктор
            Gtk::Window &GetWindow();
            void Show();
            Gtk::ComboBoxText *GetCombo();
            Gtk::Scale *GetScale();
            bool RefreshDevices();
            enum ResponseId
            {
                RESPONSE_HIDE = 1,
                RESPONSE_QUIT = 2,
            };
            sigc::connection m_scaleConn;

        private:
            // поля
            static constexpr int DEFAULT_MARGIN = 20;
            static constexpr int DEFAULT_WINDOW_WIDTH = 300;
            static constexpr int DEFAULT_WINDOW_HEIGHT = 280;

            std::unique_ptr<Modules::AudioManager> m_audioManager;
            std::unique_ptr<Modules::Tray> m_tray;
            std::unique_ptr<Gui::MicrophoneWindow> m_microphoneWindow;
            std::shared_ptr<Modules::PulseClient> m_pulse;

            Gtk::Dialog *m_dialog = nullptr;
            Gtk::Box *m_content = nullptr;
            Gtk::Notebook *m_notebook = nullptr;
            Gtk::Box *m_audioPage = nullptr;
            Gtk::Box *m_microPage = nullptr;
            Gtk::Label *m_label = nullptr;
            Gtk::ComboBoxText *m_combo = nullptr;
            Gtk::Label *m_separator = nullptr;
            Gtk::Scale *m_scale = nullptr;
            Gtk::Button *m_resetButton = nullptr;
            // методы
            bool RefreshVolume();
            static void SetMarginAll(Gtk::Widget &widget, int size);
            void CreateResetVolume();
            void CreatePages();
            void OnSwitchPage(Gtk::Widget * /*page*/, guint page_num);
            void CreateLabel();
            void FillCombo();
            void CreateCombo();
            void CreateSeparator();
            void CreateScale();
    };
} // namespace Gui
