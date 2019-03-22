This project is a 2 native C++ projects configured to use coroutine support and consume UWP APIs.

It produces an x86/x64 DLL named NativePlugin.dll intended to be used inside of a Unity project.

That DLL exports 2 functions. 

One uses UWP APIs to grab frames of video off the first camera that it finds on the system and then run them through face detection stopping when it finds one/more faces and returning the number of faces found.

The other uses a very simple call into an ApplicationData API to write a bit of text into a file in the app's local folder.

It's not really meant to be useful code, it's just an example that uses a few UWP APIs to go along with a blog post.

When built, this project copies the DLLs and PDBs that have been built to the UnityProject\Assets\Plugins folder where appropriate metadata should set them as plugins both for the editor and for the UWP player. 

Note - that custom build step probably messes up if you build both debug and release as it'll copy too many DLLs. I've only ever really built debug for this to date.

Note also - there are 2 projects here with shared source. I originally tried to avoid this and have one DLL used both in the editor & on the device but I haven't figured that yet. They are built from identical source anyway.