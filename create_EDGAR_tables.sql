-- this will create tables and related structures for
-- the EDGAR database which is extracted from the files
-- downloaded from the SEC EDGAR FTP server.

DROP TABLE IF EXISTS xbrl_extracts.edgar_filing_id CASCADE ;

CREATE TABLE xbrl_extracts.edgar_filing_id
(
	filing_ID SERIAL UNIQUE,
	cik TEXT NOT NULL,
	company_name TEXT NOT NULL,
	file_name TEXT NOT NULL,
	symbol TEXT ,
    sic TEXT NOT NULL,
	form_type TEXT NOT NULL,
	date_filed DATE NOT NULL,
	period_ending DATE NOT NULL,
	period_context_ID TEXT NOT NULL,
    shares_outstanding NUMERIC DEFAULT 0,
	UNIQUE(cik, form_type, period_ending),
    PRIMARY KEY(cik, form_type, period_ending)
);

ALTER TABLE xbrl_extracts.edgar_filing_id OWNER TO edgar_pg;


DROP TABLE IF EXISTS xbrl_extracts.edgar_filing_data ;

-- this definition is just a dummy placeholder for now.
-- the main thing was to get the foreign key stuff in plance.

CREATE TABLE xbrl_extracts.edgar_filing_data
(
	filing_data_ID SERIAL UNIQUE,
	filing_ID integer REFERENCES xbrl_extracts.edgar_filing_id (filing_ID) ON DELETE CASCADE,
	xbrl_label TEXT NOT NULL,
	user_label TEXT,
    xbrl_value TEXT NOT NULL,
	context_ID TEXT NOT NULL,
	period_begin DATE NOT NULL,
	period_end DATE NOT NULL,
	units TEXT NOT NULL,
	decimals TEXT,
	tsv TSVECTOR,
	PRIMARY KEY(filing_data_ID)
);

ALTER TABLE xbrl_extracts.edgar_filing_data OWNER TO edgar_pg;

DROP TRIGGER IF EXISTS tsv_update ON xbrl_extracts.edgar_filing_data ;

CREATE TRIGGER tsv_update
	BEFORE INSERT OR UPDATE ON xbrl_extracts.edgar_filing_data FOR EACH ROW
	EXECUTE PROCEDURE
		tsvector_update_trigger(tsv, 'pg_catalog.english', user_label);

DROP INDEX IF EXISTS idx_field_label ;

CREATE INDEX idx_field_label ON xbrl_extracts.edgar_filing_data USING GIN (tsv);
