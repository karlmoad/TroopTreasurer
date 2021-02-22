CREATE TABLE IF NOT EXISTS `DEPOSITS` (
`DEPOSIT_KEY` VARCHAR(40) NOT NULL,
`DEPOSIT_DATE` DATE NOT NULL,
`DEPOSIT_TOTAL` DOUBLE(11,2) NOT NULL,
`FINALIZED` INT(11) NOT NULL DEFAULT '0',
`FINALIZED_DATE` DATE NULL DEFAULT '9999-12-31',
`RECONCILED` INT(11) NOT NULL DEFAULT '0',
PRIMARY KEY (`DEPOSIT_KEY`))
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;