SUMMARY = "bitbake-layers recipe"
DESCRIPTION = "input control from cmd like android input tool"
LICENSE = "CLOSED"


SRC_URI += " \
    file://remote-input.c \
    file://remote-input.service \
	file://input.c \
"

inherit systemd

SYSTEMD_PACKAGES = "${PN}"
SYSTEMD_SERVICE:${PN} = "remote-input.service"

S = "${WORKDIR}"

do_compile() {
    ${CC} remote-input.c ${LDFLAGS} -o remote-input
    ${CC} input.c ${LDFLAGS} -o input
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 remote-input ${D}${bindir}
    install -m 0755 input ${D}${bindir}

    install -d ${D}${systemd_system_unitdir}
    install -m 0644 remote-input.service ${D}${systemd_system_unitdir}
}
