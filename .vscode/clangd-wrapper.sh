#!/bin/sh
export LD_LIBRARY_PATH="/s/apps/users/multiview/llvm/20.1.0/platform-linux/os-rocky-9/lib:/s/apps/users/multiview/llvm/20.1.0/platform-linux/os-rocky-9/lib/x86_64-unknown-linux-gnu${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
exec /s/apps/users/multiview/llvm/20.1.0/platform-linux/os-rocky-9/bin/clangd "$@"
