Multiup MaNaGeR is a software written in Qt/C++ that can send files to the multiupload website : [multiup.org](https://multiup.org/).

The operation is simple: You upload a file on our server, this one is uploaded automatically,
on several hosting-file websites. At the end, you get a link like:
`https://www.multiup.org/download/RANDOM_NUMBERS/FILE_NAME`,
where you will be able to see and download your file on various hosting-file websites.

<img alt="screenshot" src="./doc/prez.png" width="800" />

# How to use it ?

You should have Qt 5 libraries installed on your system.
Releases that contain all dependencies are available [here](https://github.com/ysard/MultiupMaNaGeR/releases).

Then, just execute `multiup` binary.

Simple command line options are available:

    $ multiup --login [account_name] --password [password] --read [files]

If you are more insterested by a full command line program, take a look at
[multiup-cli](https://github.com/ysard/multiup-cli).

You can configure your multiup.org account and get your links online.

For the translations, have a look at
[Translations](https://github.com/ysard/MultiupMaNaGeR#translations) chapter.

Settings of the application, once saved, are stored in a `config.ini` file, which is on the same
directory as the executable on Windows, and in `~/Multiupv2/` directory on GNU/Linux.


# Windows build

The project is based on Qt 5.12.1 (at least, but should work starting 5.10).

It is VERY strongly recommended to build the application in a stable environment
like MSYS2, and to use the same compiler for all parts of the project (qt, libcurl,
openssl). If you do not follow these recommendations, hell will come your way.

**Note about static linking**:
Note that we compile the application in static mode. You are free to use a shared
version which will greatly simplify the process.
Static mode compilations are not for the faint of heart and you could lose many
days of your life :p
MSYS2 provides Qt static binaries which saves you from having to compile the framework
yourself (believe me you don't want to do that).

Some doc here:

- https://wiki.qt.io/Compiling_OpenSSL_with_MinGW
- https://wiki.qt.io/MinGW-64-bit


`libcurl` is the only dependency, that must be compiled before Multiup MaNaGeR.
You should definitely read the insructions about it:

- <https://curl.se/docs/install.html>
- [Build with vcpkg](https://github.com/curl/curl/blob/master/docs/INSTALL.md#building-using-vcpkg)
- [libcurl build on Windows](https://github.com/curl/curl/blob/master/docs/INSTALL.md#windows)

The packages of libcurl are here: https://curl.se/download.html.
Currently, the versions 7.29 up to 7.88.1 has been tested successfully.

Minimal required MSYS2 packages for the whole project:

    # Qt specific version was taken here: https://repo.msys2.org/mingw/mingw64/
    pacman -U mingw-w64-x86_64-qt5-static-5.15.2-4-any.pkg.tar.zst
    pacman -S make
    pacman -S mingw-w64-x86_64-toolchain


The build command of libcurl was:

    ./configure --disable-shared --enable-static --with-openssl --without-winssl --without-cyassl \
    --without-gnutls --without-mbedtls --without-axtls --without-libpsl --without-libssh2 \
    --without-librtmp --without-winidn --without-libidn2 --without-nghttp2 --disable-ldap \
    --disable-ldaps --disable-unix-sockets --disable-manual --disable-gopher --disable-smtp \
    --disable-smb --disable-imap --disable-pop3 --disable-tftp --disable-ftp --disable-telnet \
    --disable-rtsp --disable-dict --disable-mqtt --disable-ntlm --disable-ntlm-wb --disable-tls-srp \
    --without-brotli
    make
    # I personally avoid 'make install' to avoid conflicts with libcurl installed in msys environment...


At this point you should have a `libcurl.a` file which is required for static linking.
Now, edit the file `multiupv2.pro` to point to the folders containing the headers and the lib.


Then compile the project itself:

- Via Qt Creator:
Just load the multiupv2.pro in the IDE Qt Creator as a new project.

- Manually:

```
    # If required, update your path:
    #set PATH=%PATH%;<path_to_QtSDK>\mingw\bin;<path_to_QtSDK>\Desktop\Qt\<Qt_version>\mingw\bin
    #or in msys2:
    #export PATH="/e/msys64/mingw64/bin:/e/msys64/mingw64/qt5-static/bin/:$PATH"

    mkdir build
    cd build
    qmake ../multiupv2.pro
    make release
```

Note: Any help to simplify/update this process is appreciated !

# GNU/Linux build

## Compilation

The project is based on Qt 5.12.1 (at least, but should work starting 5.10).

Other requirements for Debian like systems: `libcurl4-gnutls-dev` or `libcurl4-openssl-dev`

- Via Qt Creator:
Just load the multiupv2.pro in the IDE Qt Creator as a new project.

- Manually:

```
    mkdir build
    cd build
    qmake ../multiupv2.pro
    make
```

## Build Debian package

Take the release tarball (Ex: `multiup-manager_0.4.5.orig.tar.gz`),
then decompress it to the build directory:

    mkdir build
    tar -xf multiup-manager_0.4.5.orig.tar.gz -C build/
    cd build
    dpkg-buildpackage -us -uc -rfakeroot

Get & install your .deb file:

    dpkg -i ../multiup-manager_0.4.5-1_amd64.deb

You may have to change/delete the `override_dh_shlibdeps` directive of `debian/rules`
according to your installation of Qt framework (installed from system repositories
(remove it) or manually installed (update it)).

# Translations

For now, French (default), English and Spanish languages are supported.

Note : Anly help about this is appreciated !

Strings are translated using the Qt tools like lupdate, linguist & lrelease.

To update locales and translate them, in the current folder, execute :

    make translations

Feel free to adapt these directives to support a new language.


# License

[GPLv3](./LICENSE).
