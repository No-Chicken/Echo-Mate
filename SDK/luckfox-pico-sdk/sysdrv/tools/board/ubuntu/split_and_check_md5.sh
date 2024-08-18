#!/bin/bash

# Split a folder into N packages of 50M size
function split_folder() {
    folder_path="$1"
    split_size="50M"
    num_files="$(du -sh "$folder_path" | awk '{print $1/50}')"

    echo "Calculating md5 for each split package..."
    md5sum "$folder_path" > "${folder_path}.md5"

    rm -rf "$folder_path".split.*

    echo "Splitting folder $folder_path into $num_files packages of size $split_size..."
    #  split -b "$split_size" "$folder_path" "$folder_path".split.
    split -b "$split_size" -d -a 1 "$folder_path" "$folder_path".split.

    echo "Split and md5 calculation completed."
}

# Merge a folder from N packages
function merge_folder() {
    folder_path="$1"
    num_files="$(ls | wc -l)"

    rm -rf "$folder_path"

    echo "Merging $num_files packages into $folder_path..."
    cat "${folder_path}".split.* > "$folder_path"

    #  echo "Calculating md5 for merged package..."
    md5sum "$folder_path" > "${folder_path}.n.md5"
    current_md5=$(cat "${folder_path}.n.md5")

    echo "Merge and md5 calculation completed."
    # check md5
    original_md5=$(cat "${folder_path}.md5")
    if [ "$original_md5" == "$current_md5" ]; then
        echo "MD5 check passed."
    else
        echo "MD5 check failed."
        rm -rf "$folder_path"
    fi
}

# Process arguments
case "$1" in
    "split")
        split_folder "${@:2}"
        ;;
    "merge")
        merge_folder "${@:2}"
        ;;
    *)
        echo "Usage: $0 [split|merge] <file_path>"
        exit 1
        ;;
esac