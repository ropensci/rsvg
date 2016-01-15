#include <Rinternals.h>
#include <librsvg/rsvg.h>
#include <cairo.h>
#include <cairo-pdf.h>
#include <stdlib.h>
#include <string.h>

SEXP R_rsvg(SEXP data, SEXP rwidth, SEXP rheight, SEXP pdf);
SEXP write_bitmap(RsvgHandle *svg, int width, int height, int sx, int sy);
SEXP write_pdf(RsvgHandle *svg, int width, int height, int sx, int sy);

typedef struct {
  unsigned char *buf;
  size_t size;
} memory;

SEXP R_rsvg(SEXP data, SEXP rwidth, SEXP rheight, SEXP pdf){
  GError *err = NULL;
  RsvgHandle *svg = rsvg_handle_new_from_data (RAW(data), LENGTH(data), &err);
  if(err != NULL)
    Rf_error("Failed to parse svg: %s", err->message);
  RsvgDimensionData dimensions;
  rsvg_handle_get_dimensions(svg, &dimensions);

  //scale into the requested resolution
  double width;
  double height;
  double sx;
  double sy;
  if(rwidth == R_NilValue && rheight == R_NilValue){
    width = dimensions.width;
    height = dimensions.height;
    sx = sy = 1;
  } else if(rwidth != R_NilValue && rheight != R_NilValue){
    width = asInteger(rwidth);
    height = asInteger(rheight);
    sx = width / dimensions.width;
    sy = height / dimensions.height;
  } else if(rwidth != R_NilValue){
    width = asInteger(rwidth);
    sx = sy = width / dimensions.width;
    height = round(dimensions.height * sy);
  } else {
    height = asInteger(rheight);
    sx = sy = height / dimensions.height;
    width = round(dimensions.width * sx);
  }
  switch(asInteger(pdf)){
  case 0:
    return write_bitmap(svg, width, height, sx, sy);
  case 1:
    return write_pdf(svg, width, height, sx, sy);
  }
  return R_NilValue;
}

SEXP write_bitmap(RsvgHandle *svg, int width, int height, int sx, int sy){
  cairo_surface_t *canvas = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
  cairo_t *cr = cairo_create(canvas);
  cairo_scale(cr, sx, sy);
  if(!rsvg_handle_render_cairo(svg, cr))
    Rf_error("Cairo failed to render svg");
  int stride = cairo_image_surface_get_stride(canvas); //should be equal to width * channels
  int size = stride * height;
  cairo_surface_flush(canvas);
  SEXP image = PROTECT(allocVector(RAWSXP, size));
  SEXP dim = allocVector(INTSXP, 3);
  INTEGER(dim)[0] = 4;
  INTEGER(dim)[1] = width;
  INTEGER(dim)[2] = height;
  setAttrib(image, mkString("dim"), dim);
  memcpy(RAW(image), cairo_image_surface_get_data(canvas), size);
  UNPROTECT(1);
  g_object_unref(svg);
  cairo_destroy(cr);
  cairo_surface_destroy(canvas);
  return image;
}

cairo_status_t write_func(void *ctx, const unsigned char *data, unsigned int length) {
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

SEXP write_pdf(RsvgHandle *svg, int width, int height, int sx, int sy){
  memory buf = {NULL, 0};
  cairo_surface_t *canvas = cairo_pdf_surface_create_for_stream(write_func, &buf, width, height);
  cairo_t *cr = cairo_create(canvas);
  cairo_scale(cr, sx, sy);
  if(!rsvg_handle_render_cairo(svg, cr))
    Rf_error("Cairo failed to render svg");
  cairo_surface_show_page(canvas);
  cairo_surface_flush(canvas);
  cairo_destroy(cr);
  cairo_surface_destroy(canvas);
  SEXP res = allocVector(RAWSXP, buf.size);
  memcpy(RAW(res), buf.buf, buf.size);
  free(buf.buf);
  return res;
}
