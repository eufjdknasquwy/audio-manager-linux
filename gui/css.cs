using Gtk;
using Gdk;

namespace Gui {
    public static class CssHelper
    {
        public static void ApplyCss()
        {
            string css = @"
                window {

                }

                button {
                    border-radius: 20px;
                    padding: 5px 10px;
                }

                button:hover {

                }

                button:active {
                    background-color: #89b4fa;
                }

                label {

                }

                combobox {

                }

                entry {
                    background-color: #313244;
                    color: #cdd6f4;
                    border-radius: 12px;
                    padding: 4px;
                }
            ";

            var cssProvider = new CssProvider();
            cssProvider.LoadFromData(css);
            StyleContext.AddProviderForScreen(
                Screen.Default,
                cssProvider,
                800  // STYLE_PROVIDER_PRIORITY_APPLICATION
            );
        }
    }
}