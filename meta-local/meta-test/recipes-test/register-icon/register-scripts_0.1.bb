SUMMARY = "bitbake-layers recipe"
DESCRIPTION = "Simple helloworld application"
LICENSE = "MIT"

PR = "r0"

SRC_URI += " \
    file://myscripts.sh \
"

LICENSE = "CLOSED"
S = "${WORKDIR}"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install() {
    install -d ${D}${bindir}
    install -m 0755 myscripts.sh ${D}${bindir}
}

RDEPENDS:${PN} += "bash"
