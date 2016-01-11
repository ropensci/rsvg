class Glib < Formula
  homepage "https://developer.gnome.org/glib/"
  url "http://ftp.gnome.org/pub/gnome/sources/glib/2.42/glib-2.42.2.tar.xz"
  sha256 "a3cc1ebd2bd310a9fdf42ae4293ee713cdf1764bd29f552febf3bf44cadae7db"

  option :universal
  option "with-test", "Build a debug build and run tests. NOTE: Not all tests succeed yet"
  option "with-static", "Build glib with a static archive."

  deprecated_option "test" => "with-test"

  depends_on "pkg-config" => :build
  depends_on "gettext"
  depends_on "libffi"

  fails_with :llvm do
    build 2334
    cause "Undefined symbol errors while linking"
  end

  resource "config.h.ed" do
    url "https://trac.macports.org/export/111532/trunk/dports/devel/glib2/files/config.h.ed"
    version "111532"
    sha256 "9f1e23a084bc879880e589893c17f01a2f561e20835d6a6f08fcc1dad62388f1"
  end if build.universal?

  def install
    ENV.universal_binary if build.universal?


    # Disable dtrace; see https://trac.macports.org/ticket/30413
    args = %W[
      --disable-maintainer-mode
      --disable-dependency-tracking
      --disable-silent-rules
      --disable-dtrace
      --disable-libelf
      --enable-static
      --prefix=#{prefix}
      --localstatedir=#{var}
      --with-gio-module-dir=#{HOMEBREW_PREFIX}/lib/gio/modules
    ]

    system "./configure", *args

    if build.universal?
      buildpath.install resource("config.h.ed")
      system "ed", "-s", "-", "config.h", "<config.h.ed"
    end

    system "make"
    # the spawn-multithreaded tests require more open files
    system "ulimit", "-n", "1024;", "make", "check" if build.include? "test"
    system "make", "install"

    # `pkg-config --libs glib-2.0` includes -lintl, and gettext itself does not
    # have a pkgconfig file, so we add gettext lib and include paths here.
    gettext = Formula["gettext"].opt_prefix
    inreplace lib+"pkgconfig/glib-2.0.pc" do |s|
      s.gsub! "Libs: -L${libdir} -lglib-2.0 -lintl",
              "Libs: -L${libdir} -lglib-2.0 -L#{gettext}/lib -lintl"
      s.gsub! "Cflags: -I${includedir}/glib-2.0 -I${libdir}/glib-2.0/include",
              "Cflags: -I${includedir}/glib-2.0 -I${libdir}/glib-2.0/include -I#{gettext}/include"
    end

    (share+"gtk-doc").rmtree
  end

  test do
    (testpath/"test.c").write <<-EOS.undent
      #include <string.h>
      #include <glib.h>

      int main(void)
      {
          gchar *result_1, *result_2;
          char *str = "string";

          result_1 = g_convert(str, strlen(str), "ASCII", "UTF-8", NULL, NULL, NULL);
          result_2 = g_convert(result_1, strlen(result_1), "UTF-8", "ASCII", NULL, NULL, NULL);

          return (strcmp(str, result_2) == 0) ? 0 : 1;
      }
      EOS
    flags = ["-I#{include}/glib-2.0", "-I#{lib}/glib-2.0/include", "-lglib-2.0"]
    system ENV.cc, "-o", "test", "test.c", *(flags + ENV.cflags.to_s.split)
    system "./test"
  end
end
