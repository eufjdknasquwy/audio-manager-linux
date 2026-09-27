#include "gui/MicrophoneWindow.h"
#include "gui/AudioWindow.h"
#include "modules/MicrophoneManager.h"
#include "modules/PulseClient.h"
#include <gtkmm.h>
Gui::MicrophoneWindow::MicrophoneWindow(AudioWindow *window, Gtk::Box *microPage,
                                        void (*setMarginAll)(Gtk::Widget &, int))
{
    m_microphoneManager = std::make_unique<Modules::MicrophoneManager>(this);
    m_audioWindow = window;
    m_setMarginAll = setMarginAll;
    m_microPage = microPage;
    m_pulse = std::make_unique<Modules::PulseClient>();

    CreateLabel();
    CreateScale();
    CreateCombo();
    CreateSeparator();
    CreateEntryVolume();
    CreateEntryVolumeTelegram();
    CreateResetVolume();
}
Gui::MicrophoneWindow::~MicrophoneWindow() = default;
void Gui::MicrophoneWindow::RefreshVolume()
{
    int volume = m_pulse->GetDefaultSourceVolume();
    m_scaleConn.block();
    m_scale->set_value(volume);
    m_scaleConn.unblock();
}
void Gui::MicrophoneWindow::RefreshDevices()
{
    m_combo->remove_all();
    FillCombo();
}
void Gui::MicrophoneWindow::CreateResetVolume()
{
    m_resetButton = Gtk::manage(new Gtk::Button("Сбросить громкость"));
    m_resetButton->signal_clicked().connect(
        sigc::mem_fun(*m_microphoneManager, &Modules::MicrophoneManager::ResetVolume));
    m_microPage->pack_start(*m_resetButton, false, false, 0);
}
void Gui::MicrophoneWindow::CreateLabel()
{
    m_label = Gtk::manage(new Gtk::Label("Выбор микрофона"));
    m_label->set_halign(Gtk::ALIGN_START);
    m_microPage->pack_start(*m_label, false, false, 0);
}
void Gui::MicrophoneWindow::FillCombo()
{
    std::string displayName = "";
    std::vector<std::string> availableMicrophones = m_pulse->GetSources();
    for (const std::string &microName : availableMicrophones)
    {
        if (microName == "bluez_input.E4:61:F4:13:DF:08")
            displayName = "JBL Tune 520BT";
        else if (microName == "easyeffects_source")
            displayName = "EasyEffects";
        else
            displayName = microName;
        m_combo->append(displayName);
    }
    m_combo->set_active(-1);
}
void Gui::MicrophoneWindow::CreateCombo()
{
    m_combo = Gtk::manage(new Gtk::ComboBoxText());
    FillCombo();
    m_combo->signal_changed().connect(sigc::mem_fun(*m_microphoneManager, &Modules::MicrophoneManager::ChangeSource));
    m_microPage->pack_start(*m_combo, false, false, 0);
}
void Gui::MicrophoneWindow::CreateSeparator()
{
    m_separator = Gtk::manage(new Gtk::Label("Громкость"));
    m_separator->set_halign(Gtk::ALIGN_START);
    m_microPage->pack_start(*m_separator, false, false, 0);
    m_microPage->pack_start(*m_scale, false, false, 0);
}
void Gui::MicrophoneWindow::CreateScale()
{
    m_scale = Gtk::manage(new Gtk::Scale(Gtk::ORIENTATION_HORIZONTAL));
    m_scale->set_range(0, 300);
    int volume = m_pulse->GetDefaultSourceVolume();
    m_scale->set_value(volume);
    m_scale->set_digits(0);
    m_scale->set_increments(1, 1);
    m_scaleConn = m_scale->signal_value_changed().connect(
        sigc::mem_fun(*m_microphoneManager, &Modules::MicrophoneManager::ChangeVolume));
}
void Gui::MicrophoneWindow::CreateEntryVolume()
{
    m_entryVolume = Gtk::manage(new Gtk::Entry());
    m_entryVolume->set_placeholder_text("Громкость микрофона");
    m_entryVolume->signal_activate().connect(
        sigc::mem_fun(*m_microphoneManager, &Modules::MicrophoneManager::EntryVolume));
    m_microPage->pack_start(*m_entryVolume, false, false, 0);
}
void Gui::MicrophoneWindow::CreateEntryVolumeTelegram()
{
    m_entryVolumeTelegram = Gtk::manage(new Gtk::Entry());
    m_entryVolumeTelegram->set_placeholder_text("Громкость микрофона (тг)");
    m_entryVolumeTelegram->signal_activate().connect(
        sigc::mem_fun(*m_microphoneManager, &Modules::MicrophoneManager::EntryVolumeTelegram));
    m_microPage->pack_start(*m_entryVolumeTelegram, false, false, 0);
}
