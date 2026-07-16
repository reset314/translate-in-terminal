#!/bin/sh
curl -LO https://github.com/reset314/translate-in-terminal/releases/latest/download/translate-linux-x64.tar.gz
tar -xzf translate-linux-x64.tar.gz
cp translate /usr/local/bin/
mkdir -p /usr/local/bin/data
cp data/*.bin /usr/local/bin/data/
rm -rf translate translate-linux-x64.tar.gz data
echo "translate 安装完成"
