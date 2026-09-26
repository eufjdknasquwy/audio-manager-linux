#include "modules/MicrophoneManager.h"
#include "gui/MicrophoneWindow.h"
#include "modules/PulseClient.h"
#include <cmath>
#include <iostream>
#include <pulse/pulseaudio.h>
#include <string>
namespace Gui
{
    class MicrophoneWindow;
}
Modules::MicrophoneManager::MicrophoneManager(Gui::MicrophoneWindow *window)
{
    m_microphoneWindow = window;
    m_pulse = std::make_unique<PulseClient>();
    m_paMainloop = m_pulse->GetMainloop();
    m_paContext = m_pulse->GetContext();
}
Modules::MicrophoneManager::~MicrophoneManager() = default;
void Modules::MicrophoneManager::UpdateScale()
{
    if (!m_microphoneWindow)
        return;
    int volume = m_pulse->GetDefaultSourceVolume();
    m_microphoneWindow->m_scaleConn.block();
    m_microphoneWindow->GetScale()->set_value(volume);
    m_microphoneWindow->m_scaleConn.unblock();
}
void Modules::MicrophoneManager::ChangeSource()
{
    Gtk::ComboBoxText *combo = m_microphoneWindow->GetCombo();
    std::string microName = combo->get_active_text();
    if (microName == "JBL Tune 520BT")
        microName = "bluez_input.E4:61:F4:13:DF:08";
    else if (microName == "EasyEffects")
        microName = "easyeffects_source";

    SetDefaultSource(microName);
    UpdateScale();
}
void Modules::MicrophoneManager::ChangeVolume()
{
    if (!m_microphoneWindow)
        return;
    int volume = m_microphoneWindow->GetScale()->get_value();
    SetSourceVolume(volume);
}
void Modules::MicrophoneManager::EntryVolume()
{
    if (!m_microphoneWindow)
        return;
    std::string input = m_microphoneWindow->GetEntry()->get_text();
    m_microphoneWindow->GetEntry()->set_text("");
    if (input.ends_with('%'))
    {
        std::string percentVolumeString = input;
        percentVolumeString.pop_back();
        try
        {
            if (!percentVolumeString.empty())
            {
                int percentVolume = std::stoi(percentVolumeString);
                SetSourceVolume(percentVolume);
            }
        }
        catch (...)
        {
            std::cerr << STRING_TO_INT_ERR;
            return;
        }
    }
    else
    {
        try
        {
            if (!input.empty())
                SetSourceVolume(std::stoi(input));
        }
        catch (...)
        {
            std::cerr << STRING_TO_INT_ERR;
            return;
        }
    }
    UpdateScale();
}
void Modules::MicrophoneManager::EntryVolumeTelegram()
{
    if (!m_microphoneWindow)
        return;
    std::string input = m_microphoneWindow->GetEntryTelegram()->get_text();
    m_microphoneWindow->GetEntryTelegram()->set_text("");
    int percentVolume = DEFAULT_SOURCE_VOLUME;
    if (input.ends_with('%'))
    {
        std::string percentVolumeString = input;
        percentVolumeString.pop_back();
        try
        {
            if (!percentVolumeString.empty())
                percentVolume = std::stoi(percentVolumeString);
        }
        catch (...)
        {
            std::cerr << STRING_TO_INT_ERR;
            return;
        }
    }
    else
    {
        try
        {
            if (!input.empty())
                percentVolume = std::stoi(input);
        }
        catch (...)
        {
            std::cerr << STRING_TO_INT_ERR;
            return;
        }
    }
    pa_cvolume volume;
    pa_volume_t v = static_cast<pa_volume_t>(std::round(percentVolume / 100.0 * PA_VOLUME_NORM));
    pa_cvolume_set(&volume, 2, v);

    auto tgId = GetTelegramSourceOutputId();
    if (tgId < 0)
    {
        std::cerr << "Telegram source-output not found\n";
        return;
    }

    pa_operation *op =
        pa_context_set_source_output_volume(m_paContext, static_cast<uint32_t>(tgId), &volume, nullptr, nullptr);
    if (!op)
    {
        std::cerr << CREATE_OPERATION_ERR;
        return;
    }
    while (pa_operation_get_state(op) == PA_OPERATION_RUNNING)
        pa_mainloop_iterate(m_paMainloop, 0, nullptr);

    pa_operation_unref(op);
    UpdateScale();
}
void Modules::MicrophoneManager::ResetVolume()
{
    if (!m_microphoneWindow)
        return;
    SetSourceVolume(DEFAULT_SOURCE_VOLUME);
    UpdateScale();
}
void Modules::MicrophoneManager::SetDefaultSource(const std::string &sourceName)
{
    pa_operation *op = pa_context_set_default_source(m_paContext, sourceName.c_str(), nullptr, nullptr);
    if (!op)
    {
        std::cerr << CREATE_OPERATION_ERR;
        return;
    }
    while (pa_operation_get_state(op) == PA_OPERATION_RUNNING)
        pa_mainloop_iterate(m_paMainloop, 0, nullptr);

    pa_operation_unref(op);
    std::cout << "Source set to: " << sourceName << "\n";
}
void Modules::MicrophoneManager::SetSourceVolume(int targetVolume)
{
    pa_cvolume volume;
    pa_volume_t v = static_cast<pa_volume_t>(std::round(targetVolume / 100.0 * PA_VOLUME_NORM));
    pa_cvolume_set(&volume, 2, v);
    pa_operation *op = pa_context_set_source_volume_by_name(m_paContext, "@DEFAULT_SOURCE@", &volume, nullptr, nullptr);
    if (!op)
    {
        std::cerr << CREATE_OPERATION_ERR;
        return;
    }
    while (pa_operation_get_state(op) == PA_OPERATION_RUNNING)
        pa_mainloop_iterate(m_paMainloop, 0, nullptr);

    pa_operation_unref(op);
    std::cout << "Source volume set to: " << targetVolume << "\n";
}
int Modules::MicrophoneManager::GetTelegramSourceOutputId()
{
    SourceOutputData data;
    pa_operation *op = pa_context_get_source_output_info_list(m_paContext, SourceOutputListCallback, &data);

    if (!op)
        return -1;
    while (!data.done)
    {
        pa_mainloop_iterate(m_paMainloop, 0, nullptr);
    }
    pa_operation_unref(op);
    return data.found ? static_cast<int>(data.firstId) : -1;
}
void Modules::MicrophoneManager::SourceOutputListCallback(pa_context *, const pa_source_output_info *info, int eol,
                                                          void *userdata)
{
    auto *data = static_cast<SourceOutputData *>(userdata);
    if (eol)
    {
        data->done = true;
        return;
    }
    if (!info)
        return;
    if (info->sample_spec.format == PA_SAMPLE_S16LE && !data->found)
    {
        data->firstId = info->index;
        data->found = true;
    }
}
