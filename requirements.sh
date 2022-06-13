# Install everything required
sudo apt install -y curl sed grep tar htop git codeblocks wget bash coreutils make patch tar gzip binutils gcc g++ git subversion mercurial curl wget xz-utils nasm mtools ninja-build perl m4 texinfo groff gettext autopoint libexpat1-dev bison flex help2man libssl-dev gperf rsync xsltproc python3 python-is-python3 python3-pip python3-mako python3-xcbgen xorriso qemu-system-x86 nasm grub-pc-bin qemu-kvm qemu gawk bc libelf-dev

# Install VSCode
sudo apt -y install wget gpg
wget -qO- https://packages.microsoft.com/keys/microsoft.asc | gpg --dearmor > packages.microsoft.gpg
sudo install -o root -g root -m 644 packages.microsoft.gpg /etc/apt/trusted.gpg.d/
sudo sh -c 'echo "deb [arch=amd64,arm64,armhf signed-by=/etc/apt/trusted.gpg.d/packages.microsoft.gpg] https://packages.microsoft.com/repos/code stable main" > /etc/apt/sources.list.d/vscode.list'
rm -f packages.microsoft.gpg
sudo apt-get install apt-transport-https
sudo apt update
sudo apt install code
