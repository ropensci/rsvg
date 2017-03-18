#include <R_ext/Rdynload.h>
#include <librsvg/rsvg.h>

void R_init_rsvg(DllInfo *info) {
  #if !defined(LIBRSVG_MAJOR_VERSION) || LIBRSVG_MAJOR_VERSION == 2
  #if !defined(LIBRSVG_MINOR_VERSION) || LIBRSVG_MINOR_VERSION < 36
  g_type_init();
  #endif
  #endif

  R_registerRoutines(info, NULL, NULL, NULL, NULL);
  R_useDynamicSymbols(info, TRUE);
}
