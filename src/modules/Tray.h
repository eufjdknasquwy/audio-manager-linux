#pragma once
#include <gtkmm.h>
#include <string>
namespace Gui
{
    class AudioWindow;
}
namespace Modules
{
    class Tray
    {
        public:
            Tray(Gtk::Window &window, Gui::AudioWindow *audio_window = nullptr);
            ~Tray();

            void CreateTray(Gtk::Window &dialogWindow);

            void ToggleWindow();
            void OpenWindow();
            void HideWindow();
            void RefreshDevices();
            void RestartEf();
            void Quit();

            void OnDeleteEvent();
            void OnDialogResponse(int response_id);

        private:
            Gtk::Window &m_mainWindow;
            Gtk::Window *m_dialog = nullptr;
            Gui::AudioWindow *m_audioWindow = nullptr;

            Glib::RefPtr<Gtk::StatusIcon> m_indicator;
            Gtk::Menu m_menu;

            Gtk::MenuItem *m_open_app = nullptr;
            Gtk::MenuItem *m_hide_app = nullptr;
            Gtk::MenuItem *m_refresh_devices_item = nullptr;
            Gtk::MenuItem *m_restart_ef_item = nullptr;
            Gtk::MenuItem *m_quit_app = nullptr;

            Gtk::MenuItem *CreateMenuItem(const std::string &label, const std::string &icon_name,
                                          const std::function<void()> &callback);

            void InitializeMenuItems();
            void CreateMenu();
    };
} // namespace Modules
