
#include "HardwareScanner.h"

#ifdef WIN32
#include <Windows.h>
#else
#include <sys/sysinfo.h>
#include <cstring>
#endif

//-----------------------
//--- HardwareScanner ---
//-----------------------

static inline std::string DetermineCpuModel()
{
#ifdef WIN32
  HKEY rKey;
  if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
            "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
            0,
            KEY_QUERY_VALUE,
            &rKey) != ERROR_SUCCESS)
    return std::string("Unknown CPU model");
  else
  {
    DWORD size = 1024;
    std::string res(size, '\0');
    if (RegQueryValueExA(rKey,
               "ProcessorNameString",
               NULL,
               NULL,
               (LPBYTE)res.c_str(),
               &size) != ERROR_SUCCESS)
    { res = std::string("Unknown CPU model"); }

    RegCloseKey(rKey);
    return res;
  }
#else
  FILE * fp;
  char res[2048] = { 0 };
  fp = popen("/bin/cat /proc/cpuinfo | grep 'model name'", "r");
  char* fr = fgets(res, sizeof(res) - 1, fp);
  pclose(fp);

  if(fr == NULL)
    throw new std::runtime_error("Cannot read file - fgets return NULL");

  if (res[0])
  {
    char *mn = strchr(res, ':');
    if (mn && strlen(mn) > 2) {
      mn += 2;
      while (mn[0] && mn[strlen(mn) - 1] == '\n')
        mn[strlen(mn) - 1] = 0;
      return mn;
    }
  }
  return "Unknown CPU model";
#endif
}

static inline size_t DetermineCpuCores()
{
#ifdef WIN32
  SYSTEM_INFO sinfo;
  GetSystemInfo(&sinfo);
  return (size_t)sinfo.dwNumberOfProcessors;
#else
  FILE * fp;
  char res[128] = { 0 };
  fp = popen("/bin/cat /proc/cpuinfo | grep -c '^processor'", "r");
  size_t fr = fread(res, 1, sizeof(res)-1, fp);
  pclose(fp);

  if(fr <= 0)
    throw new std::runtime_error("Cannot read file");

  if (res[0])
  {
    return std::max(1, atoi(res));
  }

  return 1;

#endif
}

static inline double DetermineCpuFreq()
{
#ifdef  WIN32
  HKEY rKey;
  if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
            "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
            0,
            KEY_QUERY_VALUE,
            &rKey) != ERROR_SUCCESS)
    return 0.0;
  else
  {
    DWORD res = 0;
    DWORD size = sizeof(DWORD);
    if (RegQueryValueExA(rKey,
               "~MHz",
               NULL,
               NULL,
               (LPBYTE)&res,
               &size) != ERROR_SUCCESS)
    { res = 0; }
    RegCloseKey(rKey);
    return (double)res;
  }
#else
//#error Pls, verify me!
  FILE * fp;
  char res[2048] = { 0 };
  fp = popen("/bin/cat /proc/cpuinfo | grep 'cpu MHz'", "r");
  double mhz = 0;
  while (fgets(res, sizeof(res) - 1, fp)) {
    char *mh = strchr(res, ':');
    if (mh && strlen(mh) > 2) {
      mh += 2;
      double cand = atof(mh);
      if (cand > mhz)
        mhz = cand;
    }
  }
  pclose(fp);
  return mhz;
#endif
}

HardwareInfo HardwareScanner::Scan()
{
  // Getting information about CPU.
  auto cpu = HardwareInfo::DeviceInfo(DetermineCpuModel(),
                                      DetermineCpuCores(),
                                      DetermineCpuFreq());

  // Getting information about GPUs.
  std::vector<HardwareInfo::DeviceInfo> gpus;

#ifndef NO_CUDA
  int devCount = 0;
  CheckError(cudaGetDeviceCount(&devCount));
  for (int i = 0; i < devCount; i++)
  {
    cudaDeviceProp props;
    CheckError(cudaGetDeviceProperties(&props, i));
    
    size_t cores = props.multiProcessorCount;
    if (props.major == 1)
      cores *= 8;
    else if (props.major == 2 && props.minor == 0)
      cores *= 32;
    else if (props.major == 2 && props.minor == 1)
      cores *= 48;
    else if (props.major == 3)
      cores *= 192;
    else if (props.major == 5)
      cores *= 128;
    else
      cores *= 64;

    gpus.push_back(HardwareInfo::DeviceInfo(props.name, cores, props.clockRate * 1e-3));
  }
#endif
  
  return HardwareInfo(cpu, gpus);
}
