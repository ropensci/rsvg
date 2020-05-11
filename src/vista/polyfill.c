#include <Windows.h>

#ifndef _WIN32_WINNT_WIN7
WINBASEAPI WINBOOL WINAPI SetThreadErrorMode(DWORD dwNewMode, LPDWORD lpOldMode){
  SetErrorMode(dwNewMode);
  return TRUE;
}
#endif
