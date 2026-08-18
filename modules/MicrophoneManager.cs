#nullable disable
using Gtk;
using Process = System.Diagnostics.Process;
using System.Text;
using System.Diagnostics;
using MicrophoneWindow = Gui.MicrophoneWindow;

namespace Modules {
    class MicrophoneManager
    {
        private MicrophoneWindow microphone_window;
        private Func<int> get_volume;
        public MicrophoneManager(MicrophoneWindow window, Func<int> get_volume_func)
        {
            this.microphone_window = window;
            this.get_volume = get_volume_func;
        }
        public void update_scale()
        {
            int volume = this.get_volume();
            this.microphone_window.scale.Value = volume;
        }
        public void change_source(object sender, EventArgs e)
        {
            var combo = sender as ComboBoxText;
            if (combo == null) return;

            string micro_name = combo.ActiveText;
            if (micro_name == "JBL Tune 520BT") micro_name = "bluez_input.E4:61:F4:13:DF:08";
            else if (micro_name == "EasyEffects") micro_name = "easyeffects_source";
            var process = new Process
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = "bash",
                    Arguments = $"-c \"pactl set-default-source {micro_name}\"",
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    StandardOutputEncoding = Encoding.UTF8,
                    UseShellExecute = false,
                    CreateNoWindow = true
                }
            };
            process.Start();
            process.WaitForExit();
            Console.WriteLine($"Выбран микрофон {micro_name}");
            this.update_scale();
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
                    Arguments = $"-c \"pactl set-source-volume @DEFAULT_SOURCE@ {volume}%\"",
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
        public void entry_volume(object sender, EventArgs e)
        {
            var entry = sender as Entry;
            if (entry == null) return;

            string input = entry.Text.Trim();
            entry.Text = "";
            if (! input.EndsWith('%')) input = input + '%';
            var process = new Process
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = "bash",
                    Arguments = $"-c \"pactl set-source-volume @DEFAULT_SOURCE@ {input}\"",
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    StandardOutputEncoding = Encoding.UTF8,
                    UseShellExecute = false,
                    CreateNoWindow = true
                }
            };
            process.Start();
            process.WaitForExit();
            this.update_scale();
        }
        public void entry_volume_telegram(object sender, EventArgs e)
        {
            var entry_tg = sender as Entry;
            if (entry_tg == null) return;

            var process = new Process
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = "bash",
                    Arguments = $"-c \"pactl list source-outputs short | grep s16le | awk '{{print $1}}'\"",
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    StandardOutputEncoding = Encoding.UTF8,
                    UseShellExecute = false,
                    CreateNoWindow = true
                }
            };
            process.Start();
            string output = process.StandardOutput.ReadToEnd();
            process.WaitForExit();
            string telegram_id = output.Trim();
            string input = entry_tg.Text.Trim();
            entry_tg.Text = "";
            if (! input.EndsWith('%')) input = input + '%';
            var process2 = new Process
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = "bash",
                    Arguments = $"-c \"pactl set-source-output-volume {telegram_id} {input}\"",
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    StandardOutputEncoding = Encoding.UTF8,
                    UseShellExecute = false,
                    CreateNoWindow = true
                }
            };
            process2.Start();
            process2.WaitForExit();
            this.update_scale();
        }
        public void reset_volume(object sender, EventArgs e)
        {
            int default_volume = 100;
            var process = new Process
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = "bash",
                    Arguments = $"-c \"pactl set-source-volume @DEFAULT_SOURCE@ {default_volume}%\"",
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    StandardOutputEncoding = Encoding.UTF8,
                    UseShellExecute = false,
                    CreateNoWindow = true
                }
            };
            process.Start();
            process.WaitForExit();
            this.update_scale();
        }
    }
}