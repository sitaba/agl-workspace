SUMMARY = "bitbake-layers recipe"
DESCRIPTION = "build helloworld application from source"
LICENSE = "MIT"

inherit cmake

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI += " \
    file://main.c \
    file://CMakeLists.txt \
"

LICENSE = "CLOSED"
S = "${WORKDIR}"

#do_compile() {
#    ${CC} main.c ${LDFLAGS} -o hello_with_cmake
#}

#do_install() {
#    install -d ${D}${bindir}
#    install -m 0755 hello_with_cmake ${D}${bindir}
#}

