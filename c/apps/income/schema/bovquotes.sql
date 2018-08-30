CREATE TABLE `bovespa_quotes` (
  tipreg INTEGER,
  datapreg DATE,
  coddbi VARCHAR(2),
  codneg VARCHAR(12),
  tpmerc INTEGER,
  nomres VARCHAR(12),
  especi VARCHAR(10),
  prazot VARCHAR(3),
  modref VARCHAR(4),
  preabe DECIMAL(13,2),
  premax DECIMAL(13,2),
  premin DECIMAL(13,2),
  premed DECIMAL(13,2),
  preult DECIMAL(13,2),
  preofc DECIMAL(13,2),
  preofv DECIMAL(13,2),
  totneg INTEGER,
  quatot BIGINT,
  voltot DECIMAL(18,2),
  preexe DECIMAL(13,2),
  indopc INTEGER,
  datven DATE,
  fatcot INTEGER,
  ptoexe DECIMAL(13,6),
  codisi VARCHAR(12),
  dismes INTEGER,

  PRIMARY KEY(tpmerc, coddbi, codneg, datapreg, prazot)
);
