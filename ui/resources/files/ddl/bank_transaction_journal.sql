CREATE
    VIEW `BANK_TRANSACTION_JOURNAL`
    AS SELECT `BD`.`ID` AS `ID`,
              `BD`.`UPLOAD_BATCH` AS `UPLOAD_BATCH`,
              `BD`.`RECORD_HASH` AS `RECORD_HASH`,
              `BD`.`TRANS_DATE` AS `TRANS_DATE`,
              `BD`.`TRANS_VALUE` AS `TRANS_VALUE`,
              `BD`.`CAT_NUM` AS `CAT_NUM`,
              `BD`.`CAT_NAME` AS `CAT_NAME`,
              `BD`.`REF_NUM` AS `REF_NUM`,
              `BD`.`DESC` AS `DESC`,
              `BD`.`CODE` AS `CODE`,
              `BD`.`IDENTIFIER` AS `IDENTIFIER`
    FROM `BANK_DATA` `BD`
    UNION ALL
    SELECT `BDA`.`ID` AS `ID`,
           `BDA`.`UPLOAD_BATCH` AS `UPLOAD_BATCH`,
           `BDA`.`RECORD_HASH` AS `RECORD_HASH`,
           `BDA`.`TRANS_DATE` AS `TRANS_DATE`,
           `BDA`.`TRANS_VALUE` AS `TRANS_VALUE`,
           `BDA`.`CAT_NUM` AS `CAT_NUM`,
           `BDA`.`CAT_NAME` AS `CAT_NAME`,
           `BDA`.`REF_NUM` AS `REF_NUM`,
           `BDA`.`DESC` AS `DESC`,
           `BDA`.`CODE` AS `CODE`,
           `BDA`.`IDENTIFIER` AS `IDENTIFIER`
    from `BANK_DATA_ARCHIVE` `BDA`;