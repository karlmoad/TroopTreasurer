CREATE
    VIEW `TROOP_TRANSACTION_JOURNAL`
    AS SELECT `TD`.`ID` AS `ID`,
              `TD`.`UPLOAD_BATCH` AS `UPLOAD_BATCH`,
              `TD`.`RECORD_HASH` AS `RECORD_HASH`,
              `TD`.`ACCT_HASH` AS `ACCT_HASH`,
              `TD`.`ACCT_NAME` AS `ACCT_NAME`,
              `TD`.`ACTIVITY_DATE` AS `ACTIVITY_DATE`,
              `TD`.`ACTIVITY_VALUE` AS `ACTIVITY_VALUE`,
              `TD`.`MEMO` AS `MEMO`,
              `TD`.`PAYEE` AS `PAYEE`,
              `TD`.`DESCRIPTION` AS `DESCRIPTION`,
              `TD`.`CATEGORIES` AS `CATEGORIES`,
              `TD`.`CHECK_NUM` AS `CHECK_NUM`,
              `TD`.`TENDER_TYPE` AS `TENDER_TYPE`,
              `TD`.`TENDER_HASH` AS `TENDER_HASH`,
              `TD`.`DETAILS` AS `DETAILS`,
              `TD`.`BALANCE` AS `BALANCE`
    FROM `TROOP_TRANSACTIONS` `TD`
    UNION ALL
    SELECT `TDA`.`ID` AS `ID`,
           `TDA`.`UPLOAD_BATCH` AS `UPLOAD_BATCH`,
           `TDA`.`RECORD_HASH` AS `RECORD_HASH`,
           `TDA`.`ACCT_HASH` AS `ACCT_HASH`,
           `TDA`.`ACCT_NAME` AS `ACCT_NAME`,
           `TDA`.`ACTIVITY_DATE` AS `ACTIVITY_DATE`,
           `TDA`.`ACTIVITY_VALUE` AS `ACTIVITY_VALUE`,
           `TDA`.`MEMO` AS `MEMO`,
           `TDA`.`PAYEE` AS `PAYEE`,
           `TDA`.`DESCRIPTION` AS `DESCRIPTION`,
           `TDA`.`CATEGORIES` AS `CATEGORIES`,
           `TDA`.`CHECK_NUM` AS `CHECK_NUM`,
           `TDA`.`TENDER_TYPE` AS `TENDER_TYPE`,
           `TDA`.`TENDER_HASH` AS `TENDER_HASH`,
           `TDA`.`DETAILS` AS `DETAILS`,
           `TDA`.`BALANCE` AS `BALANCE`
    FROM `TROOP_TRANSACTIONS_ARCHIVE` `TDA`;