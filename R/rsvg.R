#' Render SVG into Bitmap
#'
#' Render svg image into a high quality bitmap. When both \code{width} and \code{height}
#' are \code{NULL}, the output resolution matches that of the input. When either \code{width}
#' or \code{height} is specified, the image is scaled proportionally. When both \code{width}
#' and \code{height} are specified, the image is stretched into the requested size.
#'
#' @export
#' @useDynLib rsvg R_rsvg
#' @param svg path to svg file or raw vector with svg data. Use \link{charToRaw} to convert
#' strings into raw vectors.
#' @param width output width in pixels or \code{NULL} for default.
#' @param height output height in pixels or \code{NULL} for default
rsvg <- function(svg, width = NULL, height = NULL) {
  if(is.character(svg)){
    if(file.exists(svg)){
      svg <- readBin(svg, raw(), file.info(svg)$size)
    } else {
      stop("Argument 'svg' must be a file path or raw vector.")
    }
  }
  stopifnot(is.raw(svg))
  stopifnot(is.null(width) || is.numeric(width))
  stopifnot(is.null(height) || is.numeric(height))
  out <- .Call(R_rsvg, svg, width, height)
  out <- structure(as.numeric(out)/255, dim = dim(out))
  aperm(out)[,,c(3,2,1,4)] # Convert to standard with*height*rgba
}
