#!/usr/bin/env sh

# Smells like runscript from CS246

failed=0

# absolute path to directory containing the config file
path_prefix=$(realpath $(dirname "${2}"))
# other test files are relative to $path_prefix

while read filename; do
    OUTFILE=$(mktemp)
    STEM=${filename%.*}
    EXPECTFILE="$path_prefix/expect/${STEM}.expect"
    if [ "${3}" = "racket" ]; then
        "${1}" "${path_prefix}/${filename}" > "${OUTFILE}" 2>&1
    else
        "${1}" < "${path_prefix}/${filename}" > "${OUTFILE}" 2>&1
    fi

    cmp "${OUTFILE}" "${EXPECTFILE}"
    if [ "${?}" -ne 0 ]; then
        echo "Test ${STEM} failed!"
        diff -y "${OUTFILE}" "${EXPECTFILE}"
        failed=1
    fi

    rm "${OUTFILE}"
done < "${2}"

exit ${failed}
