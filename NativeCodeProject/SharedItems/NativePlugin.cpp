// NativePlugin.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Media::Capture;
using namespace Windows::Media::Capture::Frames;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Media::FaceAnalysis;
using namespace Windows::Storage;

// Sorry, this shouldn't really be one massive function...
IAsyncOperation<int> InternalFindFaceInDefaultCameraAsync()
{
	auto facesFound(0);

	auto devices = co_await DeviceInformation::FindAllAsync(DeviceClass::VideoCapture);

	if (devices.Size())
	{
		DeviceInformation deviceInfo(nullptr);

		// We could do better here around choosing a device, we just take
		// the front one or the first one.
		for (auto const& device : devices)
		{
			if ((device.EnclosureLocation().Panel() == Panel::Front))
			{
				deviceInfo = device;
				break;
			}
		}
		if ((deviceInfo == nullptr) && devices.Size())
		{
			deviceInfo = *devices.First();
		}
		if (deviceInfo != nullptr)
		{
			MediaCaptureInitializationSettings initSettings;
			initSettings.StreamingCaptureMode(StreamingCaptureMode::Video);
			initSettings.VideoDeviceId(deviceInfo.Id());
			initSettings.MemoryPreference(MediaCaptureMemoryPreference::Cpu);

			MediaCapture capture;
			co_await capture.InitializeAsync(initSettings);

			auto faceDetector = co_await FaceDetector::CreateAsync();
			auto faceDetectorFormat = FaceDetector::GetSupportedBitmapPixelFormats().GetAt(0);

			// We could do better here, we will just take the first frame source and
			// we assume that there will be at least one. 
			auto frameSource = (*capture.FrameSources().First()).Value();
			auto frameReader = co_await capture.CreateFrameReaderAsync(frameSource);

			winrt::slim_mutex mutex;

			handle signal{ CreateEvent(nullptr, true, false, nullptr) };
			auto realSignal = signal.get();

			frameReader.FrameArrived(
				[&mutex, faceDetector, &facesFound, faceDetectorFormat, realSignal]
			(IMediaFrameReader reader, MediaFrameArrivedEventArgs args) -> IAsyncAction
			{
				// Not sure I need this?
				if (mutex.try_lock())
				{
					auto frame = reader.TryAcquireLatestFrame();

					if (frame != nullptr)
					{
						auto bitmap = frame.VideoMediaFrame().SoftwareBitmap();

						if (bitmap != nullptr)
						{
							if (!FaceDetector::IsBitmapPixelFormatSupported(bitmap.BitmapPixelFormat()))
							{
								bitmap = SoftwareBitmap::Convert(bitmap, faceDetectorFormat);
							}
							auto faceResults = co_await faceDetector.DetectFacesAsync(bitmap);

							if (faceResults.Size())
							{
								// We are done, we found a face.
								facesFound = faceResults.Size();
								SetEvent(realSignal);
							}
						}
					}
					mutex.unlock();
				}
			}
			);
			co_await frameReader.StartAsync();

			co_await resume_on_signal(signal.get());

			// Q - do I need to remove the event handler or will the destructor do the
			// right thing for me?
			co_await frameReader.StopAsync();
		}
	}
	return(facesFound);
}
IAsyncAction InternalWriteStringToLocalAppDataFileAsync(wchar_t* pszFileName, wchar_t* pszData)
{
	hstring fileName(pszFileName);
	hstring contents(pszData);

	auto file = co_await ApplicationData::Current().LocalFolder().CreateFileAsync(fileName, CreationCollisionOption::ReplaceExisting);

	co_await FileIO::WriteTextAsync(file, contents);
}
extern "C"
{
	__declspec(dllexport) int FindFaceInDefaultCamera()
	{
		int faceCount = InternalFindFaceInDefaultCameraAsync().get();

		return(faceCount);
	}
	__declspec(dllexport) void WriteStringToLocalAppDataFile(wchar_t* pszFileName, wchar_t* pszData)
	{
		InternalWriteStringToLocalAppDataFileAsync(pszFileName, pszData).get();
	}
}