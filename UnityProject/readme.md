This project is a simple Unity project to test calling into UWP APIs directly from the editor via PInvoke calls into a DLL that then uses C++/WinRT to call UWP APIs.
There's 1 very simple scene which has 3 voice commands.

"scan" will call into the DLL to ask it to open up the camera and scan for faces and it'll scan until it sees at least 1 face then it'll stop and return the number of faces it saw on the first frame that contained a face. Not very useful functionality but I wanted to try out those APIs in C++/WinRT.

"write" will call into the DLL to ask it to open up a file in the UWP local data folder. I'd expect this to fail from the editor but not if the editor is running in the package identity of an installed UWP app and I have a suitable UWP app in this project which performs that function - see the readme for the DummyDesktopBridgeProject.

"reset" will simply change the text on the screen back to its original value - sometimes useful when it's unclear what's going on.

Have a look at the script named "SpeechHandler.cs" for the "functionality", there's not much to it, it just does a couple of PInvokes.

Note that I have included the DLLs in the repo but they are built by the NativeCodeProject and copied into the "right" place as a post-build step (post-link).

Note also that I have somewhat assumed that only the x64 DLL matters for the editor and only the x86 DLL matters for the device (HoloLens in my case).

Note also that I've only ever built things debug, release configurations are probably broken.

Note also that sometimes when I run Unity within the packaged identity of another app, I find that it will fail to resolve my DLL named NativePlugin.dll. Bizarrely, I seem to be able to fix that by;

* Editing the script to take away the .dll extension on the PInvoke attributes.
* Running the script (it'll fail again)
* Editing the script to put back the .dll extension on the PInvoke attributes.

I know that's random and I need to look at it but it worked for me.