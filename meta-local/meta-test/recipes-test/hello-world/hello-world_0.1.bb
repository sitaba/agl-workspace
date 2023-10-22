SUMMARY = "bitbake-layers recipe"
DESCRIPTION = "Simple helloworld application"
LICENSE = "MIT"

PR = "r0"



SRC_URI += " \
    file://hello_world.c \
"

#
#    file://myscripts.sh \
#

# https://stackoverflow.com/questions/70462089/how-to-get-lic-files-chksum-for-a-github-link
LICENSE = "CLOSED"
#LIC_FILES_CHKSUM = "file://files/LICENSE;md5=a98214854bb48b7f459739f4b51ed5d2"
S = "${WORKDIR}"

python do_display_banner() {
    bb.plain("***********************************************");
    bb.plain("*                                             *");
    bb.plain("*  Example recipe created by bitbake-layers   *");
    bb.plain("*                                             *");
    bb.plain("***********************************************");
}

# https://variwiki.com/index.php?title=Yocto_Hello_World
do_compile() {
    ${CC} hello_world.c ${LDFLAGS} -o hello_world
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 hello_world ${D}${bindir}
}
#    install -m 0755 myscripts.sh ${D}${bindir}

addtask display_banner before do_build
