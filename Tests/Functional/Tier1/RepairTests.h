#pragma once
#include "Defs.h"
#include "Helpers.h"

static inline void REPAIR_VEIFY_FILE(String ^cellfile, bool corrupted)
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
    if (someNumber == 0) { FAIL() << "Test is broken"; }
  }
  catch (Exception ^)
  {
    if (!corrupted)
    { FAIL() << "Failed to open correct cell file"; }
    return;
  }
  finally
  {
    if (ts != nullptr) { delete ts; }
  }
  if (corrupted)
  { FAIL() << "Corrupted file was successfully opened"; }
}

#define REPAIR_TEST(breaker)                                        \
{                                                                   \
  Helper::PrepareTestDirectory();                                   \
  String ^correctFile = "results_correct.cell";                     \
  String ^brokenFile  = "results_corrupted.cell";                   \
                                                                    \
  FileStream ^correct = nullptr, ^broken = nullptr;                 \
  try                                                               \
  {                                                                 \
    auto parameters = gcnew LaunchParameters();                     \
    parameters->Config = gcnew SimParams();                         \
    parameters->Config[SimParameter::Int::N_MT_Total] = 200;        \
    parameters->Config[SimParameter::Int::N_Cr_Total] = 2;          \
    parameters->Config[SimParameter::Double::T_End] = 50.0;         \
    parameters->Args->RngSeed = 100500;                             \
                                                                    \
    /*Creating correct cell file*/                                  \
    parameters->Args->CellFile = correctFile;                       \
    if (Helper::Launch(parameters)->ExitedWithError)                \
    { FAIL() << "Failed to launch simulator"; }                     \
                                                                    \
    /*Checking, that it's correct*/                                 \
    REPAIR_VEIFY_FILE(correctFile, false);                          \
                                                                    \
    /*Breaking file*/                                               \
    correct = File::OpenRead(Path::Combine(Helper::TestDirectory,   \
                                           correctFile));           \
    broken = File::OpenWrite(Path::Combine(Helper::TestDirectory,   \
                                           brokenFile));            \
    breaker(correct, broken);                                       \
    delete correct; correct = nullptr;                              \
    delete broken; broken = nullptr;                                \
                                                                    \
    /*Checking, that the broken file cannot be opened*/             \
    REPAIR_VEIFY_FILE(brokenFile, true);                            \
                                                                    \
    /*Repairing*/                                                   \
    parameters->Args = gcnew CliArgs();                             \
    parameters->Args->Mode = LaunchMode::Fix;                       \
    parameters->Args->CellFile = brokenFile;                        \
    if (Helper::Launch(parameters)->ExitedWithError)                \
    { FAIL() << "Failed to repair cell file"; }                     \
                                                                    \
    /*Checking again, all troubles must be fixed*/                  \
    REPAIR_VEIFY_FILE(brokenFile, false);                           \
  }                                                                 \
  catch (Exception ^ex) { FAIL() << StringToString(ex->Message); }  \
  finally                                                           \
  {                                                                 \
    if (correct != nullptr) { delete correct; }                     \
    if (broken != nullptr) { delete broken; }                       \
    Helper::ClearUpTestDirectory();                                 \
  }                                                                 \
}

static inline void Repair_BreakSegment(FileStream ^in_correct, FileStream ^out_broken)
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
  REPAIR_TEST(Repair_BreakSegment);
}
