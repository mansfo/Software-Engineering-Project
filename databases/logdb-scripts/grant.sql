\c :dbname postgres

-- user already exists
GRANT ALL PRIVILEGES ON DATABASE :dbname to :username ;

-- grant all privileges on all tables in schema public to :username ;
-- grant all privileges on all sequences in schema public to :username ;

ALTER TABLE LogTable OWNER TO :username;
ALTER TABLE PurchasesLogTable OWNER TO :username;
ALTER TABLE AccessLogTable OWNER TO :username;
ALTER TABLE OrderLogTable OWNER TO :username;

GRANT ALL ON SCHEMA public TO :username ;
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA public TO :username ;