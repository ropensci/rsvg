% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/rsvg.R
\name{rsvg}
\alias{rsvg}
\alias{rsvg_raw}
\alias{rsvg_nativeraster}
\alias{rsvg_webp}
\alias{rsvg_png}
\alias{rsvg_pdf}
\alias{rsvg_svg}
\alias{rsvg_ps}
\alias{rsvg_eps}
\title{Render SVG into Bitmap}
\usage{
rsvg(svg, width = NULL, height = NULL, css = NULL)

rsvg_raw(svg, width = NULL, height = NULL, css = NULL)

rsvg_nativeraster(svg, width = NULL, height = NULL, css = NULL)

rsvg_webp(svg, file = NULL, width = NULL, height = NULL, css = NULL)

rsvg_png(svg, file = NULL, width = NULL, height = NULL, css = NULL)

rsvg_pdf(svg, file = NULL, width = NULL, height = NULL, css = NULL)

rsvg_svg(svg, file = NULL, width = NULL, height = NULL, css = NULL)

rsvg_ps(svg, file = NULL, width = NULL, height = NULL, css = NULL)

rsvg_eps(svg, file = NULL, width = NULL, height = NULL, css = NULL)
}
\arguments{
\item{svg}{path/url to svg file or raw vector with svg data. Use \link{charToRaw} to convert
an SVG string into raw data.}

\item{width}{output width in pixels or \code{NULL} for default.}

\item{height}{output height in pixels or \code{NULL} for default}

\item{css}{path/url to external css file or raw vector with css data. This
requires your system has a recent version of librsvg.}

\item{file}{path to output file or \code{NULL} to return content as raw vector}
}
\description{
Render svg image into a high quality bitmap. When both \code{width} and \code{height}
are \code{NULL}, the output resolution matches that of the input. When either \code{width}
or \code{height} is specified, the image is scaled proportionally. When both \code{width}
and \code{height} are specified, the image is stretched into the requested size.
}
\examples{
# create some svg
options(example.ask=FALSE)
tmp <- tempfile()
svglite::svglite(tmp, width = 10, height = 7)
ggplot2::qplot(mpg, wt, data = mtcars, colour = factor(cyl))
dev.off()

# convert directly into a vector or bitmap graphics format
rsvg_pdf(tmp, "out.pdf")
rsvg_png(tmp, "out.png")
rsvg_svg(tmp, "out.svg")
rsvg_ps(tmp, "out.ps")
rsvg_eps(tmp, "out.eps")

# render into raw bitmap array
bitmap <- rsvg(tmp, height = 1440)
dim(bitmap) # h*w*c

# render to native raster object
nr <- rsvg_nativeraster(tmp)
# grid::grid.raster(nr)

# read in your package of choice
magick::image_read(bitmap)
webp::write_webp(bitmap, "bitmap.webp", quality = 100)

# cleanup
unlink(c("out.*", "bitmap.webp"))
}
