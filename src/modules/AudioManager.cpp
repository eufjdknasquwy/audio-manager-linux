#include "modules/AudioManager.h"
#include "gui/AudioWindow.h"
#include "modules/PulseClient.h"
#include <cmath>
#include <iostream>
#include <pulse/pulseaudio.h>
Modules::AudioManager::AudioManager(Gui::AudioWindow *window, std::shared_ptr<Modules::PulseClient> pulse)
{
    m_audioWindow = window;
    m_pulse = pulse;
    m_paMainloop = m_pulse->GetMainloop();
    m_paContext = m_pulse->GetContext();
    m_pulse->signal_default_changed().connect([this]() { UpdateScale(); });
}
Modules::AudioManager::~AudioManager() = default;
void Modules::AudioManager::UpdateScale()
{
    std::cout << "UpdateScale called\n";
    if (!m_audioWindow)
        return;
    int volume = m_pulse->GetDefaultSinkVolume();
    std::cout << "  volume = " << volume << "\n";
    if (volume < 0)
        return;
    m_audioWindow->m_scaleConn.block();
    m_audioWindow->GetScale()->set_value(volume);
    m_audioWindow->m_scaleConn.unblock();
}
void Modules::AudioManager::ChangeSink()
{
    if (!m_audioWindow)
        return;
    Gtk::ComboBoxText *combo = m_audioWindow->GetCombo();
    std::string microName = combo->get_active_text();
    if (microName.empty())
        return;
    if (microName == "JBL Tune 520BT")
        microName = "bluez_output.E4_61_F4_13_DF_08.1";
    else if (microName == "EasyEffects")
        microName = "easyeffects_sink";

    SetDefaultSink(microName);
}
void Modules::AudioManager::ChangeVolume()
{
    if (!m_audioWindow)
        return;
    int volume = m_audioWindow->GetScale()->get_value();
    SetSinkVolume(volume);
}
void Modules::AudioManager::ResetVolume()
{
    SetSinkVolume(DEFAULT_SINK_VOLUME);
    UpdateScale();
}
void Modules::AudioManager::SetDefaultSink(const std::string &sinkName)
{
    if (!m_pulse->IsReady())
        return;
    pa_operation *op = pa_context_set_default_sink(m_paContext, sinkName.c_str(), nullptr, nullptr);
    if (!op)
    {
        std::cerr << CREATE_OPERATION_ERR;
        return;
    }
    while (pa_operation_get_state(op) == PA_OPERATION_RUNNING)
        pa_mainloop_iterate(m_paMainloop, DEFAULT_ML_ITERATE_BLOCK, nullptr);

    pa_operation_unref(op);
    std::cout << "Sink set to: " << sinkName << "\n";
}
void Modules::AudioManager::SetSinkVolume(int targetVolume)
{
    if (!m_pulse->IsReady())
        return;
    pa_cvolume volume;
    pa_volume_t v = static_cast<pa_volume_t>(std::round(targetVolume / 100.0 * PA_VOLUME_NORM));
    pa_cvolume_set(&volume, 2, v);
    pa_operation *op = pa_context_set_sink_volume_by_name(m_paContext, "@DEFAULT_SINK@", &volume, nullptr, nullptr);
    if (!op)
    {
        std::cerr << CREATE_OPERATION_ERR;
        return;
    }
    while (pa_operation_get_state(op) == PA_OPERATION_RUNNING)
        pa_mainloop_iterate(m_paMainloop, DEFAULT_ML_ITERATE_BLOCK, nullptr);

    pa_operation_unref(op);
    std::cout << "Sink volume set to: " << targetVolume << "\n";
}
