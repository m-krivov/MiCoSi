using System;
using System.Collections.Generic;
using System.Text;

namespace MiCoSi
{
  internal class CaptureOptions
  {
    private class WiseEnumeration<ENUM_TYPE, VALUE_TYPE>
    {
      private Dictionary<ENUM_TYPE, String> _enumToString;
      private Dictionary<ENUM_TYPE, VALUE_TYPE> _enumToValue;
      private Dictionary<String, ENUM_TYPE> _stringToEnum;

      public WiseEnumeration()
      {
        _enumToString = new Dictionary<ENUM_TYPE, string>();
        _enumToValue = new Dictionary<ENUM_TYPE, VALUE_TYPE>();
        _stringToEnum = new Dictionary<string, ENUM_TYPE>();
      }

      public void Register(ENUM_TYPE en, VALUE_TYPE value, String name)
      {
        _enumToString.Add(en, name);
        _enumToValue.Add(en, value);
        _stringToEnum.Add(name, en);
      }

      public String ToString(ENUM_TYPE en)
      {
        String res;
        if (_enumToString.TryGetValue(en, out res))
          return res;
        else
        { throw new ApplicationException("Internal error - unregistered enumeration element"); }
      }

      public VALUE_TYPE ToValue(ENUM_TYPE en)
      {
        VALUE_TYPE res;
        if (_enumToValue.TryGetValue(en, out res))
          return res;
        else
        { throw new ApplicationException("Internal error - unregistered enumeration element"); }
      }

      public ENUM_TYPE ToElement(String val)
      {
        ENUM_TYPE res;
        if (_stringToEnum.TryGetValue(val, out res))
          return res;
        else
        { throw new ApplicationException("Internal error - unregistered enumeration element"); }
      }

      public IEnumerable<ENUM_TYPE> AllElements
      {
        get
        { return _enumToValue.Keys; }
      }

    };

    public enum ImageContainer
    {
      Bmp,
      Jpeg,
      Png,
    }

    private static WiseEnumeration<ImageContainer,
                                   KeyValuePair<System.Drawing.Imaging.ImageFormat, String> > _imageContainers;

    public static IEnumerable<ImageContainer> ImageContainers
    {
      get
      { return _imageContainers.AllElements; }
    }

    public static String ToString(ImageContainer container)
    { return _imageContainers.ToString(container); }

    public static KeyValuePair<System.Drawing.Imaging.ImageFormat, String> ToValue(ImageContainer container)
    { return _imageContainers.ToValue(container); }

    public static void ToElement(String value, out ImageContainer container)
    { container = _imageContainers.ToElement(value); }

    public enum ImageSize
    {
      size640x480,
      size1024x768,
      size1600x1200,
      size2048x1536
    };

    private static WiseEnumeration<ImageSize, KeyValuePair<int, int>> _imageSizes;

    public static IEnumerable<ImageSize> ImageSizes
    {
      get
      { return _imageSizes.AllElements; }
    }

    public static String ToString(ImageSize size)
    { return _imageSizes.ToString(size); }

    public static KeyValuePair<int, int> ToValue(ImageSize size)
    { return _imageSizes.ToValue(size); }

    public static void ToElement(String value, out ImageSize size)
    { size = _imageSizes.ToElement(value); }

    public enum VideoCompression
    {
      MinimalSize,
      Optimal,
      BestQuality
    }


    private static WiseEnumeration<VideoCompression, System.Boolean> _videoCompressions;

    public static IEnumerable<VideoCompression> VideoCompressions
    {
      get
      { return _videoCompressions.AllElements; }
    }

    public static String ToString(VideoCompression compression)
    { return _videoCompressions.ToString(compression); }

    public static void ToElement(String value, out VideoCompression compression)
    { compression = _videoCompressions.ToElement(value); }

    public enum VideoFPS
    {
      fps10,
      fps25,
      fps60
    }

    private static WiseEnumeration<VideoFPS, int> _videoFPSs;

    public static IEnumerable<VideoFPS> VideoFPSs
    {
      get
      { return _videoFPSs.AllElements; }
    }

    public static String ToString(VideoFPS fps)
    { return _videoFPSs.ToString(fps); }

    public static int ToValue(VideoFPS fps)
    { return _videoFPSs.ToValue(fps); }

    public static void ToElement(String value, out VideoFPS fps)
    { fps = _videoFPSs.ToElement(value); }

    public enum VideoSize
    {
      size640x360,
      size852x480,
      size1280x720,
      size1920x1080
    }

    private static WiseEnumeration<VideoSize, KeyValuePair<int, int>> _videoSizes;

    public static IEnumerable<VideoSize> VideoSizes
    {
      get
      { return _videoSizes.AllElements; }
    }

    public static String ToString(VideoSize size)
    { return _videoSizes.ToString(size); }

    public static KeyValuePair<int, int> ToValue(VideoSize size)
    { return _videoSizes.ToValue(size); }

    public static void ToElement(String value, out VideoSize size)
    { size = _videoSizes.ToElement(value); }

    static CaptureOptions()
    {
      _imageContainers
        = new WiseEnumeration<ImageContainer, KeyValuePair<System.Drawing.Imaging.ImageFormat, string>>();
      _imageContainers.Register(ImageContainer.Bmp,
                                new KeyValuePair <System.Drawing.Imaging.ImageFormat, String >(System.Drawing.Imaging.ImageFormat.Bmp, ".bmp"),
                                "bmp");
      _imageContainers.Register(ImageContainer.Jpeg,
                                new KeyValuePair <System.Drawing.Imaging.ImageFormat, String >(System.Drawing.Imaging.ImageFormat.Jpeg, ".jpg"),
                                "jpeg");
      _imageContainers.Register(ImageContainer.Png,
                                new KeyValuePair<System.Drawing.Imaging.ImageFormat, String>(System.Drawing.Imaging.ImageFormat.Png, ".png"),
                                "png");

      _imageSizes = new WiseEnumeration<ImageSize, KeyValuePair<int, int>>();
      _imageSizes.Register(ImageSize.size640x480, new KeyValuePair<int, int>(640, 480), "640x480");
      _imageSizes.Register(ImageSize.size1024x768, new KeyValuePair<int,int>(1024, 768), "1024x768");
      _imageSizes.Register(ImageSize.size1600x1200, new KeyValuePair<int,int>(1600, 1200), "1600x1200");
      _imageSizes.Register(ImageSize.size2048x1536, new KeyValuePair<int, int>(2048, 1536), "2048x1536");

      _videoCompressions = new WiseEnumeration<VideoCompression, bool>();
      _videoCompressions.Register(VideoCompression.MinimalSize, false, "Minimal size");
      _videoCompressions.Register(VideoCompression.Optimal, false, "Optimal");
      _videoCompressions.Register(VideoCompression.BestQuality, false, "BestQuality");

      _videoFPSs = new WiseEnumeration<VideoFPS, int>();
      _videoFPSs.Register(VideoFPS.fps10, 10, "10");
      _videoFPSs.Register(VideoFPS.fps25, 25, "25");
      _videoFPSs.Register(VideoFPS.fps60, 60, "60");

      _videoSizes = new WiseEnumeration<VideoSize, KeyValuePair<int, int>>();
      _videoSizes.Register(VideoSize.size640x360, new KeyValuePair<int, int>(640, 320), "640x320");
      _videoSizes.Register(VideoSize.size852x480, new KeyValuePair<int, int>(852, 480), "852x480");
      _videoSizes.Register(VideoSize.size1280x720, new KeyValuePair<int, int>(1280, 720), "1280x720");
      _videoSizes.Register(VideoSize.size1920x1080, new KeyValuePair<int, int>(1920, 1080), "1920x1080");
    }
  }
}
