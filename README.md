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

Settings of the application are stored in a `config.ini` file, which is on the same
directory as the executable on Windows, and in `~/Multiupv2/` directory on GNU/Linux.

TODO: Static linking of Qt for Windows systems.


# Windows build

The project is based on Qt 5.12.1 (at least).

`libcurl` is the only dependency, and it have to be compiled before Multiup MaNaGeR.
You should definitely read the insructions about it:

- <https://curl.se/docs/install.html>
- [Build with vcpkg](https://github.com/curl/curl/blob/master/docs/INSTALL.md#building-using-vcpkg)
- [libcurl build on Windows](https://github.com/curl/curl/blob/master/docs/INSTALL.md#windows)

The packages of libcurl are here: https://curl.se/download.html.
Currently, the version 7.29 has been tested successfully.

The build command with Mingw32 was:

    cmake -G "MinGW Makefiles" .. \
        -DCURL_STATICLIB=ON -DHTTP_ONLY=ON -DBUILD_CURL_TESTS=OFF -DBUILD_CURL_EXE=OFF \
        -DCMAKE_BUILD_TYPE=Release -DCMAKE_USE_LIBSSH2=OFF -DCMAKE_USE_OPENSSL=OFF \
        -DENABLE_UNIX_SOCKETS=OFF -DCURL_ZLIB=OFF -DENABLE_MANUAL=OFF
    mingw32-make

At this point you should have a `libcurl.a` file which is required for static linking.
Now, edit the file `multiupv2.pro` to point to the folders containing the headers and the lib.


Then compile the project itself:

- Via Qt Creator:
Just load the multiupv2.pro in the IDE Qt Creator as a new project.

- Manually:

```
    # If required:
    set PATH=%PATH%;<path_to_QtSDK>\mingw\bin;<path_to_QtSDK>\Desktop\Qt\<Qt_version>\mingw\bin

    mkdir build
    cd build
    qmake ../multiupv2.pro
    mingw32-make release
```

Note: Any help to simplify/update this process is appreciated !

# GNU/Linux build

## Compilation

The project is based on Qt 5.12.1 (at least).

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
or manually installed).

# Translations

For now, French (default), English and Spanish languages are supported.

Note : Anly help about this is appreciated !

Strings are translated using the Qt tools like lupdate, linguist & lrelease.

To update locales and translate them, in the current folder, execute :

    make translations

Feel free to adapt these directives to support a new language.


# License

[GPLv3](./LICENSE).
