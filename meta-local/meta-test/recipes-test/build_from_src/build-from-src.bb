SUMMARY = "bitbake-layers recipe"
DESCRIPTION = "build helloworld application from source"
LICENSE = "MIT"

SRC_URI += " \
    file://main.c \
"

LICENSE = "CLOSED"
S = "${WORKDIR}"

do_compile() {
    ${CC} main.c ${LDFLAGS} -o hello_from_src
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 hello_from_src ${D}${bindir}
}

