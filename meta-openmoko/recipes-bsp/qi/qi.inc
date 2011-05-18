DESCRIPTION = "Qi bootloader w/ Openmoko GTA01, GTA02, and GTA03 support"
LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://Makefile;endline=15;md5=a6394710a271493a533a3724a1bce25d"
SECTION = "bootloader"
PROVIDES = "virtual/bootloader"
PV = "1.0.2"

EXTRA_OEMAKE = ""
PARALLEL_MAKE = ""

do_configure() {
	echo >config.mk
	# uncomment to debug
	sed -i s,@,, Makefile
	cat >config.mk <<EOF

BUILD_HOST=\$(shell hostname)
BUILD_VERSION=${PV}-${PR}
BUILD_DATE=\$(shell date --iso-8601=seconds)

# NOTE: The following depends on the fact that only one version of the cross-compiler
# has been built. If it bails out with something like: "| arm-linux-ld: 4.3.3: No such file or directory"
# then you need to remove all previously built toolchain directories out of the cross library path.
GCCV="`${CC} -dumpversion`"
COMPILER_LIB_PATH_PRE=${TOOLCHAIN_PATH}/lib/gcc/${TARGET_SYS}/$(GCCV)
COMPILER_LIB_PATH=\$(COMPILER_LIB_PATH_PRE)/\$(shell ls \$(COMPILER_LIB_PATH_PRE))

AS=${AS}
LD=${LD}
CC=${HOST_PREFIX}gcc
OBJCOPY=${OBJCOPY}
OBJDUMP=${OBJDUMP}
HOSTCC=${BUILD_CC}

export HOSTCC BUILD_HOST BUILD_VERSION BUILD_DATE CROSS_COMPILE AD LD CC OBJCOPY OBJDUMP MKUDFU
EOF	
}

do_compile() {
	make clean && \
	make CPU=s3c2442 && \
	make CPU=s3c2410 && \
	make CPU=s3c6410
}

do_compile_om-gta01() {
        make clean && make CPU=s3c2410
}
do_compile_om-gta02() {
        make clean && make CPU=s3c2442
}

inherit deploy

do_deploy () {
	cd image
	for IMG in qi-*.udfu; do
		IMG_LINK=`echo ${IMG} | sed s:-${PV}.*udfu:.udfu:g`
		ln -sf ${IMG} ${IMG_LINK}
	done
	install -d ${DEPLOY_DIR_IMAGE}
	cp -fa qi-*.udfu ${DEPLOY_DIR_IMAGE}
}

PACKAGE_ARCH = "${MACHINE_ARCH}"

addtask deploy before do_build after do_package