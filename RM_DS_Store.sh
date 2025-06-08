#!/bin/bash

# 删除当前目录及其所有子目录中的 .DS_Store 文件
find . -name '.DS_Store' -type f -delete

echo "已删除所有 .DS_Store 文件。"
