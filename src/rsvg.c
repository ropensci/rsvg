#define R_NO_REMAP
#define STRICT_R_HEADERS

#include <Rinternals.h>
#include <R_ext/Rdynload.h>
#include <librsvg/rsvg.h>
#include <cairo.h>
#include <cairo-pdf.h>
#include <cairo-svg.h>
#include <cairo-ps.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  unsigned char *buf;
  size_t size;
} memory;

#if LIBRSVG_CHECK_VERSION(2,52,0)
static int unit_to_px(RsvgLength x){
  double l = x.length;
  switch(x.unit){
  case RSVG_UNIT_PX:
  case RSVG_UNIT_PT:
    return l;
  case RSVG_UNIT_IN:
    return 300*l;
  case RSVG_UNIT_CM:
    return 100*l;
  default:
    return 800;
  }
}
#endif

static void setup_render_handle(RsvgHandle *svg, cairo_t *cr){
#if LIBRSVG_CHECK_VERSION(2,52,0)
  GError *err = NULL;
  RsvgRectangle viewport = {0.0};
  //viewport.width = width;
  //viewport.height = height;
  rsvg_handle_render_document(svg, cr, &viewport, &err);
  if(err != NULL)
    Rf_error("Failure in rsvg_handle_render_document: %s", err->message);
#else
  if(!rsvg_handle_render_cairo(svg, cr))
    Rf_error("Cairo failed to render svg");
#endif
}

static SEXP write_bitmap(RsvgHandle *svg, int width, int height, double sx, double sy){
  cairo_surface_t *canvas = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
  cairo_t *cr = cairo_create(canvas);
  cairo_scale(cr, sx, sy);
  setup_render_handle(svg, cr);
  int stride = cairo_image_surface_get_stride(canvas); //should be equal to width * channels
  int size = stride * height;
  cairo_surface_flush(canvas);
  SEXP image = PROTECT(Rf_allocVector(RAWSXP, size));
  SEXP dim = Rf_allocVector(INTSXP, 3);
  INTEGER(dim)[0] = 4;
  INTEGER(dim)[1] = width;
  INTEGER(dim)[2] = height;
  Rf_setAttrib(image, R_DimSymbol, dim);
  memcpy(RAW(image), cairo_image_surface_get_data(canvas), size);
  UNPROTECT(1);
  g_object_unref(svg);
  cairo_surface_destroy(canvas);
  cairo_destroy(cr);
  return image;
}

static cairo_status_t write_func(void *ctx, const unsigned char *data, unsigned int length) {
  memory *mem = (memory*) ctx;
  #ifdef _WIN32
    mem->buf = realloc(mem->buf, exp2(ceil(log2(mem->size + length))));
  #else
    mem->buf = realloc(mem->buf, mem->size + length);
  #endif
  if (!mem->buf)
    return CAIRO_STATUS_WRITE_ERROR;
  memcpy(&(mem->buf[mem->size]), data, length);
  mem->size += length;
  return CAIRO_STATUS_SUCCESS;
}

static SEXP write_png(RsvgHandle *svg, int width, int height, double sx, double sy){
  cairo_surface_t *canvas = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
  cairo_t *cr = cairo_create(canvas);
  cairo_scale(cr, sx, sy);
  setup_render_handle(svg, cr);
  memory mem = {NULL, 0};
  cairo_surface_write_to_png_stream(canvas, write_func, &mem);
  cairo_surface_flush(canvas);
  cairo_surface_destroy(canvas);
  cairo_destroy(cr);
  g_object_unref(svg);
  SEXP res = Rf_allocVector(RAWSXP, mem.size);
  memcpy(RAW(res), mem.buf, mem.size);
  free(mem.buf);
  return res;
}

static SEXP write_stream(RsvgHandle *svg, int width, int height, double sx, double sy, cairo_surface_t* (*fun) (cairo_write_func_t, void *, double, double), cairo_bool_t is_eps) {
  memory buf = {NULL, 0};
  cairo_surface_t *canvas = fun(write_func, &buf, width, height);
  if(is_eps)
    cairo_ps_surface_set_eps(canvas, TRUE);
  cairo_t *cr = cairo_create(canvas);
  cairo_scale(cr, sx, sy);
  setup_render_handle(svg, cr);
  cairo_surface_show_page(canvas);
  cairo_surface_flush(canvas);
  cairo_surface_destroy(canvas);
  cairo_destroy(cr);
  g_object_unref(svg);
  SEXP res = Rf_allocVector(RAWSXP, buf.size);
  memcpy(RAW(res), buf.buf, buf.size);
  free(buf.buf);
  return res;
}

SEXP R_rsvg(SEXP data, SEXP rwidth, SEXP rheight, SEXP format, SEXP css){
  GError *err = NULL;
  RsvgHandle *svg = rsvg_handle_new_from_data (RAW(data), LENGTH(data), &err);
  if(err != NULL)
    Rf_error("Failed to parse svg: %s", err->message);
  if(Rf_length(css)){
#if LIBRSVG_CHECK_VERSION(2,48,0)
  if(!rsvg_handle_set_stylesheet(svg, (const unsigned char *) RAW(css), Rf_length(css),  &err) || err){
    //Note: this doesn't seem to work?
    //Looks like rsvg_handle_set_stylesheet never fails.
    g_object_unref(svg);
    Rf_error("Failed to load css stylesheet: %s", err ? err->message : "");
  }
#else
    Rf_warning("An external CSS file was specified but this requires at least librsvg 2.48 (you have %s)", LIBRSVG_VERSION);
#endif
  }
#if LIBRSVG_CHECK_VERSION(2,52,0)
  gboolean has_width;
  RsvgLength in_width;
  gboolean has_height;
  RsvgLength in_height;
  gboolean has_viewbox;
  RsvgRectangle viewbox;
  rsvg_handle_get_intrinsic_dimensions (svg, &has_width, &in_width, &has_height, &in_height, &has_viewbox, &viewbox);
  double input_width = has_viewbox ? viewbox.width : (has_width ? unit_to_px(in_width) : 800);
  double input_height = has_viewbox ? viewbox.height : (has_height ? unit_to_px(in_height) : 800);
  //REprintf("Size: %fx%f\n", input_width, input_height);
#else
  RsvgDimensionData dimensions;
  rsvg_handle_get_dimensions(svg, &dimensions);
  int input_width = dimensions.width;
  int input_height = dimensions.height;
  //REprintf("Size: %fx%f\n", (double)input_width, (double)input_height);
#endif

  //scale into the requested resolution
  double width;
  double height;
  double sx;
  double sy;
  if(rwidth == R_NilValue && rheight == R_NilValue){
    width = input_width;
    height = input_height;
    sx = sy = 1;
  } else if(rwidth != R_NilValue && rheight != R_NilValue){
    width = Rf_asInteger(rwidth);
    height = Rf_asInteger(rheight);
    sx = width / input_width;
    sy = height / input_height;
  } else if(rwidth != R_NilValue){
    width = Rf_asInteger(rwidth);
    sx = sy = width / input_width;
    height = round(input_height * sy);
  } else {
    height = Rf_asInteger(rheight);
    sx = sy = height / input_height;
    width = round(input_width * sx);
  }
  switch(Rf_asInteger(format)){
  case 0:
    return write_bitmap(svg, width, height, sx, sy);
  case 1:
    return write_png(svg, width, height, sx, sy);
  case 2:
    return write_stream(svg, width, height, sx, sy, cairo_pdf_surface_create_for_stream, FALSE);
  case 3:
    return write_stream(svg, width, height, sx, sy, cairo_svg_surface_create_for_stream, FALSE);
  case 4:
    return write_stream(svg, width, height, sx, sy, cairo_ps_surface_create_for_stream, FALSE);
  case 5:
    return write_stream(svg, width, height, sx, sy, cairo_ps_surface_create_for_stream, TRUE);
  }
  return R_NilValue;
}

SEXP R_librsvg_version(){
  return Rf_mkString(LIBRSVG_VERSION);
}

void R_init_rsvg(DllInfo *dll) {

#if !defined(LIBRSVG_MAJOR_VERSION) || LIBRSVG_MAJOR_VERSION == 2
#if !defined(LIBRSVG_MINOR_VERSION) || LIBRSVG_MINOR_VERSION < 36
  g_type_init();
#endif
#endif

  static const R_CallMethodDef CallEntries[] = {
    {"R_rsvg", (DL_FUNC) &R_rsvg, 5},
    {"R_librsvg_version", (DL_FUNC) &R_librsvg_version, 0},
    {NULL, NULL, 0}
  };

  R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
  R_useDynamicSymbols(dll, FALSE);
}
