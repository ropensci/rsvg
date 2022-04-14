# rsvg <img src="man/figures/logo.svg" align="right" alt="logo" width="120" height = "139" style = "border: none; float: right;">

> Render SVG Images into PDF, PNG, PostScript, or Bitmap Arrays

<!-- badges: start -->
[![Project Status: Active – The project has reached a stable, usable state and is being actively developed.](https://www.repostatus.org/badges/latest/active.svg)](https://www.repostatus.org/#active)
![runiverse-name](https://ropensci.r-universe.dev/badges/:name)
![runiverse-package](https://ropensci.r-universe.dev/badges/rsvg)
![cran-badge](http://www.r-pkg.org/badges/version/rsvg)
<!-- badges: end -->

Renders vector-based 'svg' images into high-quality custom-size bitmap
arrays using 'librsvg'. The resulting bitmap can be written to e.g. 'png', 'jpeg'
or 'webp' format. In addition, the package can convert images directly to various
formats such as pdf or postscript.

## Documentation

 - The R package: https://docs.ropensci.org/rsvg
 - Upstream librsvg: https://gitlab.gnome.org/GNOME/librsvg

## Hello World

```r
# create some svg
tmp <- tempfile()
svglite::svglite(tmp, width = 10, height = 7)
ggplot2::qplot(mpg, wt, data = mtcars, colour = factor(cyl))
dev.off()

# render it into a bitmap array
bitmap <- rsvg(tmp, height = 1440)
dim(bitmap) # h*w*c
png::writePNG(bitmap, "bitmap.png", dpi = 144)
jpeg::writeJPEG(bitmap, "bitmap.jpg", quality = 1)
webp::write_webp(bitmap, "bitmap.webp", quality = 100)

# render straight to output format
rsvg_pdf(tmp, "out.pdf")
rsvg_png(tmp, "out.png")
rsvg_svg(tmp, "out.svg")
rsvg_ps(tmp, "out.ps")
```

## Installation

Binary packages for __OS-X__ or __Windows__ can be installed directly from CRAN:

```r
install.packages("rsvg")
```

Installation from source on Linux or OSX requires [`librsvg2`](https://wiki.gnome.org/Projects/LibRsvg). On __Debian__ or __Ubuntu__ install [librsvg2-dev](https://packages.debian.org/testing/librsvg2-dev):

```
sudo apt-get install -y librsvg2-dev
```

On __Fedora__, __CentOS or RHEL__ we need [librsvg2-devel](https://src.fedoraproject.org/rpms/librsvg2):

```
sudo yum install librsvg2-devel
````

On __OS-X__ use [rsvg](https://github.com/Homebrew/homebrew-core/blob/master/Formula/librsvg.rb) from Homebrew:

```
brew install librsvg
```
