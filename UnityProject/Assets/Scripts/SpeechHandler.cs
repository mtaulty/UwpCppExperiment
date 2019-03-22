using System.Runtime.InteropServices;
using System.Threading.Tasks;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.Windows.Speech;

public class SpeechHandler : MonoBehaviour
{
    public Text textField;

    // Note - when running Unity editor inside of a packaged identity, I find that DLL resolution
    // can break here. Strangely, I find that if I add/remove the ".dll" extension below and run
    // the script again, that seems to resolve it and I go from "DllNotFound" exceptions to working
    // code. I can't explain why at this point, I need to figure that out.
    [DllImport("NativePlugin.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.StdCall)]
    static extern int FindFaceInDefaultCamera();
    [DllImport("NativePlugin.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.StdCall)]
    static extern void WriteStringToLocalAppDataFile(string fileName, string dataContents);

    // Start is called before the first frame update
    void Start()
    {
        this.recognizer = new KeywordRecognizer(new string[] { "scan", "file", "reset" } );
        this.recognizer.OnPhraseRecognized += OnKeywordRecognised;
        this.recognizer.Start();
    }
    async void OnKeywordRecognised(PhraseRecognizedEventArgs args)
    {
        if ((args.confidence == ConfidenceLevel.High) ||
            (args.confidence == ConfidenceLevel.Medium))
        {
            if (args.text == "scan")
            {
                this.textField.text = "searching for faces";

                int faceCount = 0;

                await Task.Run(
                    () =>
                    {
                    // This blocks, doesn't return until it has found a face. Could add a timeout!
                    faceCount = FindFaceInDefaultCamera();
                    }
                  );
                this.textField.text = $"{faceCount} faces found";
            }
            else if (args.text == "file")
            {
                bool written = false;

                this.textField.text = "writing file";

                try
                {
                    await Task.Run(
                        () =>
                        {
                            WriteStringToLocalAppDataFile("test.txt", "test content");
                            written = true;
                        }
                    );
                }
                catch
                {

                }
                this.textField.text = written ? "written file" : "failed to write";
            }
            else
            {
                this.textField.text = "nothing happening";
            }
        }
    }
    KeywordRecognizer recognizer;
}
