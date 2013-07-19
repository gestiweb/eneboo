#!/bin/bash

svn up
svn -r $(cat last_merged_version):HEAD merge http://svn.infosial-dmz.lan/abanq/motor/2.5/developer/src
svn commit -m ""
svn up
echo $(svnversion -n) > last_merged_version
svn commit -m ""

