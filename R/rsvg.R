#' Render SVG
#'
#' Render svg data into high quality bitmap.
#'
#' @export
#' @useDynLib rsvg R_rsvg
#' @param path to svg file or raw vector with svg data
rsvg <- function(svg) {
  if(is.character(svg)){
    if(file.exists(svg)){
      svg <- readBin(svg, raw(), file.info(svg)$size)
    } else {
      stop("Argument 'svg' must be a file path or raw vector.")
    }
  }
  stopifnot(is.raw(svg))
  out <- .Call(R_rsvg, svg)
  out <- structure(as.numeric(out)/255, dim = dim(out))
  aperm(out)[,,c(3,2,1,4)] # Convert to standard with*height*rgba
}
