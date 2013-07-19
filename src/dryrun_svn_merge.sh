#!/bin/bash

svn up
svn --dry-run -r $(cat last_merged_version):HEAD merge http://svn.infosial-dmz.lan/abanq/motor/2.5/developer/src

