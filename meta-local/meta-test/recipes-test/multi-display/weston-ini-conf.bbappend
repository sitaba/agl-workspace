

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI += " \
    file://weston.ini \
"

do_install:append() {
    install -d ${D}${weston_ini_dir}
    install -m 0644 weston.ini ${D}${sysconfdir}/xdg/weston/weston.ini.default
}




