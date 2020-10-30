#pragma once
#include "Defs.h"

static inline std::string StringToString(String ^str)
{
  std::string res;
  if (str->Length != 0)
  {
    res.resize(str->Length);
    for (int i = 0; i < str->Length; i++)
    {
      res[i] = (char)str[i];
    }
  }
  return res;
}

public ref class Helper
{
  public:
    static property String ^WorkingDirectory
    {
      String ^get() { return Directory::GetCurrentDirectory(); }
    }

    static property String ^TestDirectory
    {
      String ^get() { return Path::Combine(WorkingDirectory, "Tests"); }
    }

    static property String ^SimulatorFile
    {
      String ^get() { return Path::Combine(WorkingDirectory, "mitosis.exe"); }
    }

    static void PrepareTestDirectory()
    {
      String ^dir = TestDirectory;
      if (Directory::Exists(dir)) Directory::Delete(dir, true);
      Directory::CreateDirectory(dir);
    }

    static void ClearUpTestDirectory()
    {
      String ^dir = TestDirectory;
      if (Directory::Exists(dir)) Directory::Delete(dir, true);
    }

    static IntPtr CopyData(Cell ^cell)
    {
      int dataSize = cell->Data->DataSize;
      auto res = Marshal::AllocHGlobal(dataSize + 12);
      Int32 *p = (Int32 *)(void *)res;
      p[0] = dataSize;
      p[1] = cell->Data->MTsPerPole;
      p[2] = cell->Data->ChromosomePairs;
      for (int i = 0; i < dataSize / 4; i++)
        p[3 + i] = ((Int32 *)(void *)cell->Data->DataPointer)[i];
      return res;
    }

    static bool CompareData(IntPtr cell1, IntPtr cell2)
    {
      Int32 *p1 = (Int32 *)(void *)cell1;
      Int32 *p2 = (Int32 *)(void *)cell2;
      if (p1[0] != p2[0] || p1[0] <= 0)
        return false;
      for (int i = 1; i < p1[0] / 4 + 2; i++)
        if (p1[i] != p2[i])
          return false;
      return true;
    }

    static void ReleaseData(IntPtr data)
    { Marshal::FreeHGlobal(data); }

    static LaunchResult ^Launch(LaunchParameters ^parameters)
    {
      Launcher launcher(Helper::TestDirectory, Helper::SimulatorFile, parameters);
      return launcher.StartAndWait();
    }

    static TimeStream ^LaunchAndOpen(LaunchParameters ^parameters)
    {
      if (parameters->Args->CellCount != 1)
      { throw gcnew ApplicationException("Test error - cannot open multiple files"); }

      Launcher launcher(Helper::TestDirectory, Helper::SimulatorFile, parameters);
      auto res = launcher.StartAndWait();
      if (res->ExitedWithError)
      {
        throw gcnew ApplicationException(String::Format("Failed to launch the simulation process ({0}: {1})",
                                                        res->ExitCode, res->Output));
      }
      String ^resFile = launcher.Params->Args->CellFile;
      resFile = Path::IsPathRooted(resFile) ? resFile : Path::Combine(launcher.WorkingDir, resFile);
      return TimeStream::Open(resFile);
    }
};

// Assertion 'FAIL()' can be used only in test or test fixture
// So, we are forced to define this helper as macro
#define UNIFIED_TEST(parameters, checker, arg)          \
do {                                                    \
  TimeStream ^ts = nullptr;                             \
  try                                                   \
  {                                                     \
    Helper::PrepareTestDirectory();                     \
    ts = Helper::LaunchAndOpen(parameters);             \
    auto ret = checker(ts, arg);                        \
    if (!String::IsNullOrEmpty(ret))                    \
    { FAIL() << StringToString(ret); }                  \
  }                                                     \
  catch (Exception ^ex)                                 \
  { FAIL() << StringToString(ex->Message); }            \
  finally                                               \
  {                                                     \
    if (ts != nullptr) { delete ts; }                   \
    Helper::ClearUpTestDirectory();                     \
  }                                                     \
} while (false)
