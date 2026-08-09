#!/bin/bash
for f in src/esp32_nat_router.c src/urihandler/statichandler.c src/urihandler/portmaphandler.c src/urihandler/scanhandler.c src/urihandler/applyhandler.c components/cmd_router/cmd_router.c; do
  echo "Checking $f"
  grep "getDefaultIPByNetmask" $f
done
