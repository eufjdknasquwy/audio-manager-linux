#include "gui/AudioWindow.h"
#include "modules/Parser.h"
#include <gtkmm.h>
int main(int argc, char *argv[])
{
    Modules::Parser::Parse(argc, argv);
    auto app = Gtk::Application::create(argc, argv, "org.example.audio-manager");

    app->hold();

    Gui::AudioWindow audioWindow;
    audioWindow.Show();

    return app->run(audioWindow.GetWindow());
}
