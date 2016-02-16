#!/bin/sh

# connect to the postgres database using the postgres user
# and initialize the roles needed for the next step
if [ ! -z $1 ]
then
   psql -d postgres -U $1 -f "./roles.sql"
else
   psql -d postgres -U postgres -f "./roles.sql"
fi

# load the sql dump containing all kronos related tables
PGPASSWORD=weloveparaview psql --username=stuproUser --host=localhost --dbname=template1 --file="./kronos.sql"
