# Build against static libraries from rwinlib
VERSION <- commandArgs(TRUE)
if(!file.exists(sprintf("../windows/rsvg-%s/include/librsvg-2.0/librsvg/rsvg.h", VERSION))){
  if(getRversion() < "3.3.0") setInternet2()
  download.file(sprintf("https://github.com/rwinlib/rsvg/archive/%s.zip", VERSION), "lib.zip", quiet = TRUE)
  dir.create("../windows", showWarnings = FALSE)
  unzip("lib.zip", exdir = "../windows")
  unlink("lib.zip")
}
