#include "gui/AudioWindow.h"
#include "gui/CssHelper.h"
#include "gui/MicrophoneWindow.h"
#include "modules/AudioManager.h"
#include "modules/Parser.h"
#include "modules/PulseClient.h"
#include "modules/Tray.h"
#include <gtkmm.h>
#include <memory>
#include <string>
#include <vector>
Gui::AudioWindow::AudioWindow()
{
    m_pulse = std::make_shared<Modules::PulseClient>();
    m_dialog = new Gtk::Dialog("Выбор динамиков");
    m_dialog->set_position(Gtk::WIN_POS_CENTER);
    m_dialog->set_default_size(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
    m_dialog->set_resizable(false);
    m_content = m_dialog->get_content_area();
    m_audioManager = std::make_unique<Modules::AudioManager>(this, m_pulse);
    m_tray = std::make_unique<Modules::Tray>(*m_dialog, this);

    m_dialog->signal_delete_event().connect(
        [this](GdkEventAny *) -> bool
        {
            m_tray->OnDeleteEvent();
            return true;
        });
    m_dialog->add_button("Свернуть", Gui::AudioWindow::RESPONSE_HIDE);
    m_dialog->add_button("Закрыть", Gui::AudioWindow::RESPONSE_QUIT);
    m_dialog->signal_response().connect([this](int response_id) { m_tray->OnDialogResponse(response_id); });

    CreatePages();
    m_microphoneWindow = std::make_unique<Gui::MicrophoneWindow>(this, m_microPage, m_pulse);
    CreateLabel();
    CreateScale();
    CreateCombo();
    CreateSeparator();
    CreateResetVolume();
    CssHelper::ApplyCss();
    m_tray->CreateTray(*m_dialog);

    Glib::signal_timeout().connect(sigc::mem_fun(*this, &AudioWindow::RefreshVolume), 1500);
    Glib::signal_timeout().connect(sigc::mem_fun(*this, &AudioWindow::RefreshDevices), 3600000);
}
Gui::AudioWindow::~AudioWindow()
{
    delete m_dialog;
}
Gtk::Window &Gui::AudioWindow::GetWindow()
{
    return *m_dialog;
}
void Gui::AudioWindow::Show()
{
    if (!Modules::Parser::Tray)
        m_dialog->show_all();
}
Gtk::ComboBoxText *Gui::AudioWindow::GetCombo()
{
    return m_combo;
}
Gtk::Scale *Gui::AudioWindow::GetScale()
{
    return m_scale;
}
bool Gui::AudioWindow::RefreshVolume()
{
    if (m_dialog->get_visible())
    {
        int volume = m_pulse->GetDefaultSinkVolume();
        m_scaleConn.block();
        m_scale->set_value(volume);
        m_scaleConn.unblock();
        m_microphoneWindow->RefreshVolume();
    }
    return true;
}
bool Gui::AudioWindow::RefreshDevices()
{
    m_combo->remove_all();
    FillCombo();
    m_microphoneWindow->RefreshDevices();
    return true;
}
void Gui::AudioWindow::SetMarginAll(Gtk::Widget &widget, int size)
{
    widget.set_margin_top(size);
    widget.set_margin_bottom(size);
    widget.set_margin_start(size);
    widget.set_margin_end(size);
}
void Gui::AudioWindow::CreateResetVolume()
{
    m_resetButton = Gtk::manage(new Gtk::Button("Сбросить громкость"));
    m_resetButton->signal_clicked().connect(sigc::mem_fun(*m_audioManager, &Modules::AudioManager::ResetVolume));
    m_audioPage->pack_start(*m_resetButton, false, false, 0);
}
void Gui::AudioWindow::CreatePages()
{
    m_notebook = Gtk::manage(new Gtk::Notebook());
    m_content->add(*m_notebook);
    m_audioPage = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, 10));
    m_microPage = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, 10));
    SetMarginAll(*m_audioPage, DEFAULT_MARGIN);
    SetMarginAll(*m_microPage, DEFAULT_MARGIN);
    m_notebook->append_page(*m_audioPage, *Gtk::manage(new Gtk::Label("Динамики")));
    m_notebook->append_page(*m_microPage, *Gtk::manage(new Gtk::Label("Микрофоны")));
    m_notebook->signal_switch_page().connect(sigc::mem_fun(*this, &AudioWindow::OnSwitchPage));
    if (Modules::Parser::Input)
    {
        Glib::signal_timeout().connect(
            [this]()
            {
                m_notebook->set_current_page(1);
                m_dialog->set_title("Выбор микрофона");
                return false;
            },
            50);
    }
    else if (Modules::Parser::Output)
    {
        Glib::signal_timeout().connect(
            [this]()
            {
                m_notebook->set_current_page(0);
                m_dialog->set_title("Выбор динамиков");
                return false;
            },
            50);
    }
}
void Gui::AudioWindow::OnSwitchPage(Gtk::Widget * /*page*/, guint page_num)
{
    switch (page_num)
    {
    case 0:
        m_dialog->set_title("Выбор динамиков");
        break;
    case 1:
        m_dialog->set_title("Выбор микрофона");
        break;
    }
}
void Gui::AudioWindow::CreateLabel()
{
    m_label = Gtk::manage(new Gtk::Label("Выбор динамиков"));
    m_label->set_halign(Gtk::ALIGN_START);
    m_audioPage->pack_start(*m_label, false, false, 0);
}
void Gui::AudioWindow::FillCombo()
{
    std::string displayName = "";
    std::vector<std::string> availableSinks = m_pulse->GetSinks();
    for (const std::string &sinkName : availableSinks)
    {
        if (sinkName == "bluez_output.E4_61_F4_13_DF_08.1")
            displayName = "JBL Tune 520BT";
        else if (sinkName == "easyeffects_sink")
            displayName = "EasyEffects";
        else
            displayName = sinkName;
        m_combo->append(displayName);
    }
    m_combo->set_active(-1);
}
void Gui::AudioWindow::CreateCombo()
{
    m_combo = Gtk::manage(new Gtk::ComboBoxText());
    FillCombo();
    m_combo->signal_changed().connect(sigc::mem_fun(*m_audioManager, &Modules::AudioManager::ChangeSink));
    m_audioPage->pack_start(*m_combo, false, false, 0);
}
void Gui::AudioWindow::CreateSeparator()
{
    m_separator = Gtk::manage(new Gtk::Label("Громкость"));
    m_separator->set_halign(Gtk::ALIGN_START);
    m_audioPage->pack_start(*m_separator, false, false, 0);
    m_audioPage->pack_start(*m_scale, false, false, 0);
}
void Gui::AudioWindow::CreateScale()
{
    m_scale = Gtk::manage(new Gtk::Scale(Gtk::ORIENTATION_HORIZONTAL));
    m_scale->set_range(0, 100);
    int volume = m_pulse->GetDefaultSinkVolume();
    m_scale->set_value(volume);
    m_scale->set_digits(0);
    m_scale->set_increments(1, 1);
    m_scaleConn =
        m_scale->signal_value_changed().connect(sigc::mem_fun(*m_audioManager, &Modules::AudioManager::ChangeVolume));
}
