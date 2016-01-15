#include <Rinternals.h>
#include <librsvg/rsvg.h>
#include <cairo.h>
#include <string.h>

SEXP R_rsvg(SEXP data, SEXP rwidth, SEXP rheight){
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
