#!/bin/sh

# connect to the postgres database using the postgres user
# and initialize the roles needed for the next step
psql -d postgres -U postgres -f "./roles.sql"

# load the sql dump containing all kronos related tables
psql --username=stuproUser --dbname=template1 --file="./kronos.sql"
