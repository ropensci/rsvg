#include <Rinternals.h>
#include <librsvg/rsvg.h>
#include <string.h>

SEXP R_rsvg(SEXP data){
  GError *err = NULL;
  RsvgHandle *svg = rsvg_handle_new_from_data (RAW(data), LENGTH(data), &err);
  if(err != NULL)
    Rf_error("Failed to parse svg: %s", err->message);
  RsvgDimensionData dimensions;
  rsvg_handle_get_dimensions(svg, &dimensions);
  int width = dimensions.width;
  int height = dimensions.height;
  cairo_surface_t *canvas = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
  cairo_t *cr = cairo_create(canvas);
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
