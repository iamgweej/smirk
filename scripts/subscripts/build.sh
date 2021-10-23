#!/usr/bin/env bash
set -x

smirk_dir="$(cd "$(dirname "$0")" >/dev/null 2>&1; cd ..; cd ..; pwd -P)"
toolchain_dir="$smirk_dir/toolchain"

# === Parse args ===

function usage {
    echo "usage: $0 <target>"
}

if [[ $# < 1 ]]; then
    usage $@
    exit 1
fi

target=""
profile=$1

case $profile in
    i686-pc)
        target=i686-none-elf
        ;;
    arm64-raspi3)
        target=aarch64-none-elf
        ;;
    *)
        usage $@
        exit 1
        ;;
esac

profile_file=$smirk_dir/profiles/$profile.ini

board=$(echo $profile | cut -d'-' -f2)

if [[ ! -f $profile_file ]]; then
    echo "Profile not found: $profile_file"
    exit 1
fi

# === Create build directory ===

build_dir="$smirk_dir/build-$profile"
if [[ ! -d $build_dir ]]; then mkdir $build_dir; fi

echo "Building '$profile' to '$build_dir'"


# === Build toolchain ===

toolchain_prefix=$build_dir/toolchain
export PATH="$toolchain_prefix/bin:$PATH"

if [[ ! -d  $toolchain_prefix ]]; then
    pushd `pwd`

    echo "Building toolchain"

    # Binutils
    binutils_src_dir=$toolchain_dir/binutils-gdb
    binutils_build_dir=$build_dir/binutils-gdb

    mkdir -p $binutils_build_dir
    cd $binutils_build_dir

    $binutils_src_dir/configure --target="$target" \
                                --prefix="$toolchain_prefix" \
                                --with-sysroot \
                                --disable-nls \
                                --disable-werror

    time make -j$((`nproc`*2))
    make install

    which -- $target-as || (echo $target-as is not in the PATH; exit 1)

    # Gcc
    gcc_src_dir=$toolchain_dir/gcc
    gcc_build_dir=$build_dir/gcc

    mkdir -p $gcc_build_dir
    cd $gcc_build_dir

    $gcc_src_dir/configure --target="$target" \
                           --prefix="$toolchain_prefix" \
                           --disable-nls \
                           --enable-languages=c \
                           --without-headers

    time make all-gcc -j$((`nproc`*2))
    time make all-target-libgcc -j$((`nproc`*2))

    make install-gcc
    make install-target-libgcc

    which -- $target-gcc || (echo $target-gcc is not in the PATH; exit 1)

    popd
fi

# === Build Smirk ===

echo "Building Smirk"

smirk_build_dir=$build_dir/smirk

if [[ ! -d $smirk_build_dir ]]; then
    meson setup $smirk_build_dir --cross-file $profile_file #-Dboard=$board
fi

meson compile -C $smirk_build_dir
if [[ $? != 0 ]]; then exit 1; fi

# === Make Disk ===


if [[ $profile == "amd64-pc" ]]; then
    echo "Creating Drive"
    smirk_drive_file=$build_dir/drive.hdd

    dd if=/dev/zero bs=1M count=64 of=$smirk_drive_file

    parted -s $smirk_drive_file mklabel msdos
    parted -s $smirk_drive_file mkpart primary 1 100%

    echfs-utils -m -p0 $smirk_drive_file quick-format 32768
    echfs-utils -m -p0 $smirk_drive_file import $smirk_build_dir/kernel.elf kernel.elf
    echfs-utils -m -p0 $smirk_drive_file import $smirk_dir/limine.cfg limine.cfg

    echfs-utils -m -p0 $smirk_drive_file import $toolchain_dir/limine/stage2.map stage2.map

    limine-install $smirk_drive_file
fi

