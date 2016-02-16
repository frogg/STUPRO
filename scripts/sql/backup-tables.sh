#!/bin/sh
pg_dump -U postgres --encoding=UTF8 --clean --create --format=plain --role="stuproUser" --file="./kronos.sql" kronos
