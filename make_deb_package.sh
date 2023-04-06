#!/bin/bash
current_folder=$(pwd)

tag=$(git tag | tail -n 1 | head -c -1)
# Remove suffix -dev for tarball...
arr=(${tag//-/ })
short_tag=${arr[0]}

release_name="multiup-manager_$short_tag"
release_fullname="multiup-manager_${tag}"
git archive HEAD | gzip > ${release_name}.orig.tar.gz

cp ${release_name}.orig.tar.gz /tmp

cd /tmp
rm -rf ${release_fullname}
mkdir ${release_fullname}
tar xvf ${release_name}.orig.tar.gz -C ${release_fullname}/
cd ${release_fullname}/
dpkg-buildpackage -us -uc -rfakeroot


lintian -EvIL +pedantic ../${release_fullname}_amd64.deb

cd $current_folder
mkdir -p poc
mv /tmp/${release_fullname}_amd64.deb ./poc/
