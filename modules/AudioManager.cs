using Gtk;
using System;
using System.Diagnostics;
using Process = System.Diagnostics.Process;
using System.Text;
using AudioWindow = Gui.AudioWindow;

namespace Modules {
    public class AudioManager
    {
        private AudioWindow audio_window;
        private Func<int> get_volume;
        public AudioManager(AudioWindow window, Func<int> get_volume_func)
        {
            this.audio_window = window;
            this.get_volume = get_volume_func;
        }
        public void change_sink(object sender, EventArgs e)
        {
            var combo = sender as ComboBoxText;
            if (combo == null) return;

            string sink_name = combo.ActiveText;
            if (sink_name == "JBL Tune 520BT") sink_name = "bluez_output.E4_61_F4_13_DF_08.1";
            else if (sink_name == "EasyEffects") sink_name = "easyeffects_sink";
            var process = new Process
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = "bash",
                    Arguments = $"-c \"pactl set-default-sink {sink_name}\"",
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    StandardOutputEncoding = Encoding.UTF8,
                    UseShellExecute = false,
                    CreateNoWindow = true
                }
            };
            process.Start();
            process.WaitForExit();
            Console.WriteLine($"Выбраны динамики {sink_name}");
            int volume = this.get_volume();
            this.audio_window.scale.Value = volume;
        }
        public void change_volume(object sender, EventArgs e)
        {
            var scale = sender as Scale;
            if (scale == null) return;

            int volume = (int)scale.Value;
            var process = new Process
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = "bash",
                    Arguments = $"-c \"pactl set-sink-volume @DEFAULT_SINK@ {volume}%\"",
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    StandardOutputEncoding = Encoding.UTF8,
                    UseShellExecute = false,
                    CreateNoWindow = true
                }
            };
            process.Start();
            process.WaitForExit();
        }
        public void reset_volume(object sender, EventArgs e)
        {
            int default_volume = 50;
            var process = new Process
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = "bash",
                    Arguments = $"-c \"pactl set-sink-volume @DEFAULT_SINK@ {default_volume}%\"",
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    StandardOutputEncoding = Encoding.UTF8,
                    UseShellExecute = false,
                    CreateNoWindow = true
                }
            };
            process.Start();
            process.WaitForExit();
        }
    }
}