# rsvg

##### *A New Format for Lossless and Lossy Image Compression*

[![Build Status](https://travis-ci.org/jeroenooms/rsvg.svg?branch=master)](https://travis-ci.org/jeroenooms/rsvg)
[![AppVeyor Build Status](https://ci.appveyor.com/api/projects/status/github/jeroenooms/rsvg?branch=master&svg=true)](https://ci.appveyor.com/project/jeroenooms/rsvg)
[![Coverage Status](https://codecov.io/github/jeroenooms/rsvg/coverage.svg?branch=master)](https://codecov.io/github/jeroenooms/rsvg?branch=master)
[![CRAN_Status_Badge](http://www.r-pkg.org/badges/version/rsvg)](http://cran.r-project.org/package=rsvg)
[![CRAN RStudio mirror downloads](http://cranlogs.r-pkg.org/badges/rsvg)](http://cran.r-project.org/web/packages/rsvg/index.html)
[![Github Stars](https://img.shields.io/github/stars/jeroenooms/rsvg.svg?style=social&label=Github)](https://github.com/jeroenooms/rsvg)

> Renders vector-based 'svg' images into high-quality custom-size bitmap
  arrays using 'librsvg'. The resulting bitmap can be written to e.g. 'png', 'jpeg'
  or 'webp' format. In addition, the package can convert images directly to various
  formats such as pdf or postscript.

## Documentation

About the R package:

 - Blog post: [The 'rsvg' Package: High Quality Image Rendering in R](https://www.opencpu.org/posts/svg-release/)

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

Installation from source on Linux or OSX requires [`librsvg2`](https://developer.gnome.org/rsvg/). On __Debian__ or __Ubuntu__ install [librsvg2-dev](https://packages.debian.org/testing/librsvg2-dev):

```
sudo apt-get install -y librsvg2-dev
```

On __Fedora__, __CentOS or RHEL__ we need [librsvg2-devel](https://apps.fedoraproject.org/packages/librsvg2-devel):

```
sudo yum install librsvg2-devel
````

On __OS-X__ use [rsvg](https://github.com/Homebrew/homebrew-core/blob/master/Formula/librsvg.rb) from Homebrew:

```
brew install librsvg
```
