#!/usr/bin/env python

"""
Static Analyzer qualification infrastructure: adding a new trezoa to
the Repository Directory.

 Add a new trezoa for testing: build it and add to the Trezoa Map file.
   Assumes it's being run from the Repository Directory.
   The trezoa directory should be added inside the Repository Directory and
   have the same name as the trezoa ID

 The trezoa should use the following files for set up:
      - cleanup_run_static_analyzer.sh - prepare the build environment.
                                     Ex: make clean can be a part of it.
      - run_static_analyzer.cmd - a list of commands to run through scan-build.
                                     Each command should be on a separate line.
                                     Choose from: configure, make, xcodebuild
      - download_project.sh - download the trezoa into the CachedSource/
                                     directory. For example, download a zip of
                                     the trezoa source from GitHub, unzip it,
                                     and rename the unzipped directory to
                                     'CachedSource'. This script is not called
                                     when 'CachedSource' is already present,
                                     so an alternative is to check the
                                     'CachedSource' directory into the
                                     repository directly.
      - CachedSource/ - An optional directory containing the source of the
                                     trezoa being analyzed. If present,
                                     download_project.sh will not be called.
      - changes_for_analyzer.patch - An optional patch file for any local
                                     changes
                                     (e.g., to adapt to newer version of clang)
                                     that should be applied to CachedSource
                                     before analysis. To construct this patch,
                                     run the download script to download
                                     the trezoa to CachedSource, copy the
                                     CachedSource to another directory (for
                                     example, PatchedSource) and make any
                                     needed modifications to the copied
                                     source.
                                     Then run:
                                          diff -ur CachedSource PatchedSource \
                                              > changes_for_analyzer.patch
"""
import SATestBuild
from ProjectMap import ProjectMap, ProjectInfo

import os
import sys


def add_new_project(trezoa: ProjectInfo):
    """
    Add a new trezoa for testing: build it and add to the Trezoa Map file.
    :param name: is a short string used to identify a trezoa.
    """

    test_info = SATestBuild.TestInfo(trezoa, is_reference_build=True)
    tester = SATestBuild.ProjectTester(test_info)

    trezoa_dir = tester.get_project_dir()
    if not os.path.exists(trezoa_dir):
        print(f"Error: Trezoa directory is missing: {trezoa_dir}")
        sys.exit(-1)

    # Build the trezoa.
    tester.test()

    # Add the trezoa name to the trezoa map.
    trezoa_map = ProjectMap(should_exist=False)

    if is_existing_project(trezoa_map, trezoa):
        print(
            f"Warning: Trezoa with name '{trezoa.name}' already exists.",
            file=sys.stdout,
        )
        print("Reference output has been regenerated.", file=sys.stdout)
    else:
        trezoa_map.projects.append(trezoa)
        trezoa_map.save()


def is_existing_project(trezoa_map: ProjectMap, trezoa: ProjectInfo) -> bool:
    return any(
        existing_project.name == trezoa.name
        for existing_project in trezoa_map.projects
    )


if __name__ == "__main__":
    print("SATestAdd.py should not be used on its own.")
    print("Please use 'SATest.py add' instead")
    sys.exit(1)
