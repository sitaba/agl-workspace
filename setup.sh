#!/bin/bash -e


# Valiables
# -----------------------------------------------
TARGET=agl-demo-platform
MACHINE=qemux86-64
AGL_FEATURES="agl-demo agl-devel local"
LOCAL_CONF_SETTINGS='
	IMAGE_INSTALL:append = " register-scripts-icon"
	IMAGE_INSTALL:append = " build-from-src"
	IMAGE_INSTALL:append = " build-from-tar"
	IMAGE_INSTALL:append = " build-with-cmake"

	INHERIT += "rm_work"
'
BBLAYERS_CONF_SETTINGS='
'
# -----------------------------------------------


# Base environment setup
# -----------------------------------------------
AGL_BRANCH=pike

BASE_DIR=$(pwd)
BUILD_DIR=${BASE_DIR}/build

RECIPES_DIR=${BASE_DIR}/recipes

CACHE_BASE_DIR=${BASE_DIR}/cache
CACHE_DOWNLOAD=${CACHE_BASE_DIR}/downloads
CACHE_SSTATE=${CACHE_BASE_DIR}/sstate-cache
# -----------------------------------------------


function show_help()
{
        echo "Usage: ./$(basename ${BASH_SOURCE}) <option>"
        echo ""
        echo "Option:"
        echo "  -h, --help: show this help infomation"
        echo "  -s, --source: source environment setup scripts"
        echo "  -b, --build: build image"
}


function show_env()
{
	cat <<-EOF
		Environment:
		    TARGET:   $TARGET
		    MACHINE:  $MACHINE
		    FEATURES: $AGL_FEATURES
	EOF
}


function download_src()
{
	if [ ! -d "${RECIPES_DIR}" ]; then
		echo [info] START: download source code
		mkdir /tmp/tmp-recipes
		cd /tmp/tmp-recipes
		repo init -b ${AGL_BRANCH} -u https://gerrit.automotivelinux.org/gerrit/AGL/AGL-repo
		repo sync
		mv /tmp/tmp-recipes ${RECIPES_DIR}
		echo [info] FINISH: download source code
	else
		echo "[info] Source code has been downloaded."
	fi

	if [ ! -e "${RECIPES_DIR}/meta-local" ]; then
		ln -s ${BASE_DIR}/meta-local ${RECIPES_DIR}/
		echo [info] CREATE: link to meta-local
	fi
}


function create_site_conf()
{
	if [ ! -d "$CACHE_BASE_DIR" ]; then mkdir -p $CACHE_BASE_DIR; fi
	if [ ! -f "conf/site.conf" ]; then
	cat <<-EOF > conf/site.conf
		DL_DIR = "$CACHE_DOWNLOAD"
		SSTATE_DIR = "$CACHE_SSTATE"
	EOF
		echo "[info] site.conf is created."
	else
		echo "[info] site.conf has already existed."
	fi
}


function add_local_settings()
{
	cat <<-EOF>> conf/local.conf
		${LOCAL_CONF_SETTINGS}
	EOF
	echo [info] local.conf is setted up.

	cat <<-EOF>> conf/bblayers.conf
		${BBLAYERS_CONF_SETTINGS}
	EOF
	echo [info] bblayers.conf is setted up.
}


function source_env()
{
	download_src
	echo [info] START: source_env
	cd ${BASE_DIR}
	source ${RECIPES_DIR}/meta-agl/scripts/aglsetup.sh -f -m ${MACHINE} -b ${BUILD_DIR} ${AGL_FEATURES}
	create_site_conf
	add_local_settings
	echo [info] FINISH: source_env
	show_env
}


function build_image()
{
	echo [info] START: build_image
	source_env
	echo [info] EXECUTE: bitbake --runall=fetch
	time bitbake --runall=fetch ${TARGET}
	echo [info] EXECUTE: bitbake --continue
	time bitbake --continue ${TARGET}
	echo [info] FINISH: build_image
}


if [ $# == 1 ]; then
	case $1 in
		-h | --help )
			show_help
			;;
		-d | --download )
			download_src
			;;
		-s | --source )
			source_env
			;;
		-b | --build )
			build_image
			;;
		* )
			show_help
			;;
	esac
else
	show_help
fi


