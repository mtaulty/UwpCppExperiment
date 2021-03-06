Within this project there is an application called WpfDummyApp. 

This project does nothing - it is an empty project which builds out a blank WPF application which displays a blank window.

There's another project in here called WpfDummyAppBridgePackaging. This project simply packages the first project as a desktop bridge project (i.e. for UWP-like install) and requests full trust for it along with broad file system access (both via the manifest).

It's the WpfDummyAppBridgePackaging project that's of interest here, not the other project. So, if you ever wanted to do a build/run then choose the packaging project but there's little point really in running the app as it doesn't do anything.

The intention is more that you would build a Store package for the packaging project and then install that package onto your machine.

That then gives a well-known (and dummy) package identity that can be used to run some other desktop app.

To do the installation, you'd need to add the certificate for this app to your trusted certificates. You can do that manually or you can do it via the powershell script which is generated by Visual Studio when you make an app package.

Note that the display name is "WpfDummyAppBridgePackaging" and the package name is "1dd298fc-e736-4f7d-8bf3-427472e00a93" and the package family name is "1dd298fc-e736-4f7d-8bf3-427472e00a93_54fjpjf3c8m40".

Once you've built and installed the app, you should find its (data) folders at your equivalent of "C:\Users\mtaulty\AppData\Local\Packages>cd 1dd298fc-e736-4f7d-8bf3-427472e00a93_54fjpjf3c8m40".

Once you have this application installed the intention is that you can use the PowerShell command to invoke another application (Unity was the one that I was interested in at the time of writing) within the identity of this application.

I do this by doing something like;

"Invoke-CommandInDesktopPackage -PackageFamilyName "1dd298fc-e736-4f7d-8bf3-427472e00a93_54fjpjf3c8m40" -AppId WpfDummyAppBridgePackaging -Command "C:\Program Files\Unity\Hub\Editor\2018.3.9f1\Editor\Unity.exe""