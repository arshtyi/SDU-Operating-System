#!/bin/bash
source /etc/os-release
codename="${VERSION_CODENAME}"
mirror_uri="https://mirrors.sdu.edu.cn/ubuntu"

if [ -f /etc/apt/sources.list.d/ubuntu.sources ]; then
    sudo tee /etc/apt/sources.list.d/ubuntu.sources > /dev/null <<EOF
Types: deb
URIs: ${mirror_uri}
Suites: ${codename}
Components: main restricted universe multiverse
Signed-By: /usr/share/keyrings/ubuntu-archive-keyring.gpg

Types: deb
URIs: ${mirror_uri}
Suites: ${codename}-security
Components: main restricted universe multiverse
Signed-By: /usr/share/keyrings/ubuntu-archive-keyring.gpg

Types: deb
URIs: ${mirror_uri}
Suites: ${codename}-updates
Components: main restricted universe multiverse
Signed-By: /usr/share/keyrings/ubuntu-archive-keyring.gpg
EOF
    apt_source_mode="ubuntu.sources"
else
    sudo tee /etc/apt/sources.list > /dev/null <<EOF
deb ${mirror_uri} ${codename} main restricted universe multiverse
deb ${mirror_uri} ${codename}-security main restricted universe multiverse
deb ${mirror_uri} ${codename}-updates main restricted universe multiverse
EOF
    apt_source_mode="sources.list"
fi

sudo apt update

xmake_source="system"
if apt-cache policy xmake | grep -q "Candidate: (none)"; then
    sudo apt-add-repository -y ppa:xmake-io/xmake
    sudo apt update
    xmake_source="ppa"
fi

sudo apt install -y xmake gcc g++ build-essential ninja-build
xmake_version="$(xmake --version | head -n 1)"
gcc_version="$(gcc --version | head -n 1)"
gpp_version="$(g++ --version | head -n 1)"
make_version="$(make --version | head -n 1)"
ninja_version="$(ninja --version)"

color_title="\033[1;36m"
color_key="\033[1;33m"
color_value="\033[0;32m"
color_reset="\033[0m"

echo ""
printf "${color_title}================ Environment Summary ================${color_reset}\n"
printf "${color_key}apt mirror${color_reset}   : ${color_value}%s${color_reset}\n" "${mirror_uri}"
printf "${color_key}apt source file${color_reset}: ${color_value}%s${color_reset}\n" "${apt_source_mode}"
printf "${color_key}ubuntu codename${color_reset}: ${color_value}%s${color_reset}\n" "${codename}"
printf "${color_key}xmake source${color_reset} : ${color_value}%s${color_reset}\n" "${xmake_source}"
printf "${color_key}xmake version${color_reset}: ${color_value}%s${color_reset}\n" "${xmake_version}"
printf "${color_key}gcc version${color_reset}  : ${color_value}%s${color_reset}\n" "${gcc_version}"
printf "${color_key}g++ version${color_reset}  : ${color_value}%s${color_reset}\n" "${gpp_version}"
printf "${color_key}make version${color_reset} : ${color_value}%s${color_reset}\n" "${make_version}"
printf "${color_key}ninja version${color_reset}: ${color_value}%s${color_reset}\n" "${ninja_version}"
printf "${color_title}=====================================================${color_reset}\n"
