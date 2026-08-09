#nullable disable
using System;
using System.Diagnostics;
using Gtk;
using Process = System.Diagnostics.Process;

namespace Modules
{
    public class Tray
    {
        private Window main_window;
        private Window dialog;
        private StatusIcon indicator;
        private Menu menu;

        private MenuItem restart_ef_item;
        private MenuItem open_app;
        private MenuItem hide_app;
        private MenuItem quit_app;

        public Tray(Window window)
        {
            this.main_window = window;
            InitializeMenuItems();
            CreateMenu();
        }

        private MenuItem CreateMenuItem(string label, string iconName, EventHandler callback)
        {
            var box = new Box(Orientation.Horizontal, 10);
            var icon = Image.NewFromIconName(iconName, IconSize.Menu);
            box.PackStart(icon, false, false, 0);
            var text = new Label(label);
            box.PackStart(text, false, false, 0);
            var item = new MenuItem();
            item.Add(box);
            item.Activated += callback;
            return item;
        }

        private void InitializeMenuItems()
        {
            restart_ef_item = CreateMenuItem(
                "Перезагрузить EasyEffects",
                "preferences-desktop-multimedia",
                (sender, e) => RestartEf()
            );
            open_app = CreateMenuItem(
                "Открыть",
                "window-new",
                (sender, e) => OpenWindow()
            );
            hide_app = CreateMenuItem(
                "Скрыть",
                "list-remove",
                (sender, e) => HideWindow()
            );
            quit_app = CreateMenuItem(
                "Выйти",
                "application-exit",
                (sender, e) => Quit()
            );
        }

        private void CreateMenu()
        {
            menu = new Menu();
            menu.Append(restart_ef_item);
            menu.Append(open_app);
            menu.Append(hide_app);
            menu.Append(new SeparatorMenuItem());
            menu.Append(quit_app);
            menu.ShowAll();
        }

        public void CreateTray(Window dialogWindow)
        {
            this.dialog = dialogWindow;

            indicator = new StatusIcon();
            indicator.Pixbuf = IconTheme.Default.LoadIcon("audio-volume-medium", 64, IconLookupFlags.GenericFallback);
            indicator.Visible = true;
            indicator.TooltipText = "Audio Manager";

            // 👇 ЛКМ — переключает окно (открыть/свернуть)
            indicator.ButtonPressEvent += (sender, args) =>
            {
                if (args.Event.Button == 1)
                    ToggleWindow();
            };

            // ПКМ — показывает меню
            indicator.PopupMenu += (sender, args) =>
            {
                menu.Popup();
            };
        }

        // 👇 ПЕРЕКЛЮЧЕНИЕ ОКНА
        public void ToggleWindow()
        {
            if (dialog.Visible)
                dialog.Hide();
            else
            {
                dialog.ShowAll();
                dialog.Present();
            }
        }

        public void OpenWindow()
        {
            dialog.ShowAll();
            dialog.Present();
        }

        public void HideWindow()
        {
            dialog.Hide();
        }

        public void RestartEf()
        {
            Process.Start("/home/yegor/.restart-easyeffects.sh");
        }

        public void Quit()
        {
            Application.Quit();
        }

        public void OnDeleteEvent(object sender, DeleteEventArgs args)
        {
            dialog.Hide();
            args.RetVal = true;
        }

        public void OnDialogResponse(object sender, ResponseArgs args)
        {
            if (args.ResponseId == ResponseType.Ok)
                dialog.Hide();
            else if (args.ResponseId == ResponseType.Cancel)
                Application.Quit();
        }
    }
}