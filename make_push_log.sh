#!/bin/bash
cd ~/imgui-test
LOG=~/storage/downloads/push.log
> $LOG
echo "Date: $(date)" >> $LOG
git rev-parse HEAD >> $LOG
git show --stat HEAD >> $LOG
git show HEAD -- . ':!*.apk' ':!*.so' ':!*.a' ':!*.png' ':!*.jpg' ':!*.iso' >> $LOG
echo "--- FILE GEDE (ls only) ---" >> $LOG
find ~/imgui-test -type f -size +1M -not -path "*/.git/*" -not -name "android.jar" -exec ls -lh {} \; >> $LOG
echo "DONE" 
