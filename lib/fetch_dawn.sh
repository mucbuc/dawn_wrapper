#!/usr/bin/env bash


if [ ! -d "dawn" ]; then
  git clone --depth 1 --branch chromium/7780 https://dawn.googlesource.com/dawn
fi