#include <Windows.h>

WINBASEAPI WINBOOL WINAPI SetThreadErrorMode(DWORD dwNewMode, LPDWORD lpOldMode){
  SetErrorMode(dwNewMode);
  return TRUE;
}
