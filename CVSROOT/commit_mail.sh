#!/bin/sh
rm -f /tmp/kd_log.txt
echo "" > /tmp/kd_log.txt
echo %{sVv} >> /tmp/kd_log.txt
date >> /tmp/kd_log.txt
echo "" >> /tmp/kd_log.txt
cat >> /tmp/kd_log.txt
echo "" >> /tmp/kd_log.txt
echo "--\n${USER}" >> /tmp/kd_log.txt
/bin/mail -s "[KrystalDrop] Commit" krys@users.sourceforge.net < /tmp/kd_log.txt
rm -f /tmp/kd_log.txt

