# TRANSLATE-IN-TERMINAL
## 项目概述
一个在终端中的翻译软件（字典映射）
## 实现
将英语字典按照前两字符分片存储为bin文件，
线性遍历所有word根据levenshtein计算相似度然后返回
## 使用
编译打包后输出文件output
直接
output <word>
如果word不和法那么会计算相似度最高的10个词语然后输出
如果合法那么输出释义
## 注意
因为使用了剪枝，输出会剪掉levenshtein步数过多的word可以修改变量修改该剪枝界限
分片脚本为./fp.py分片的词典为ecdict.csv可以修改分片使得修改翻译语言等目标
合法时目前只输出释义没有输出变形，词性，音标等可以修改，变形的存储还是遵循字典项目需要处理后输出
## 构建发布版
```bash
g++ -std=c++17 -O2 main.cpp ShardLoader.cpp -o translate
tar czf translate-linux-x64.tar.gz translate data/
```
## About
英语字典项目地址：
https://github.com/skywind3000/ECDICT