#' Render SVG into Bitmap
#'
#' Render svg image into a high quality bitmap. When both \code{width} and \code{height}
#' are \code{NULL}, the output resolution matches that of the input. When either \code{width}
#' or \code{height} is specified, the image is scaled proportionally. When both \code{width}
#' and \code{height} are specified, the image is stretched into the requested size.
#'
#' @export
#' @useDynLib rsvg R_rsvg
#' @param svg path/url to svg file or raw vector with svg data. Use \link{charToRaw} to convert
#' an SVG string into raw data.
#' @param width output width in pixels or \code{NULL} for default.
#' @param height output height in pixels or \code{NULL} for default
#' @param css path/url to external css file or raw vector with css data. This
#' requires your system has a recent version of librsvg.
#' @rdname rsvg
#' @examples # create some svg
#' options(example.ask=FALSE)
#' tmp <- tempfile()
#' svglite::svglite(tmp, width = 10, height = 7)
#' ggplot2::qplot(mpg, wt, data = mtcars, colour = factor(cyl))
#' dev.off()
#'
#' # convert directly into a bitmap format
#' rsvg_pdf(tmp, "out.pdf")
#' rsvg_png(tmp, "out.png")
#' rsvg_svg(tmp, "out.svg")
#' rsvg_ps(tmp, "out.ps")
#'
#' # render into raw bitmap array
#' bitmap <- rsvg(tmp, height = 1440)
#' dim(bitmap) # h*w*c
#'
#' # read in your package of choice
#' magick::image_read(bitmap)
#' webp::write_webp(bitmap, "bitmap.webp", quality = 100)
#'
#' # cleanup
#' unlink(c("out.pdf", "out.png", "out.svg", "out.ps", "bitmap.webp"))
rsvg <- function(svg, width = NULL, height = NULL, css = NULL) {
  out <- rsvg_raw(svg, width = width, height = height, css = css)
  out <- structure(as.numeric(out)/255, dim = dim(out))
  aperm(out) # Convert to standard with*height*rgba
}

#' @rdname rsvg
#' @export
rsvg_raw <- function(svg, width = NULL, height = NULL, css = NULL) {
  svg <- read_data(svg)
  if(length(css)){
    css <- read_data(css)
  }
  stopifnot(is.null(width) || is.numeric(width))
  stopifnot(is.null(height) || is.numeric(height))
  out <- .Call(R_rsvg, svg, width, height, 0L, css)
  out[c(3,2,1,4),,, drop = FALSE]
}

#' @rdname rsvg
#' @export
rsvg_webp <- function(svg, file = NULL, width = NULL, height = NULL, css = NULL) {
  out <- rsvg_raw(svg, width = width, height = height, css = css)
  webp::write_webp(out, file, 100)
}

#' @rdname rsvg
#' @export
#' @param file path to output file or \code{NULL} to return content as raw vector
rsvg_png <- function(svg, file = NULL, width = NULL, height = NULL, css = NULL) {
  rsvg_format(svg, file, width = width, height = height, css = css, format = 1L)
}

#' @rdname rsvg
#' @export
rsvg_pdf <- function(svg, file = NULL, width = NULL, height = NULL, css = NULL) {
  rsvg_format(svg, file, width = width, height = height, css = css, format = 2L)
}

#' @rdname rsvg
#' @export
rsvg_svg <- function(svg, file = NULL, width = NULL, height = NULL, css = NULL) {
  rsvg_format(svg, file, width = width, height = height, css = css, format = 3L)
}

#' @rdname rsvg
#' @export
rsvg_ps <- function(svg, file = NULL, width = NULL, height = NULL, css = NULL) {
  rsvg_format(svg, file, width = width, height = height, css = css, format = 4L)
}

#' @rdname rsvg
#' @export
rsvg_eps <- function(svg, file = NULL, width = NULL, height = NULL, css = NULL) {
  rsvg_format(svg, file, width = width, height = height, css = css, format = 5L)
}

rsvg_format <- function(svg, file = NULL, width = NULL, height = NULL, css = NULL, format = 0L) {
  svg <- read_data(svg)
  if(length(css)){
    css <- read_data(css)
  }
  stopifnot(is.null(width) || is.numeric(width))
  stopifnot(is.null(height) || is.numeric(height))
  out <- .Call(R_rsvg, svg, width, height, format, css)
  if(is.character(file)){
    writeBin(out, file[1])
  } else {
    return(out)
  }
}

read_data <- function(svg){
  if(is.character(svg)){
    if(grepl("^https?://", svg)){
      con <- url(svg, "rb")
      on.exit(close(con))
      svg <- raw();
      while(length(buf <- readBin(con, raw(), 1e6))){
        svg <- c(svg, buf)
      }
    } else if(file.exists(svg)){
      svg <- readBin(svg, raw(), file.info(svg)$size)
    } else {
      stop("Argument 'svg' or 'css' must be a file path, url, or raw vector.")
    }
  }
  stopifnot(is.raw(svg))
  return(svg)
}
