# Test rendering sizes and svg normalization
# Inspired by this issue: https://github.com/ropensci/rsvg/issues/41

image_dim <- function(path){
  img <- magick::image_read(path)
  info <- magick::image_info(img)
  sprintf('%dx%d', info$width, info$height)
}

image_similarity <- function(path1, path2){
  im1 <- magick::image_read(path1)
  im2 <- magick::image_read(path2)
  magick::image_compare_dist(im1, im2)$distortion
}

test_that("rendering to size works", {
  skip_on_cran()
  tmp1 <- tempfile(fileext = '.png')
  tmp2 <- tempfile(fileext = '.png')
  tmp3 <- tempfile(fileext = '.png')
  orig <- 'au.svg'
  normalized <- rsvg_svg(orig)

  # Use natural size
  rsvg_png(orig, tmp1)
  rsvg_png(normalized, tmp2)
  png::writePNG(rsvg::rsvg_nativeraster(orig), tmp3)
  expect_equal(image_dim(tmp1), '512x512')
  expect_equal(image_dim(tmp2), '512x512')
  expect_equal(image_dim(tmp3), '512x512')
  expect_gt(image_similarity(tmp1, tmp2), 0.99)
  expect_gt(image_similarity(tmp1, tmp3), 0.99)

  # Scale by width
  rsvg_png(orig, tmp1, width = 256)
  rsvg_png(normalized, tmp2, width = 256)
  png::writePNG(rsvg::rsvg_nativeraster(orig, width = 256), tmp3)
  expect_equal(image_dim(tmp1), '256x256')
  expect_equal(image_dim(tmp2), '256x256')
  expect_equal(image_dim(tmp3), '256x256')
  expect_gt(image_similarity(tmp1, tmp2), 0.99)
  expect_gt(image_similarity(tmp1, tmp3), 0.99)

  # Scale by height
  rsvg_png(orig, tmp1, height = 1024)
  rsvg_png(normalized, tmp2, height = 1024)
  png::writePNG(rsvg::rsvg_nativeraster(orig, height = 1024), tmp3)
  expect_equal(image_dim(tmp1), '1024x1024')
  expect_equal(image_dim(tmp2), '1024x1024')
  expect_equal(image_dim(tmp3), '1024x1024')
  expect_gt(image_similarity(tmp1, tmp2), 0.99)
  expect_gt(image_similarity(tmp1, tmp3), 0.99)

  # Force stretch
  rsvg_png(orig, tmp1, width = 512, height = 1024)
  rsvg_png(normalized, tmp2, width = 512, height = 1024)
  png::writePNG(rsvg::rsvg_nativeraster(orig, width = 512, height = 1024), tmp3)
  expect_equal(image_dim(tmp1), '512x1024')
  expect_equal(image_dim(tmp2), '512x1024')
  expect_equal(image_dim(tmp3), '512x1024')
  expect_gt(image_similarity(tmp1, tmp2), 0.99)
  expect_gt(image_similarity(tmp1, tmp3), 0.99)
})
