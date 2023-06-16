RELEASE_NAME := $(shell git tag --sort=creatordate | tail -n 1 | head -c -1)
CURRENT_SRC_PATH := $(shell pwd)

translations:
	@# Create/update ts files
	lupdate multiupv2.pro
	@# Start Linguist to do the translation work for english
	linguist *.ts
	@# Compile the .qm file
	lrelease multiupv2.pro

build:
	mkdir -p ./build

poc:
	mkdir -p ./poc

compile: build
	cd ./build && qmake ../multiupv2.pro && make -j

win_compile:
	# Compile outside sources for Windows...
	# Not possible to compile in $TEMP dir in MSYS env... (infinite loop of make ><)
	mkdir -p ../build && cd ../build && qmake $(CURRENT_SRC_PATH)/multiupv2.pro && make release -j

clean:
	rm -rf ./build
	rm *.tar.gz

deb:
	./make_deb_package.sh

gnulinux_archive: compile poc
	@echo Build archive for $(RELEASE_NAME)
	tar cjvf poc/multiup-manager_gnulinux_$(RELEASE_NAME).tar.gz ./build/multiup

win_archive: win_compile poc
	@echo Build archive for $(RELEASE_NAME)
	tar cjvf poc/multiup-manager_x86_$(RELEASE_NAME).tar.gz ../build/release/multiup.exe

release: gnulinux_archive deb
