#!/bin/bash

set -x -e

DBNAME=lecker
DBUSER=hannibal
DBPASS=19440807

mysql -u root -p < setup_db.sql
