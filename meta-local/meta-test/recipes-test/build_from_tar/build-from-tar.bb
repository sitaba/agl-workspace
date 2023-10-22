SUMMARY = "bitbake-layers recipe"
DESCRIPTION = "build helloworld application from source"
LICENSE = "MIT"

SRC_URI += " \
    file://hello-world.tar.gz \
"

LICENSE = "CLOSED"
S = "${WORKDIR}"
S = "${WORKDIR}/hello-world"

do_compile() {
    ${CC} main.c ${LDFLAGS} -o hello_from_tar
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 hello_from_tar ${D}${bindir}
}

