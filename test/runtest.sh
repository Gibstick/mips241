#!/usr/bin/env sh 

# Smells like runscript from CS246

failed=0

# absolute path to directory containing the config file
path_prefix=$(realpath $(dirname "${2}"))
# other test files are relative to $path_prefix

while read stem; do
    OUTFILE=$(mktemp)
    EXPECTFILE="$path_prefix/expect/${stem}.expect"

    "${1}" < "${path_prefix}/${stem}.mips" > "${OUTFILE}" 2>&1 

    cmp "${OUTFILE}" "${EXPECTFILE}"
    if [ "${?}" -ne 0 ]; then
        echo "Test ${stem} failed!"
        diff "${OUTFILE}" "${EXPECTFILE}"
        failed=1
    fi

    rm "${OUTFILE}"
done < "${2}"

exit ${failed}
