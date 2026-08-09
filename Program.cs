using Gtk;
using System;
using AudioWindow = Gui.AudioWindow;

Application.Init();

var audiowindow = new AudioWindow();
audiowindow.show();
Application.Run();
Environment.Exit(0);