#pragma once

namespace MiCoSi
{
  public interface class IObjectWithID
  {
    public:
      property System::UInt32 ID
      { System::UInt32 get(); }
  };
}
