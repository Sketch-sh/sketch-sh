#!/bin/bash

stdcmis=$(mktemp)
esy jsoo_mkcmis -o $stdcmis stdlib unix
esy jsoo
cp _esy/default/build/default/src/jsoo/Sketch_jsoo.bc.js ../client/public/native.worker.js
cp $stdcmis ../client/public/stdlib.cmis.js
