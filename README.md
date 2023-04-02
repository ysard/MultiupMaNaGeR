Multiup MaNaGeR is a software written in Qt/C++ that can send files to the multiupload website : [multiup.org](https://multiup.org/).

The operation is simple: You upload a file on our server, this one is uploaded automatically,
on several hosting-file websites. At the end, you get a link like:
<code>https://www.multiup.org/download/RANDOM_NUMBERS/FILE_NAME</code>,
where you will be able to see and download your file on various hosting-file websites.

<img alt="Website screenshot" src="./doc/prez.png" width="800" />

# Build

## Compilation

The project is based on Qt 5.12.1.

Other requirements for Debian like systems: libcurl4-gnutls-dev or libcurl4-openssl-dev

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
according to your installation of Qt framework (installed from system repositories or manually installed).

## Translations

Strings are translated using the Qt tools like lupdate, linguist & lrelease.

To update locales and translate them, in the current folder, execute :

    make translations

Feel free to adapt these directives to support a new language.


# License

[GPLv3](./LICENSE).
