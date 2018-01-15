# d3d-win32-simple-window
A simple native Win32 window in C++ ready for D3D and input. Could be converted to OO very easily, but a namespace is more than enough for a single-window application.

This window is ready for rendering Direct3D and has a framework setup for accepting keyboard and mouse input.

Usage:

```
Window::init(L"Window Name", 1280, 720);

// TODO: init

Window::Show();

// TODO: magic

Window::Hide();

// TODO: cleanup

Window::End();
```
