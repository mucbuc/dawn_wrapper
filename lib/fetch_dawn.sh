#!/usr/bin/env bash

#version bumper1 

if [ ! -d "dawn" ]; then
  git clone --depth 1 --branch chromium/7780 https://dawn.googlesource.com/dawn
  cd dawn
  python tools/fetch_dawn_dependencies.py
  rm -rf .git
  rm .gitignore
  rm -rf third_party/**/.git
fi