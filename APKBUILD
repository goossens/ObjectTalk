# Contributor: Johan Goossens
# Maintainer: Johan Goossens
pkgname="ot"
pkgver="1.0"
pkgrel=0
pkgdesc="ObjectTalk - The Completely Object-Oriented Scripting Language"
url=""
arch="all"
license="Apache-2.0"
depends=""
makedepends="cmake libuv-dev"
install=""
subpackages="$pkgname-dev $pkgname-doc"
source=""
builddir="$srcdir/"

build() {
	cd "$startdir"/build

	cmake .. \
		-DCMAKE_INSTALL_PREFIX=/usr \
		-DCMAKE_INSTALL_LIBDIR=lib \
		-DBUILD_SHARED_LIBS=True \
		-DCMAKE_BUILD_TYPE=Release \
		-DCMAKE_CXX_FLAGS="$CXXFLAGS" \
		-DCMAKE_C_FLAGS="$CFLAGS"

	make
}

check() {
	make check
}

package() {
	cd "$startdir"/build
	make DESTDIR="$pkgdir" install
}

