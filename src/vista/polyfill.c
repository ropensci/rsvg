#include <Windows.h>

WINBASEAPI WINBOOL WINAPI PolySetThreadErrorMode(DWORD dwNewMode, LPDWORD lpOldMode){
  SetErrorMode(dwNewMode);
  return TRUE;
}
