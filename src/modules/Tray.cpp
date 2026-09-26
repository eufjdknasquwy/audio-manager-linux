#include "Tray.h"
#include "gui/AudioWindow.h"
#include <functional>
#include <glibmm.h>
#include <gtkmm.h>
#include <iostream>
Modules::Tray::Tray(Gtk::Window &window, Gui::AudioWindow *audio_window)
    : m_mainWindow(window), m_audioWindow(audio_window)
{
    InitializeMenuItems();
    CreateMenu();
}
Modules::Tray::~Tray() = default;
Gtk::MenuItem *Modules::Tray::CreateMenuItem(const std::string &label, const std::string &icon_name,
                                             const std::function<void()> &callback)
{
    Gtk::Box *box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 10));
    Gtk::Image *icon = nullptr;
    try
    {
        auto icon_theme = Gtk::IconTheme::get_default();
        auto pixbuf = icon_theme->load_icon(icon_name, Gtk::ICON_SIZE_MENU, Gtk::ICON_LOOKUP_GENERIC_FALLBACK);
        icon = Gtk::manage(new Gtk::Image(pixbuf));
    }
    catch (const Glib::Error &)
    {
        icon = Gtk::manage(new Gtk::Image());
    }
    box->pack_start(*icon, false, false, 0);
    Gtk::Label *text = Gtk::manage(new Gtk::Label(label));
    box->pack_start(*text, false, false, 0);

    Gtk::MenuItem *item = Gtk::manage(new Gtk::MenuItem());
    item->add(*box);
    item->signal_activate().connect(callback);
    return item;
}
void Modules::Tray::InitializeMenuItems()
{
    m_open_app = CreateMenuItem("Открыть", "window-new", [this]() { OpenWindow(); });

    m_hide_app = CreateMenuItem("Скрыть", "list-remove", [this]() { HideWindow(); });

    m_refresh_devices_item = CreateMenuItem("Обновить устройства", "view-refresh", [this]() { RefreshDevices(); });

    m_restart_ef_item =
        CreateMenuItem("Перезагрузить EasyEffects", "preferences-desktop-multimedia", [this]() { RestartEf(); });

    m_quit_app = CreateMenuItem("Выйти", "application-exit", [this]() { Quit(); });
}
void Modules::Tray::CreateMenu()
{
    m_menu.append(*m_open_app);
    m_menu.append(*m_hide_app);
    m_menu.append(*m_refresh_devices_item);
    m_menu.append(*m_restart_ef_item);
    auto *sep = Gtk::manage(new Gtk::SeparatorMenuItem());
    m_menu.append(*sep);
    // m_menu.append(*Gtk::manage(new Gtk::SeparatorMenuItem()));
    m_menu.append(*m_quit_app);
    m_menu.show_all();
}
// void Modules::Tray::CreateTray(Gtk::Window &dialogWindow)
// {
//     m_dialog = &dialogWindow;
//     // m_indicator = Gtk::StatusIcon::create();
//     auto icon_theme = Gtk::IconTheme::get_default();
//     Glib::RefPtr<Gdk::Pixbuf> pixbuf;
//     try
//     {
//         auto icon_theme = Gtk::IconTheme::get_default();
//         auto pixbuf = icon_theme->load_icon("audio-editor", 64, Gtk::ICON_LOOKUP_GENERIC_FALLBACK);
//         m_indicator->set(pixbuf);
//     }
//     catch (const Glib::Error &) // иконка не найдена
//     {
//         m_indicator = pixbuf ? Gtk::StatusIcon::create(pixbuf) : Gtk::StatusIcon::create("audio-editor");
//     }
//     m_indicator->set_visible(true);
//     m_indicator->set_tooltip_text("Audio Manager");
//
//     m_indicator->signal_button_press_event().connect(
//         [this](GdkEventButton *event) -> bool
//         {
//             if (event->button == 1) // ЛКМ
//                 ToggleWindow();
//             return false;
//         });
//
//     m_indicator->signal_popup_menu().connect([this](guint button, guint activate_time)
//                                              { m_menu.popup(button, activate_time); });
// }
void Modules::Tray::CreateTray(Gtk::Window &dialogWindow)
{
    m_dialog = &dialogWindow;

    Glib::RefPtr<Gdk::Pixbuf> pixbuf;
    try
    {
        auto icon_theme = Gtk::IconTheme::get_default();
        pixbuf = icon_theme->load_icon("audio-editor", 64, Gtk::ICON_LOOKUP_GENERIC_FALLBACK);
    }
    catch (const Glib::Error &)
    {
        // pixbuf остаётся пустым
    }

    if (pixbuf)
        m_indicator = Gtk::StatusIcon::create(pixbuf);
    else
        m_indicator = Gtk::StatusIcon::create("audio-editor");

    if (!m_indicator)
    {
        std::cerr << "Не удалось создать StatusIcon\n";
        return;
    }

    m_indicator->set_visible(true);
    m_indicator->set_tooltip_text("Audio Manager");

    m_indicator->signal_button_press_event().connect(
        [this](GdkEventButton *event) -> bool
        {
            if (event->button == 1)
                ToggleWindow();
            return false;
        });

    m_indicator->signal_popup_menu().connect([this](guint button, guint activate_time)
                                             { m_menu.popup(button, activate_time); });
}
void Modules::Tray::ToggleWindow()
{
    if (!m_dialog)
        return;
    if (m_dialog->get_visible())
        m_dialog->hide();
    else
    {
        m_dialog->show_all();
        m_dialog->present();
    }
}
void Modules::Tray::OpenWindow()
{
    if (!m_dialog)
        return;
    m_dialog->show_all();
    m_dialog->present();
}
void Modules::Tray::HideWindow()
{
    if (m_dialog)
        m_dialog->hide();
}
void Modules::Tray::RefreshDevices()
{
    if (m_audioWindow)
        m_audioWindow->RefreshDevices();
}
void Modules::Tray::RestartEf()
{
    Glib::spawn_command_line_async("/home/yegor/.restart-easyeffects.sh");
}
void Modules::Tray::Quit()
{
    if (auto app = Gtk::Application::get_default())
    {
        app->release();
        app->quit();
    }
    else
        Gtk::Main::quit();
}
void Modules::Tray::OnDeleteEvent()
{
    if (m_dialog)
        m_dialog->hide();
}
void Modules::Tray::OnDialogResponse(int response_id)
{
    if (response_id == Gui::AudioWindow::RESPONSE_HIDE)
    {
        if (m_dialog)
            m_dialog->hide();
    }
    else if (response_id == Gui::AudioWindow::RESPONSE_QUIT)
    {
        Quit();
    }
}
