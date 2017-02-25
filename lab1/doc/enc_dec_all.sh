#!/bin/bash

key=purplemonkeydishwasher
src_name=Lecture02.pdf
outdir=output
src=../../$src_name
modes="cbc cfb ctr ecb ofb"

checksums_file=$outdir/checksums.md5sum
md5sum $src > $checksums_file
for mode in $modes ; do
    enc_tgt="$outdir/Lecture02_${mode}_encrypted.base64"
    dec_tgt="$outdir/Lecture02_${mode}_decrypted.pdf"

    TIMEFORMAT='%3R'; { time openssl enc --in $src --out $enc_tgt --base64 --nosalt -e --aes-256-$mode -k $key; } 2>&1 | tee "$enc_tgt.time"
    md5sum $enc_tgt >> $checksums_file
    TIMEFORMAT='%3R'; { time openssl enc --in $enc_tgt --out $dec_tgt --base64 --nosalt -d --aes-256-$mode -k $key; } 2>&1 | tee "$dec_tgt.time"
    md5sum $dec_tgt >> $checksums_file
done
