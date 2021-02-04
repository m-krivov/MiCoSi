#pragma once
#include "Defs.h"
#include "Helpers.h"

static inline std::string RepairVerifyFile(String ^cellfile, bool corrupted)
{
  TimeStream ^ts = nullptr;
  try
  {
    ts = TimeStream::Open(Path::Combine(Helper::TestDirectory, cellfile));
    int someNumber = 0;
    while (ts->MoveNext())
    {
      someNumber += Enumerable::Count(ts->Current->Cell->MTs);
    }
    if (someNumber == 0)
    { return "Test is broken"; }
  }
  catch (Exception ^)
  { return corrupted ? std::string() : "Failed to open correct cell file"; }
  finally
  { if (ts != nullptr) { delete ts; } }
  
  return corrupted ? "Corrupted file was successfully opened" : std::string();
}

#define REPAIR_TEST(breaker)                                          \
{                                                                     \
  Helper::PrepareTestDirectory();                                     \
  String ^correctFile = "results_correct.cell";                       \
  String ^brokenFile  = "results_corrupted.cell";                     \
                                                                      \
  FileStream ^correct = nullptr, ^broken = nullptr;                   \
  try                                                                 \
  {                                                                   \
    auto parameters = gcnew LaunchParameters();                       \
    parameters->Config = gcnew SimParams();                           \
    parameters->Config[SimParameter::Int::N_MT_Total] = 200;          \
    parameters->Config[SimParameter::Int::N_Cr_Total] = 2;            \
    parameters->Config[SimParameter::Double::T_End] = 50.0;           \
    parameters->Args->UserSeed = 100500;                              \
                                                                      \
    /*Creating correct cell file*/                                    \
    parameters->Args->CellFile = correctFile;                         \
    if (Helper::Launch(parameters)->ExitedWithError)                  \
    { FAIL() << "Failed to launch simulator"; }                       \
                                                                      \
    /*Checking, that it's correct*/                                   \
    auto ret = RepairVerifyFile(correctFile, false);                  \
    ASSERT_TRUE(ret.empty()) << ret;                                  \
                                                                      \
    /*Breaking file*/                                                 \
    correct = File::OpenRead(Path::Combine(Helper::TestDirectory,     \
                                           correctFile));             \
    broken = File::OpenWrite(Path::Combine(Helper::TestDirectory,     \
                                           brokenFile));              \
    breaker(correct, broken);                                         \
    delete correct; correct = nullptr;                                \
    delete broken; broken = nullptr;                                  \
                                                                      \
    /*Checking, that the broken file cannot be opened*/               \
    ret = RepairVerifyFile(brokenFile, true);                         \
    ASSERT_TRUE(ret.empty()) << ret;                                  \
                                                                      \
    /*Repairing*/                                                     \
    parameters->Args = gcnew CliArgs();                               \
    parameters->Args->Mode = LaunchMode::Fix;                         \
    parameters->Args->CellFile = brokenFile;                          \
    if (Helper::Launch(parameters)->ExitedWithError)                  \
    { FAIL() << "Failed to repair cell file"; }                       \
                                                                      \
    /*Checking again, all troubles must be fixed*/                    \
    ret = RepairVerifyFile(brokenFile, false);                        \
    ASSERT_TRUE(ret.empty()) << ret;                                  \
  }                                                                   \
  catch (Exception ^ex) { FAIL() << StringToString(ex->Message); }    \
  finally                                                             \
  {                                                                   \
    if (correct != nullptr) { delete correct; }                       \
    if (broken != nullptr) { delete broken; }                         \
    Helper::ClearUpTestDirectory();                                   \
  }                                                                   \
}

static inline void RepairBreakSegment(FileStream ^in_correct, FileStream ^out_broken)
{
  auto rnd = gcnew Random();
  int c = 0;
  int n = 0;
  while ((c = in_correct->ReadByte()) >= 0)
  {
    n += 1;
    if (n > 512 * 1024 && n < 520 * 1024)
    { c = (Byte)(rnd->Next() % 256); }
    out_broken->WriteByte((Byte)c);
  }
}

TEST(Repair, BrokenSegment)
{
  REPAIR_TEST(RepairBreakSegment);
}
