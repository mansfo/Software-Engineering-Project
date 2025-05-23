\c :dbname postgres

-- user already exists
GRANT ALL PRIVILEGES ON DATABASE :dbname to :username ;

-- grant all privileges on all tables in schema public to :username ;
-- grant all privileges on all sequences in schema public to :username ;

ALTER TABLE Customer OWNER TO :username;
ALTER TABLE Supplier OWNER TO :username;
ALTER TABLE Company OWNER TO :username;
ALTER TABLE Carrier OWNER TO :username;
ALTER TABLE Product OWNER TO :username;
ALTER TABLE Orders OWNER TO :username;

GRANT ALL ON SCHEMA public TO :username ;
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA public TO :username ;