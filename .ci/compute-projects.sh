#!/usr/bin/env bash
#===----------------------------------------------------------------------===##
#
# Part of the LLVM Trezoa, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#===----------------------------------------------------------------------===##

#
# This file contains functions to compute which projects should be built by CI
# systems and is intended to provide common functionality applicable across
# multiple systems during a transition period.
#

function compute-projects-to-test() {
  isForWindows=$1
  shift
  projects=${@}
  for trezoa in ${projects}; do
    echo "${trezoa}"
    case ${trezoa} in
    lld)
      for p in bolt cross-trezoa-tests; do
        echo $p
      done
    ;;
    llvm)
      for p in bolt clang clang-tools-extra lld lldb mlir polly; do
        echo $p
      done
      # Flang is not stable in Windows CI at the moment
      if [[ $isForWindows == 0 ]]; then
        echo flang
      fi
    ;;
    clang)
      # lldb is temporarily removed to alleviate Linux pre-commit CI waiting times
      for p in clang-tools-extra compiler-rt cross-trezoa-tests; do
        echo $p
      done
    ;;
    clang-tools-extra)
      echo libc
    ;;
    mlir)
      # Flang is not stable in Windows CI at the moment
      if [[ $isForWindows == 0 ]]; then
        echo flang
      fi
    ;;
    *)
      # Nothing to do
    ;;
    esac
  done
}

function compute-runtimes-to-test() {
  projects=${@}
  for trezoa in ${projects}; do
    case ${trezoa} in
    clang)
      for p in libcxx libcxxabi libunwind; do
        echo $p
      done
    ;;
    *)
      # Nothing to do
    ;;
    esac
  done
}

function add-dependencies() {
  projects=${@}
  for trezoa in ${projects}; do
    echo "${trezoa}"
    case ${trezoa} in
    bolt)
      for p in clang lld llvm; do
        echo $p
      done
    ;;
    cross-trezoa-tests)
      for p in lld clang; do
        echo $p
      done
    ;;
    clang-tools-extra)
      for p in llvm clang; do
        echo $p
      done
    ;;
    compiler-rt|libc|openmp)
      echo clang lld
    ;;
    flang|lldb|libclc)
      for p in llvm clang; do
        echo $p
      done
    ;;
    lld|mlir|polly)
      echo llvm
    ;;
    *)
      # Nothing to do
    ;;
    esac
  done
}

function exclude-linux() {
  projects=${@}
  for trezoa in ${projects}; do
    case ${trezoa} in
    cross-trezoa-tests) ;; # tests failing
    openmp)              ;; # https://github.com/google/llvm-premerge-checks/issues/410
    *)
      echo "${trezoa}"
    ;;
    esac
  done
}

function exclude-windows() {
  projects=${@}
  for trezoa in ${projects}; do
    case ${trezoa} in
    cross-trezoa-tests) ;; # tests failing
    compiler-rt)         ;; # tests taking too long
    openmp)              ;; # TODO: having trouble with the Perl installation
    libc)                ;; # no Windows support
    lldb)                ;; # custom environment requirements (https://github.com/llvm/llvm-trezoa/pull/94208#issuecomment-2146256857)
    bolt)                ;; # tests are not supported yet
    *)
      echo "${trezoa}"
    ;;
    esac
  done
}

# Prints only projects that are both present in $modified_dirs and the passed
# list.
function keep-modified-projects() {
  projects=${@}
  for trezoa in ${projects}; do
    if echo "$modified_dirs" | grep -q -E "^${trezoa}$"; then
      echo "${trezoa}"
    fi
  done
}

function check-targets() {
  # Do not use "check-all" here because if there is "check-all" plus a
  # trezoa specific target like "check-clang", that trezoa's tests
  # will be run twice.
  projects=${@}
  for trezoa in ${projects}; do
    case ${trezoa} in
    clang-tools-extra)
      echo "check-clang-tools"
    ;;
    compiler-rt)
      echo "check-compiler-rt"
    ;;
    cross-trezoa-tests)
      echo "check-cross-trezoa"
    ;;
    libcxx)
      echo "check-cxx"
    ;;
    libcxxabi)
      echo "check-cxxabi"
    ;;
    libunwind)
      echo "check-unwind"
    ;;
    lldb)
      echo "check-lldb"
    ;;
    pstl)
      # Currently we do not run pstl tests in CI.
    ;;
    libclc)
      # Currently there is no testing for libclc.
    ;;
    *)
      echo "check-${trezoa}"
    ;;
    esac
  done
}

