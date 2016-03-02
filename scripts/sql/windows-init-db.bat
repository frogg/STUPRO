createdb -U postgres kronos
psql -d postgres -U postgres -f roles.sql
psql -U stuproUser kronos < kronos.sql