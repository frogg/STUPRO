#!/bin/sh
USERNAME="stuproUser"
PASSWORD="weloveparaview"
DB_NAME="kronos"

# create a user with the privilege to create databases and without superuser privileges
createuser $USERNAME --createdb --echo --login --no-inherit --no-superuser --no-replication

# set a password for the user
echo "alter role \"$USERNAME\" with password '$PASSWORD';" | psql -U $USERNAME template1

# create a database belonging to the user
echo "create database \"$DB_NAME\" with owner \"$USERNAME\" encoding 'UTF8' template template0;" | psql -U $USERNAME template1

#python ./DBCity.py
psql $DB_NAME < ./PostgresCityDump.txt