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
        target=i686-elf
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
toolchain_share=$toolchain_prefix/share
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

# === Building Bootloader ===
if [[ $profile == "i686-pc" ]]; then
    if [[ ! -f $toolchain_prefix/bin/limine-install ]]; then
        pushd `pwd`

        echo "Building bootloader"
        limine_src_dir=$toolchain_dir/limine

        cd $limine_src_dir
        time make -j$((`nproc`*2))

        make install PREFIX=$toolchain_prefix

        # Limeine

        popd
    else
        echo "limine-install exists - continuing"
    fi
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


if [[ $profile == "i686-pc" ]]; then
    echo "Creating Drive"

    smirk_drive_file=$build_dir/drive.hdd
    smirk_iso_dir=$build_dir/smirk_iso
    smirk_iso=$build_dir/smirk.iso

    mkdir $smirk_iso_dir

    cp $smirk_build_dir/kernel.elf $toolchain_dir/limine.cfg $toolchain_share/limine/limine.sys \
       $toolchain_share/limine/limine-cd.bin $toolchain_share/limine/limine-eltorito-efi.bin $smirk_iso_dir

    xorriso -as mkisofs -b limine-cd.bin \
            -no-emul-boot -boot-load-size 4 -boot-info-table \
            --efi-boot limine-eltorito-efi.bin \
            -efi-boot-part --efi-boot-image --protective-msdos-label \
            $smirk_iso_dir -o $smirk_iso

    limine-install $smirk_iso
fi

