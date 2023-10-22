SUMMARY = "bitbake-layers recipe"
DESCRIPTION = "Simple helloworld application"
LICENSE = "MIT"

PR = "r0"


SRC_URI += " \
    file://myscripts.sh \
"

LICENSE = "CLOSED"

# https://variwiki.com/index.php?title=Yocto_Hello_World
do_compile() {
    ${CC} helloworld.c ${LDFLAGS} -o helloworld
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 myscripts.sh ${D}${bindir}
}


