using Gtk;
using System.Diagnostics;
using Process = System.Diagnostics.Process;
using System.Text;
using MicrophoneManager = Modules.MicrophoneManager;

namespace Gui {
    class GetMicrophones
    {
        public static List<string> get_micro_devs()
        {
            var process = new Process
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = "bash",
                    Arguments = "-c \"pactl list short sources | grep -v '.monitor' | awk '{print $2}'\"",
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

            List<string> available_microphones = new List<string>();
            foreach (string line in output.Split('\n'))
            {
                if (!string.IsNullOrEmpty(line))
                    available_microphones.Add(line.Trim());
            }
            return available_microphones;
        }
        public static int get_volume()
        {
            var process = new Process
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = "bash",
                    Arguments = "-c \"pactl get-source-volume @DEFAULT_SOURCE@ | awk '{print $5}'\"",
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
            string volStr = output.Trim().Replace("%", "");
            if (int.TryParse(volStr, out int volume))
                return volume;
            else
                return 50;
        }
        public GetMicrophones()
        {
            var available_microphones = get_micro_devs();
            int volume = get_volume();
        }
    }
    public class MicrophoneWindow
    {
        private MicrophoneManager microphone_manager;
        private AudioWindow audio_window;
        private Action<Gtk.Widget, int> set_margin_all;
        private Gtk.Box micro_page;
        private Gtk.Label label = null!;
        private Gtk.ComboBoxText combo = null!;
        private Gtk.Label separator = null!;
        public Gtk.Scale scale = null!;
        private Gtk.Entry entry_volume = null!;
        private Gtk.Entry entry_volume_telegram = null!;
        public MicrophoneWindow(AudioWindow window, Action<Gtk.Widget, int> set_margin_all, Gtk.Box micro_page)
        {
            this.microphone_manager = new MicrophoneManager(this, GetMicrophones.get_volume);
            this.audio_window = window;
            this.set_margin_all = set_margin_all;
            this.micro_page = micro_page;

            this.create_label();
            this.create_scale();
            this.create_combo();
            this.create_separator();
            this.create_entry_volume();
            this.create_entry_volume_telegram();
        }
        public void create_label()
        {
            this.label = new Gtk.Label("Выбор микрофона");
            this.label.Halign = Align.Start;
            this.micro_page.PackStart(this.label, false, false, 0);
        }
        public void create_combo()
        {
            this.combo = new Gtk.ComboBoxText();
            string display_name = "";
            List<string> available_sinks = GetMicrophones.get_micro_devs();
            foreach (string micro in available_sinks)
            {
                if (micro == "bluez_input.E4:61:F4:13:DF:08") display_name = "JBL Tune 520BT";
                else if (micro == "easyeffects_source") display_name = "EasyEffects";
                else display_name = micro;
                this.combo.AppendText(display_name);
            }
            this.combo.Active = 0;
            this.combo.Changed += this.microphone_manager.change_source;
            this.micro_page.PackStart(this.combo, false, false, 0);
        }
        public void create_separator()
        {
            this.separator = new Gtk.Label("Громкость");
            this.separator.Halign = Align.Start;
            this.micro_page.PackStart(this.separator, false, false, 0);
            this.micro_page.PackStart(this.scale, false, false, 0);
        }
        public void create_scale()
        {
            this.scale = new Scale(Orientation.Horizontal, null);
            this.scale.SetRange(0, 300);
            int volume = GetMicrophones.get_volume();
            this.scale.Value = volume;
            this.scale.Digits = 0;
            this.scale.SetIncrements(1, 1);
            this.scale.ValueChanged += this.microphone_manager.change_volume;
        }
        public void create_entry_volume()
        {
            this.entry_volume = new Gtk.Entry();
            this.entry_volume.PlaceholderText = "Громкость микрофона";
            this.entry_volume.Activated += this.microphone_manager.entry_volume;
            this.micro_page.PackStart(this.entry_volume, false, false, 0);
        }
        public void create_entry_volume_telegram()
        {
            this.entry_volume_telegram = new Gtk.Entry();
            this.entry_volume_telegram.PlaceholderText = "Громкость микрофона (тг)";
            this.entry_volume_telegram.Activated += this.microphone_manager.entry_volume_telegram;
            this.micro_page.PackStart(this.entry_volume_telegram, false, false, 0);
        }
    }
}