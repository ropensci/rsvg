# Lazyload magick so we can skip on cran
image_dim <- function(path){
  img <- magick::image_read(path)
  info <- magick::image_info(img)
  sprintf('%dx%d', info$width, info$height)
}

test_that("rendering to size works", {
  skip_on_cran()
  tmp <- tempfile(fileext = '.png')

  # Use natural size
  rsvg_png('au.svg', tmp)
  expect_equal(image_dim(tmp), '512x512')

  # Scale by width
  rsvg_png('au.svg', tmp, width = 256)
  expect_equal(image_dim(tmp), '256x256')

  # Scale by height
  rsvg_png('au.svg', tmp, height = 1024)
  expect_equal(image_dim(tmp), '1024x1024')

  # Force stretch
  rsvg_png('au.svg', tmp, width = 512, height = 1024)
  expect_equal(image_dim(tmp), '512x1024')
})
