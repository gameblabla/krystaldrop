#!/bin/sh
rm -f /tmp/kd_log.txt
echo -e -n "\n Commit from ${USER} on " > /tmp/kd_log.txt
date >> /tmp/kd_log.txt
echo "" >> /tmp/kd_log.txt
cat >> /tmp/kd_log.txt
echo -e "\n--\ncvs-${USER}" >> /tmp/kd_log.txt
/bin/mail -s "[KrystalDrop] Commit" krys@users.sourceforge.net < /tmp/kd_log.txt
rm -f /tmp/kd_log.txt

