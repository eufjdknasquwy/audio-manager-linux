#nullable disable
using Gtk;
using GLib;
using System;
using System.Diagnostics;
using Process = System.Diagnostics.Process;
using System.Text;
using System.Collections.Generic;
using MicrophoneWindow = Gui.MicrophoneWindow;
using CssHelper = Gui.CssHelper;
using AudioManager = Modules.AudioManager;
using ArgsParser = Modules.ArgsParser;
using Tray = Modules.Tray;

namespace Gui {
    public class GetAudio
    {
        public static List<string> get_audio_devs()
        {
            var process = new Process
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = "bash",
                    Arguments = "-c \"pactl list short sinks | grep -v '.monitor' | awk '{print $2}'\"",
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

            List<string> available_sinks = new List<string>();
            foreach (string line in output.Split('\n'))
            {
                if (!string.IsNullOrEmpty(line))
                    available_sinks.Add(line.Trim());
            }
            return available_sinks;
        }
        public static int get_volume()
        {
            var process = new Process
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = "bash",
                    Arguments = "-c \"pactl get-sink-volume @DEFAULT_SINK@ | awk '{print $5}'\"",
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
        public GetAudio()
        {
            var available_sinks = get_audio_devs();
            int volume = get_volume();
        }
    }
    public class AudioWindow
    {
        private AudioManager audio_manager;
        private Tray tray;
        private MicrophoneWindow microphone_window;
        private Gtk.Dialog dialog;
        private Gtk.Box content;
        private Gtk.Notebook notebook = null!;
        private Gtk.Box audio_page = null!;
        private Gtk.Box micro_page = null!;
        private Gtk.Label label = null!;
        private Gtk.ComboBoxText combo = null!;
        private Gtk.Label separator = null!;
        public Gtk.Scale scale = null!;
        private Gtk.Button reset_button;
        public AudioWindow()
        {
            this.audio_manager = new AudioManager(this, GetAudio.get_volume);
            this.tray = new Tray(this.dialog, this);
            this.dialog = new Gtk.Dialog("Выбор динамиков", null, 0);
            this.dialog.SetPosition(Gtk.WindowPosition.Center);
            this.dialog.SetDefaultSize(300, 280);
            this.content = this.dialog.ContentArea;

            this.dialog.DeleteEvent += this.tray.OnDeleteEvent;
            this.dialog.AddButton("Свернуть", ResponseType.Ok);
            this.dialog.AddButton("Закрыть", ResponseType.Cancel);
            this.dialog.Response += this.tray.OnDialogResponse;

            this.create_pages();
            this.microphone_window = new MicrophoneWindow(this, this.set_margin_all, this.micro_page);
            this.create_label();
            this.create_scale();
            this.create_combo();
            this.create_separator();
            this.create_reset_volume();
            CssHelper.ApplyCss();
            this.tray.CreateTray(this.dialog);

            GLib.Timeout.Add(500, refresh_volume);
            GLib.Timeout.Add(1800000, refresh_devices);
        }
        public bool refresh_volume()
        {
            if (this.dialog.Visible)
            {
                int volume = GetAudio.get_volume();
                this.scale.Value = volume;
                this.microphone_window.refresh_volume();
            }
            return true;
        }
        public bool refresh_devices()
        {
            this.combo.RemoveAll();
            string display_name = "";
            List<string> available_sinks = GetAudio.get_audio_devs();
            foreach (string sinks in available_sinks)
            {
                switch (sinks)
                {
                    case "bluez_output.E4_61_F4_13_DF_08.1":
                        display_name = "JBL Tune 520BT";
                        break;
                    case "easyeffects_sink":
                        display_name = "EasyEffects";
                        break;
                    default:
                        display_name = sinks;
                        break;
                }
                this.combo.AppendText(display_name);
            }
            this.combo.Active = -1;
            this.microphone_window.refresh_devices();
            return true;
        }
        public void set_margin_all(Gtk.Widget widget, int size)
        {
            widget.MarginTop = size;
            widget.MarginBottom = size;
            widget.MarginStart = size;
            widget.MarginEnd = size;
        }
        public void create_reset_volume()
        {
            this.reset_button = new Gtk.Button("Сбросить громкость");
            this.reset_button.Clicked += this.audio_manager.reset_volume;
            this.audio_page.PackStart(this.reset_button, false, false, 0);
        }
        public void create_pages()
        {
            this.notebook = new Gtk.Notebook();
            this.content.Add(this.notebook);
            this.audio_page = new Gtk.Box(Gtk.Orientation.Vertical, 10);
            this.micro_page = new Gtk.Box(Gtk.Orientation.Vertical, 10);
            this.set_margin_all(this.audio_page, 20);
            this.set_margin_all(this.micro_page, 20);
            this.notebook.AppendPage(this.audio_page, new Gtk.Label("Динамики"));
            this.notebook.AppendPage(this.micro_page, new Gtk.Label("Микрофоны"));

            if (ArgsParser.Input)
                this.notebook.CurrentPage = 1;
            else if (ArgsParser.Output)
                this.notebook.CurrentPage = 0;
        }
        public void create_label()
        {
            this.label = new Gtk.Label("Выбор динамиков");
            this.label.Halign = Align.Start;
            this.audio_page.PackStart(this.label, false, false, 0);
        }
        public void create_combo()
        {
            this.combo = new Gtk.ComboBoxText();
            string display_name = "";
            List<string> available_sinks = GetAudio.get_audio_devs();
            foreach (string sinks in available_sinks)
            {
                switch (sinks)
                {
                    case "bluez_output.E4_61_F4_13_DF_08.1":
                        display_name = "JBL Tune 520BT";
                        break;
                    case "easyeffects_sink":
                        display_name = "EasyEffects";
                        break;
                    default:
                        display_name = sinks;
                        break;
                }
                this.combo.AppendText(display_name);
            }
            this.combo.Active = -1;
            this.combo.Changed += this.audio_manager.change_sink;
            this.audio_page.PackStart(this.combo, false, false, 0);
        }
        public void create_separator()
        {
            this.separator = new Gtk.Label("Громкость");
            this.separator.Halign = Align.Start;
            this.audio_page.PackStart(this.separator, false, false, 0);
            this.audio_page.PackStart(this.scale, false, false, 0);
        }
        public void create_scale()
        {
            this.scale = new Scale(Orientation.Horizontal, null);
            this.scale.SetRange(0, 100);
            int volume = GetAudio.get_volume();
            this.scale.Value = volume;
            this.scale.Digits = 0;
            this.scale.SetIncrements(1, 1);
            this.scale.ValueChanged += this.audio_manager.change_volume;
        }
        public void show()
        {
            if (!ArgsParser.Tray) this.dialog.ShowAll();
        }
    }
}