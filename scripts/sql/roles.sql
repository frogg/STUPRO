--
-- PostgreSQL database cluster dump
--

SET default_transaction_read_only = off;

SET client_encoding = 'SQL_ASCII';
SET standard_conforming_strings = on;

--
-- Roles
--

CREATE ROLE "stuproUser";
ALTER ROLE "stuproUser" WITH NOSUPERUSER NOINHERIT NOCREATEROLE CREATEDB LOGIN NOREPLICATION PASSWORD 'md5d66bc960198e7fefe80a1e18c8f810b0';




--
-- PostgreSQL database cluster dump complete
--
