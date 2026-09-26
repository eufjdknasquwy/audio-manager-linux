#include "CssHelper.h"
#include "gtkmm/stylecontext.h"
#include <gtkmm.h>

namespace Gui
{
    void CssHelper::ApplyCss()
    {
        const std::string css = R"(
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
        )";

        auto cssProvider = Gtk::CssProvider::create();
        cssProvider->load_from_data(css);

        Gtk::StyleContext::add_provider_for_screen(Gdk::Screen::get_default(), cssProvider,
                                                   GTK_STYLE_PROVIDER_PRIORITY_APPLICATION // 800
        );
    }
} // namespace Gui
