/****************************************************************************
While the underlying libraries are covered by LGPL, this sample is released 
as public domain.  It is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE.  
*****************************************************************************/

// This code is just to show a couple of ways you could use the 
// GenericSampleSourceFilter.  For a more details discussion, check out the
// readme.txt

// Note that in order to use the MP3 methods, you must download and
/// install Bass.Net (which must be added as a reference) and BASS.DLL
/// from http://www.un4seen.com
//#define USING_BASS_DLL

using System;
using System.Diagnostics;
using System.Drawing.Imaging;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.IO;
using DirectShowLib;
using GenericSampleSourceFilterClasses;
using WindowsMediaLib;
using IConfigAsfWriter = WindowsMediaLib.IConfigAsfWriter;
using AMMediaType = WindowsMediaLib.Defs.AMMediaType;

#if USING_BASS_DLL
using Un4seen.Bass;
#endif

namespace MiCoSi
{
    /// <summary>
    /// A class to construct a graph using the GenericSampleSourceFilter.
    /// </summary>
    internal class DxPlay : IDisposable
    {
        #region Member variables

        // Event called when the graph stops
        public bool IsCompleted = false;

        /// <summary>
        /// The class that retrieves the images
        /// </summary>
        private ImageHandler m_ImageHandler;

        /// <summary>
        /// graph builder interfaces
        /// </summary>
        private IFilterGraph2 m_FilterGraph;

        /// <summary>
        /// Another graph builder interface
        /// </summary>
        private IMediaControl m_mediaCtrl;

        private int _inFrameWidth;
        private int _inFrameHeight;
        private int _FPS;

#if DEBUG
        /// <summary>
        /// Allow you to "Connect to remote graph" from GraphEdit
        /// </summary>
        DsROTEntry m_DsRot;
#endif

        #endregion

        /// <summary>
        /// Release everything
        /// </summary>
        public void Dispose()
        {
            GC.SuppressFinalize(this);
            CloseInterfaces();
        }

        ~DxPlay()
        {
            CloseInterfaces();
        }

        public DxPlay(ImageHandler handler,
                      string fileName,
                      CaptureOptions.VideoCompression quality,
                      int inFrameWidth, int inFrameHeight, int FPS)
        {
            if (handler == null)
            {
                throw new ArgumentNullException("handler");
            }

            try
            {
                _inFrameWidth = inFrameWidth;
                _inFrameHeight = inFrameHeight;
                _FPS = FPS;
                m_ImageHandler = handler;

                // Set up the graph
                SetupGraph(fileName, quality);
            }
            catch
            {
                Dispose();
                throw;
            }
        }

        /// <summary>
        /// Start playing
        /// </summary>
        public void Start()
        {
            // Create a new thread to process events
            new Thread(EventWait) { Name = "Media Event Thread" }.Start();

            int hr = m_mediaCtrl.Run();
            DsError.ThrowExceptionForHR(hr);
        }

        /// <summary>
        /// Stop the capture graph.
        /// </summary>
        public void Stop()
        {
            int hr;

            hr = ((IMediaEventSink)m_FilterGraph).Notify(EventCode.UserAbort, IntPtr.Zero, IntPtr.Zero);
            DsError.ThrowExceptionForHR(hr);

            hr = m_mediaCtrl.Stop();
            DsError.ThrowExceptionForHR(hr);
        }


        /// <summary>
        /// Build the filter graph
        /// </summary>
        private void SetupGraph(string fileName, CaptureOptions.VideoCompression quality)
        {
            int hr;

            // Get the graphbuilder object
            m_FilterGraph = (IFilterGraph2)new FilterGraph();

            // Get a ICaptureGraphBuilder2 to help build the graph
            ICaptureGraphBuilder2 icgb2 = (ICaptureGraphBuilder2)new CaptureGraphBuilder2();

            try
            {
                // Link the ICaptureGraphBuilder2 to the IFilterGraph2
                hr = icgb2.SetFiltergraph(m_FilterGraph);
                DsError.ThrowExceptionForHR(hr);

#if DEBUG
                // Allows you to view the graph with GraphEdit File/Connect
                m_DsRot = new DsROTEntry(m_FilterGraph);
#endif

                // Our data source
                IBaseFilter ipsb = (IBaseFilter)new GenericSampleSourceFilter();

                try
                {
                    // Get the pin from the filter so we can configure it
                    IPin ipin = DsFindPin.ByDirection(ipsb, PinDirection.Output, 0);

                    try
                    {
                        // Configure the pin using the provided BitmapInfo
                        ConfigurePusher((IGenericSampleConfig)ipin);
                    }
                    finally
                    {
                        Marshal.ReleaseComObject(ipin);
                    }

                    // Add the filter to the graph
                    hr = m_FilterGraph.AddFilter(ipsb, "GenericSampleSourceFilter");
                    Marshal.ThrowExceptionForHR(hr);

                    // Create a filter for the output avi file
                    var asfWriter = ConfigAsf(icgb2, fileName, quality);

                    // Connect everything together
                    hr = icgb2.RenderStream(null, null, ipsb, null, asfWriter);
                    DsError.ThrowExceptionForHR(hr);
                }
                finally
                {
                    Marshal.ReleaseComObject(ipsb);
                }

                // Grab some other interfaces
                m_mediaCtrl = m_FilterGraph as IMediaControl;
            }
            finally
            {
                Marshal.ReleaseComObject(icgb2);
            }
        }

        private IBaseFilter ConfigAsf(ICaptureGraphBuilder2 capGraph,
                                      string szOutputFileName,
                                      CaptureOptions.VideoCompression quality)
        {
            IFileSinkFilter pTmpSink = null;
            IBaseFilter asfWriter = null;

            int hr = capGraph.SetOutputFileName(MediaSubType.Asf, szOutputFileName, out asfWriter, out pTmpSink);
            Marshal.ThrowExceptionForHR(hr);

            try
            {
                IConfigAsfWriter lConfig = (IConfigAsfWriter)asfWriter;

                string profileData;
                int bitRate;
                switch (quality)
                {
                    case CaptureOptions.VideoCompression.MinimalSize:
                        bitRate = (int)(1000000.0 * (_inFrameWidth * _inFrameHeight * _FPS) / (720 * 1280 * 25));
                        break;

                    case CaptureOptions.VideoCompression.Optimal:
                        bitRate = (int)(2000000.0 * (_inFrameWidth * _inFrameHeight * _FPS) / (720 * 1280 * 25));
                        break;

                    case CaptureOptions.VideoCompression.BestQuality:
                        bitRate = (int)(4000000.0 * (_inFrameWidth * _inFrameHeight * _FPS) / (720 * 1280 * 25));
                        break;

                    default:
                        throw new ApplicationException("Internal error: unknown type of the required video codec");
                }

                //Getting XML with profile.
                using (var reader = new StreamReader(Path.Combine(Directories.GetConfigDirectory(), "Encoder.prx")))
                {
                    profileData = reader.ReadToEnd();
                }
                profileData = profileData.Replace("__FRAME_WIDTH__", _inFrameWidth.ToString());
                profileData = profileData.Replace("__FRAME_HEIGHT__", _inFrameHeight.ToString());
                profileData = profileData.Replace("__BIT_RATE__", bitRate.ToString());

                // Get manager
                IWMProfileManager profileManager;
                WMUtils.WMCreateProfileManager(out profileManager);

                // Create profile
                IWMProfile profile;
                profileManager.LoadProfileByData(profileData, out profile);
                lConfig.ConfigureFilterUsingProfile(profile);
            }
            finally
            {
                Marshal.ReleaseComObject(pTmpSink);
            }

            return asfWriter;
        }

        /// <summary>
        /// Configure the GenericSampleSourceFilter
        /// </summary>
        /// <param name="ips">The interface to the GenericSampleSourceFilter</param>
        private void ConfigurePusher(IGenericSampleConfig ips)
        {
            int hr;

            m_ImageHandler.SetMediaType(ips);

            // Specify the callback routine to call with each sample
            hr = ips.SetBitmapCB(m_ImageHandler);
            DsError.ThrowExceptionForHR(hr);
        }

        /// <summary>
        /// Shut down graph
        /// </summary>
        private void CloseInterfaces()
        {
            int hr;

            lock (this)
            {
                // Stop the graph
                if (m_mediaCtrl != null)
                {
                    // Stop the graph
                    hr = m_mediaCtrl.Stop();
                    m_mediaCtrl = null;
                }

                if (m_ImageHandler != null)
                {
                    m_ImageHandler.Dispose();
                    m_ImageHandler = null;
                }

#if DEBUG
                if (m_DsRot != null)
                {
                    m_DsRot.Dispose();
                    m_DsRot = null;
                }
#endif

                // Release the graph
                if (m_FilterGraph != null)
                {
                    hr = ((IMediaEventSink)m_FilterGraph).Notify(EventCode.UserAbort, IntPtr.Zero, IntPtr.Zero);

                    while (!IsCompleted)
                        System.Threading.Thread.Sleep(0);
                    
                    Marshal.ReleaseComObject(m_FilterGraph);
                    m_FilterGraph = null;
                }
            }
            GC.Collect();
        }

        /// <summary>
        /// Called on a new thread to process events from the graph.  The thread
        /// exits when the graph finishes.
        /// </summary>
        private void EventWait()
        {
            // Returned when GetEvent is called but there are no events
            const int E_ABORT = unchecked((int)0x80004004);

            int hr;
            IntPtr p1, p2;
            EventCode ec;
            EventCode exitCode = 0;

            IMediaEvent pEvent = (IMediaEvent)m_FilterGraph;

            do
            {
                // Read the event
                for (
                    hr = pEvent.GetEvent(out ec, out p1, out p2, 100);
                    hr >= 0;
                    hr = pEvent.GetEvent(out ec, out p1, out p2, 100)
                    )
                {
                    Debug.WriteLine(ec);
                    switch (ec)
                    {
                        // If the clip is finished playing
                        case EventCode.Complete:
                        case EventCode.ErrorAbort:
                        case EventCode.UserAbort:
                            exitCode = ec;

                            // Release any resources the message allocated
                            hr = pEvent.FreeEventParams(ec, p1, p2);
                            DsError.ThrowExceptionForHR(hr);
                            break;

                        default:
                            // Release any resources the message allocated
                            hr = pEvent.FreeEventParams(ec, p1, p2);
                            DsError.ThrowExceptionForHR(hr);
                            break;
                    }
                }

                // If the error that exited the loop wasn't due to running out of events
                if (hr != E_ABORT)
                {
                    DsError.ThrowExceptionForHR(hr);
                }
            } while (exitCode == 0);

            // Send an event saying we are complete
            IsCompleted = true;

        } // Exit the thread


        public class CompletedArgs : System.EventArgs
        {
            /// <summary>The result of the rendering</summary>
            /// <remarks>
            /// This code will be a member of DirectShowLib.EventCode.  Typically it 
            /// will be EventCode.Complete, EventCode.ErrorAbort or EventCode.UserAbort.
            /// </remarks>
            public EventCode Result;

            /// <summary>
            /// Used to construct an instace of the class.
            /// </summary>
            /// <param name="ec"></param>
            internal CompletedArgs(EventCode ec)
            {
                Result = ec;
            }
        }

    }

    // A generic class to support easily changing between my different sources of data.

    // Note: You DON'T have to use this class, or anything like it.  The key is the SampleCallback
    // routine.  How/where you get your bitmaps is ENTIRELY up to you.  Having SampleCallback call
    // members of this class was just the approach I used to isolate the data handling.
    public abstract class ImageHandler : IDisposable, IGenericSampleCB
    {
        #region Definitions

        /// <summary>
        /// 100 ns - used by a number of DS methods
        /// </summary>
        protected const long UNIT = 10000000;

        #endregion

        /// <summary>
        /// Number of callbacks that returned a positive result
        /// </summary>
        protected int m_iFrameNumber = 0;

        virtual public void Dispose()
        {
        }

        abstract public void SetMediaType(IGenericSampleConfig psc);
        abstract public int GetImage(int iFrameNumber, IntPtr ip, int iSize, out int iRead);
        virtual public int SetTimeStamps(IMediaSample pSample)
        {
            return 0;
        }

        /// <summary>
        /// Called by the GenericSampleSourceFilter.  This routine populates the MediaSample.
        /// </summary>
        /// <param name="pSample">Pointer to a sample</param>
        /// <returns>0 = success, 1 = end of stream, negative values for errors</returns>
        virtual public int SampleCallback(IMediaSample pSample)
        {
            int hr;
            IntPtr pData;

            try
            {
                // Get the buffer into which we will copy the data
                hr = pSample.GetPointer(out pData);
                if (hr >= 0)
                {
                    // Set TRUE on every sample for uncompressed frames
                    hr = pSample.SetSyncPoint(true);
                    if (hr >= 0)
                    {
                        // Find out the amount of space in the buffer
                        int cbData = pSample.GetSize();

                        hr = SetTimeStamps(pSample);
                        if (hr >= 0)
                        {
                            int iRead;

                            // Get copy the data into the sample
                            hr = GetImage(m_iFrameNumber, pData, cbData, out iRead);
                            if (hr == 0) // 1 == End of stream
                            {
                                pSample.SetActualDataLength(iRead);

                                // increment the frame number for next time
                                m_iFrameNumber++;
                            }
                        }
                    }
                }
            }
            finally
            {
                // Release our pointer the the media sample.  THIS IS ESSENTIAL!  If
                // you don't do this, the graph will stop after about 2 samples.
                Marshal.ReleaseComObject(pSample);
            }

            return hr;
        }
    }
}
