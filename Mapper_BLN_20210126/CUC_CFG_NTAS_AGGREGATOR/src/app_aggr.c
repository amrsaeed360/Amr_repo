#include "app_aggr.h"
/***********************************************************************
 *
 * Main file of TAS appregator-correlator node.
 *
 ***********************************************************************/


int handleSelectUtilization = 0;
int handleInsertQuery = 0;
int handleRecCountQuery = 0;
int handleSelectQuery = 0;
int handleUpdateQuery = 0;
int handleUpdateTimeoutQuery = 0;
int handleDeleteQuery = 0;
int handleDeleteRecQuery = 0;
int handleAllSelectQuery = 0;
int handleTimeoutSelectQuery = 0;

// parameters
int prm_TRS_threshold;
int prm_PartialBufferedACRLimit = 1;
int prm_PartialTriggerTypeValues_length = 0;
char **prm_PartialTriggerTypeValues = NULL;
char **prm_SessionIdFields = NULL;
int triggerTypeOneEnabled = 0;
int prm_SessionIdFields_length = 0;
int prm_SessionTimeout;
int prm_SessionTimeoutInterimInterval;
int prm_SessionTimeoutInterimFactor;
char prm_RecordSeqNumberInitial [12] = {'\0'};
int prm_RecordSeqNumberKeepLastEmpty;
int prm_AddDurationMilli;
DBBlock blocks[120];

// functions for the file
void aggr_storage_init();

/* This function is called in the beginning when the node is starting up.
 *
 * Arguments:
 *   None.
 * Return values:
 *   None.
 */
void node_init(void) {
    DIAG(DIAG_LOW, "node_init(): entered the function");

    nb_load_library("trs");

    if (!trs_initialize()) {
        DIAG(DIAG_LOW, "node_init(): Initializing TRS library failed, aborting");
        nb_msg("AGGR001");
        nb_abort();
    }

    // load TRS library
    if (sql_initialize() != TRS_SUCCESS) {
        char* msg = "node_init(): Unable to initialize TRS library. Aborting...";
        DIAG(DIAG_LOW, msg);
        nb_msg("AGGR002");
        nb_abort();
    }
    DIAG(DIAG_LOW, "node_init(): TRS library initialized...");

    aggr_storage_init();

    const char* tmp_par = nb_get_parameter_string("TRS_threshold");

    prm_TRS_threshold = atol(tmp_par);
    // validate the above parameter
    if (prm_TRS_threshold <= 0 || prm_TRS_threshold >= 100) {
        char* msg = "node_init(): Mandatory parameter TRS_threshold incorrect value Aborting...";
        DIAG(DIAG_LOW, msg);
        nb_msg("AGGR005");
        nb_abort();
    }

    // Read Partial CDR creation related node parameters    
    tmp_par = nb_get_parameter_string("PartialBufferedACRLimit");
    if (strlen(tmp_par) == 0) {
        prm_PartialBufferedACRLimit = 0;
    } else {
        prm_PartialBufferedACRLimit = atoi(tmp_par);
    }

    DIAG(DIAG_LOW, "node_init(): PartialBufferedACRLimit was %s", tmp_par);

    tmp_par = nb_get_parameter_string("PartialTriggerTypeValues");
    DIAG(DIAG_LOW, "node_init(): PartialTriggerTypeValues were %s", tmp_par);
    if (strlen(tmp_par) == 0 || strcmp(tmp_par, "0") == 0) {
        prm_PartialTriggerTypeValues_length = 0;
    } else {
        prm_PartialTriggerTypeValues_length = split_string(&prm_PartialTriggerTypeValues, tmp_par, ',');
        int i;
        for (i = 0; i < prm_PartialTriggerTypeValues_length; i++) {
            if (strcmp(prm_PartialTriggerTypeValues[i], "1") == 0) {
                triggerTypeOneEnabled = 1;
                break;
            }
        }
    }

    DIAG(DIAG_LOW, "node_init(): triggerTypeOneEnabled was %d", triggerTypeOneEnabled);

    DIAG(DIAG_LOW, "node_init(): prm_PartialTriggerTypeValues_length was %d", prm_PartialTriggerTypeValues_length);

    tmp_par = nb_get_parameter_string("SessionIdFields");
    DIAG(DIAG_LOW, "node_init(): SessionIdFields %s", tmp_par);
    if (strlen(tmp_par) == 0) {
        tmp_par = "Session-Id";
    }
    prm_SessionIdFields_length = split_string(&prm_SessionIdFields, tmp_par, ' ');
    DIAG(DIAG_LOW, "node_init(): prm_SessionIdFields_length %d", prm_SessionIdFields_length);

    tmp_par = nb_get_parameter_string("SessionTimeout");
    prm_SessionTimeout = atoi(tmp_par);
    DIAG(DIAG_LOW, "node_init(): prm_SessionTimeout %d", prm_SessionTimeout);

    tmp_par = nb_get_parameter_string("SessionTimeoutInterimInterval");
    if (strlen(tmp_par) == 0) {
        tmp_par = "0";
    }
    prm_SessionTimeoutInterimInterval = atoi(tmp_par);
    DIAG(DIAG_LOW, "node_init(): prm_SessionTimeoutInterimInterval %d", prm_SessionTimeoutInterimInterval);

    tmp_par = nb_get_parameter_string("SessionTimeoutInterimFactor");
    if (strlen(tmp_par) == 0) {
        tmp_par = "0";
    }
    prm_SessionTimeoutInterimFactor = atoi(tmp_par);
    DIAG(DIAG_LOW, "node_init(): prm_SessionTimeoutInterimFactor %d", prm_SessionTimeoutInterimFactor);

    tmp_par = nb_get_parameter_string("RecordSeqNumberInitial");
    if (strlen(tmp_par) == 0) {
        tmp_par = "1";
    }
    int iValue = atoi(tmp_par);
    if (iValue != 0 && iValue != 1) {
        iValue = 1;
    }
    sprintf(prm_RecordSeqNumberInitial, "%d", iValue);
    ;
    DIAG(DIAG_LOW, "node_init(): prm_RecordSeqNumberInitial %s", prm_RecordSeqNumberInitial);

    tmp_par = nb_get_parameter_string("RecordSeqNumberKeepLastEmpty");
    if (strcmp("Always Empty", tmp_par) == 0) {
        prm_RecordSeqNumberKeepLastEmpty = 1;
    } else if (strcmp("Never Empty", tmp_par) == 0) {
        prm_RecordSeqNumberKeepLastEmpty = 2;
    } else {
        prm_RecordSeqNumberKeepLastEmpty = 0;
    }
    DIAG(DIAG_LOW, "node_init(): prm_RecordSeqNumberKeepLastEmpty %d", prm_RecordSeqNumberKeepLastEmpty);

    tmp_par = nb_get_parameter_string("TimeZoneDesignator");
    if (strlen(tmp_par) == 0) {
        DIAG(DIAG_LOW, "node_init(): prm_TimeZoneDesignator not set");
    } else if (strcmp("Z", tmp_par) == 0) {
        DIAG(DIAG_LOW, "node_init(): prm_TimeZoneDesignator use UTC");
        setenv("TZ", "UTC", 1);
        tzset();
    } else if (strlen(tmp_par) > 6) {
        DIAG(DIAG_LOW, "node_init(): prm_TimeZoneDesignator is invalid : %s", tmp_par);
    } else {
        // TZ format specifies "The offset is positive if the local time zone is west of the
        // Prime Meridian and negative if it is east."
        // So we need to flip the sign
        char zone[20] = {'\0'};
        strcat(zone, "local");
        if (tmp_par[0] == '+') {
            strcat(zone, "-");
        } else {
            strcat(zone, "+");
        }
        strcat(zone, &tmp_par[1]);

        DIAG(DIAG_LOW, "node_init(): prm_TimeZoneDesignator use %s", zone);
        setenv("TZ", zone, 1);
        tzset();
    }

    tmp_par = nb_get_parameter_string("AddDurationMilli"); 

    if(strcmp("Yes",tmp_par) == 0)
    {
        prm_AddDurationMilli = 1;
    }
    else
    {
        prm_AddDurationMilli = 0;
    }    
    /**
     * All known block names for compression. See do_compress() and do_decompress().
     */
    addBlock(0, ".access-Network-Information");
    addBlock(1, ".called-Party-Address.");
    addBlock(2, ".called-Party-Address.sIP-URI");
    addBlock(3, ".iMS-Charging-Identifier");
    addBlock(4, ".list-Of-Calling-Party-Address.");
    addBlock(5, ".list-Of-Calling-Party-Address.sIP-URI");
    addBlock(6, ".list-Of-SDP-Media-Components.");
    addBlock(7, ".list-Of-SDP-Media-Components.List-of-Media-Components.");
    addBlock(8, ".list-Of-SDP-Media-Components.List-of-Media-Components.sDP-Media-Components.");
    addBlock(9, ".list-Of-SDP-Media-Components.List-of-Media-Components.sDP-Media-Components.SDP-Media-Component.");
    addBlock(10, ".list-Of-SDP-Media-Components.List-of-Media-Components.sDP-Media-Components.SDP-Media-Component.sDP-Media-Descriptions.");
    addBlock(11, ".list-Of-SDP-Media-Components.List-of-Media-Components.sDP-Media-Components.SDP-Media-Component.sDP-Media-Descriptions.GraphicString");
    addBlock(12, ".list-Of-SDP-Media-Components.List-of-Media-Components.sDP-Media-Components.SDP-Media-Component.sDP-Media-Name");
    addBlock(13, ".list-Of-SDP-Media-Components.List-of-Media-Components.sDP-Session-Description.");
    addBlock(14, ".list-Of-SDP-Media-Components.List-of-Media-Components.sDP-Session-Description.GraphicString");
    addBlock(15, ".list-Of-SDP-Media-Components.List-of-Media-Components.sDP-Type");
    addBlock(16, ".list-Of-SDP-Media-Components.List-of-Media-Components.sIP-Request-Time-Stamp");
    addBlock(17, ".list-Of-SDP-Media-Components.List-of-Media-Components.sIP-Response-Time-Stamp");
    addBlock(18, ".node-Address.");
    addBlock(19, ".node-Address.domainName");
    addBlock(20, ".online-Charging-Flag");
    addBlock(21, ".record-Closure-Time");
    addBlock(22, ".record-Opening-Time");
    addBlock(23, ".record-Type");
    addBlock(24, ".retransmission");
    addBlock(25, ".role-of-Node");
    addBlock(26, ".service-Delivery-Start-Time-Stamp");
    addBlock(27, ".service-Request-Time-Stamp");
    addBlock(28, ".session-ID");
    addBlock(29, ".timestamp");
    addBlock(30, ".timestamps");
    addBlock(31, ".called-Party-Address.tEL-URI");
    addBlock(32, ".calling-Party.");
    addBlock(33, ".calling-Party.sIP-URI");
    addBlock(34, ".calling-Party.tEL-URI");
    addBlock(35, ".Cause-Code");
    addBlock(36, ".charged-Party.");
    addBlock(37, ".charged-Party.sIP-URI");
    addBlock(38, ".charged-Party.tEL-URI");
    addBlock(39, ".connected-Number.");
    addBlock(40, ".connected-Number.tEL-URI");
    addBlock(41, ".connected-Number.sIP-URI");
    addBlock(42, ".tADS-Indication");
    addBlock(43, ".cS-Location-Information.");
    addBlock(44, ".cS-Location-Information.CS-Location-Information.");
    addBlock(45, ".cS-Location-Information.CS-Location-Information.cell-Identification");
    addBlock(46, ".cS-Location-Information.CS-Location-Information.location-Area");
    addBlock(47, ".cS-Location-Information.CS-Location-Information.mscNumber");
    addBlock(48, ".cS-Location-Information.CS-Location-Information.service-Area-Code");
    addBlock(49, ".ims-Visited-Network-Identifier");
    addBlock(50, ".inter-Operator-Identifiers.");
    addBlock(51, ".inter-Operator-Identifiers.Inter-Operator-Identifiers.");
    addBlock(52, ".inter-Operator-Identifiers.Inter-Operator-Identifiers.originating-IOI");
    addBlock(53, ".inter-Operator-Identifiers.Inter-Operator-Identifiers.terminating-IOI");
    addBlock(54, ".local-Record-Sequence-Number");
    addBlock(55, ".list-Of-Calling-Party-Address.tEL-URI");
    addBlock(56, ".sNMD-Service-Type");
    addBlock(57, ".list-Of-SDP-Media-Components.List-of-Media-Components.sDP-Media-Components.SDP-Media-Component.gPRS-Charging-Id");
    addBlock(58, ".record-Sequence-Number");
    addBlock(59, ".requested-Party-Address.");
    addBlock(60, ".requested-Party-Address.tEL-URI");
    addBlock(61, ".requested-Party-Address.sIP-URI");
    addBlock(62, ".sDP-Media-Identifier");
    addBlock(63, ".sDP-Media-Identifier_LATEST");
    addBlock(64, ".service-Identity.");
    addBlock(65, ".service-Identity.Service-Identity");
    addBlock(66, ".service-Interact-Info.");
    addBlock(67, ".service-Interact-Info.executed-Service");
    addBlock(68, ".service-Reason-Return-Code");
    addBlock(69, ".subscriber-Equipment-Number.");
    addBlock(70, ".subscriber-Equipment-Number.subscriber-Equipment-Number-Data");
    addBlock(71, ".subscriber-Equipment-Number.subscriber-Equipment-Number-Type");
    addBlock(72, ".sIP-Method");
    addBlock(73, ".service-Delivery-End-Time-Stamp");
    addBlock(74, ".Trigger-Type");
    addBlock(75, ".cause-For-Record-Closing");
    addBlock(76, ".duration");
    addBlock(77, ".list-Of-SDP-Media-Components.List-of-Media-Components.media-Initiator-Flag");
    addBlock(78, ".serviceRequestTimeStampFraction");
    addBlock(79, ".serviceDeliveryStartTimeStampFraction");
    addBlock(80, ".serviceDeliveryEndTimeStampFraction");
    addBlock(81, ".list-Of-SDP-Media-Components.List-of-Media-Components.sIP-Request-Timestamp-Fraction");
    addBlock(82, ".list-Of-SDP-Media-Components.List-of-Media-Components.sIP-Response-Timestamp-Fraction");
    addBlock(83, ".dialled-Party-Address.");
    addBlock(84, ".dialled-Party-Address.tEL-URI");
    addBlock(85, ".dialled-Party-Address.sIP-URI");
    addBlock(86, ".list-of-subscription-ID.");
    addBlock(87, ".list-of-subscription-ID.SubscriptionID.");
    addBlock(88, ".list-of-subscription-ID.SubscriptionID.subscriptionIDType");
    addBlock(89, ".list-of-subscription-ID.SubscriptionID.subscriptionIDData");
    addBlock(90, ".list-Of-Called-Asserted-Identity.");
    addBlock(91, ".list-Of-Called-Asserted-Identity.tEL-URI");
    addBlock(92, ".list-Of-Called-Asserted-Identity.sIP-URI");
    addBlock(93, ".call-Description");
    addBlock(94, ".ctxgroup-Number");
    addBlock(95, ".short-Number");
    addBlock(96, ".list-Of-Message-Bodies.");
    addBlock(97, ".list-Of-Message-Bodies.MessageBody.");
    addBlock(98, ".list-Of-Message-Bodies.MessageBody.content-Type");
    addBlock(99, ".list-Of-Message-Bodies.MessageBody.content-Disposition");
    addBlock(100, ".list-Of-Message-Bodies.MessageBody.content-Length");
    addBlock(101, ".list-Of-Message-Bodies.MessageBody.originator.");
    addBlock(102, ".list-Of-Message-Bodies.MessageBody.originator.tEL-URI");
    addBlock(103, ".list-Of-Message-Bodies.MessageBody.originator.sIP-URI");
    addBlock(104, ".list-Of-Message-Bodies.MessageBody.");
    addBlock(105, ".privateUserID");
    addBlock(106, ".applicationServersInformation.");
    addBlock(107, ".applicationServersInformation.ApplicationServersInformation.");
    addBlock(108, ".applicationServersInformation.ApplicationServersInformation.applicationServersInvolved.");
    addBlock(109, ".applicationServersInformation.ApplicationServersInformation.applicationServersInvolved.domainName");
    addBlock(110, ".applicationServersInformation.ApplicationServersInformation.applicationProvidedCalledParties.");
    addBlock(111, ".applicationServersInformation.ApplicationServersInformation.applicationProvidedCalledParties.tEL-URI");
    addBlock(112, ".applicationServersInformation.ApplicationServersInformation.applicationProvidedCalledParties.sIP-URI");
    addBlock(113, ".valueadded-Service-Identity.");
    addBlock(114, ".valueadded-Service-Identity.INTEGER");
    addBlock(115, ".aCName");
    addBlock(116, ".user-Charging-Category");
    addBlock(117,".record-Opening-Time-Fraction");
    addBlock(118,".record-Closure-Time-Fraction"); 
     addBlock(119,".Total_volume");
    // Last block need to be NULL
    addBlock(120, NULL);
    DIAG(DIAG_HIGH, "Known block names:");
    int i = -1;
    while (blocks[++i].longName != NULL) {
        DIAG(DIAG_HIGH, "%s", blocks[i].longName);
    }
    DIAG(DIAG_LOW, "node_init(): returning...");
}

void addBlock(int numberCode, char* longName) {
    blocks[numberCode].longName = longName;
    if (longName != NULL) {
        blocks[numberCode].shortName = get_field_name(longName);
        blocks[numberCode].level = get_level(longName);
    } else {
        blocks[numberCode].shortName = NULL;
        blocks[numberCode].level = -1;
    }
}

void freeBlockMemory() {
    int i = -1;
    while (blocks[++i].longName != NULL) {
        free(blocks[i].shortName);
    }
}

DBBlock getBlockByIndex(const char* indexAsString) {
    return blocks[atoi(indexAsString)];
}

int getBlockIndex(const char* mappedName) {
    int i = -1;
    while (blocks[++i].longName != NULL) {
        if (strcmp(mappedName, blocks[i].longName) == 0) {
            return i;
        }
    }
    return -1;
}

void aggr_storage_init() {
    DIAG(DIAG_LOW, "aggr_storage_init(): entered the function");

    // create the tables and indexes if they do not exist; if they do exist,
    // the queries will return errors - we ignore these errors
    sql_hide_errors();
    sql_execute_direct(SQL_CREATE_TABLE_CDRS);
    // an index is created automatically when we create a primary key
    sql_execute_direct(SQL_CREATE_INDEX_CDRS);
    sql_execute_direct(SQL_CREATE_INDEX_TIMEOUT);
    sql_commit();
    sql_show_errors();

    // initialize the prepared SQL statements
    handleSelectUtilization = sql_prepare(SQL_SELECT_UTILIZATION);
    handleInsertQuery = sql_prepare(SQL_INSERT_CDR);
    handleRecCountQuery = sql_prepare(SQL_SELECT_REC_COUNT);
    handleSelectQuery = sql_prepare(SQL_SELECT_CDR);
    handleUpdateQuery = sql_prepare(SQL_UPDATE_CDR);
    handleUpdateTimeoutQuery = sql_prepare(SQL_UPDATE_CDR_TIMEOUT);
    handleDeleteQuery = sql_prepare(SQL_DELETE_CDR);
    handleDeleteRecQuery = sql_prepare(SQL_DELETE_CDR_REC);
    handleAllSelectQuery = sql_prepare(SQL_SELECT_CDR_ALL);
    handleTimeoutSelectQuery = sql_prepare(SQL_SELECT_CDR_TIMEOUT);

    // verify that all SQL statements were prepared
    if (handleInsertQuery <= 0 ||
            handleRecCountQuery <= 0 ||
            handleSelectQuery <= 0 ||
            handleUpdateQuery <= 0 ||
            handleUpdateTimeoutQuery <= 0 ||
            handleDeleteQuery <= 0 ||
            handleDeleteRecQuery <= 0 ||
            handleAllSelectQuery <= 0 ||
            handleTimeoutSelectQuery <= 0
            ) {
        const char* err_msg = sql_get_error_message();
        char msg[(strlen(err_msg) + 200)];
        sprintf(msg,
                "sql_prepare_init(): Cannot create handle to TRS table "
                "RECORD_TABLE: %s", err_msg);
        DIAG(DIAG_LOW, msg);

        nb_msg("AGGR003");
        nb_abort();
    }
}

/* This function is called to determine if a partial CDR should be sent for this record
 * 
 * Returns 
 * 
 * 1 - partial CDR should be sent
 * 0 - partial CDR should not be sent
 * -1 - error situation, record rejected
 */
int check_partial_CDR_triggers(const char* bufferedACRNum, const char* triggerType, partialReason *pTrigger) {
    DIAG(DIAG_HIGH, "check_partial_CDR_triggers(): entered the function");

    DIAG(DIAG_MEDIUM, "check_partial_CDR_triggers(): Trigger-Type is '%s', bufferedACRNum is %s, buffer limit is %d"
            , triggerType, bufferedACRNum, prm_PartialBufferedACRLimit);

    if (strcmp(triggerType, "\0") != 0 && prm_PartialTriggerTypeValues_length > 0) {
        DIAG(DIAG_MEDIUM, "Trigger-Type is: %s", triggerType);
        int i;
        for (i = 0; i < prm_PartialTriggerTypeValues_length; i++) {
            DIAG(DIAG_MEDIUM, "Possible trigger value is: %s", prm_PartialTriggerTypeValues[i]);
            if (strcmp(prm_PartialTriggerTypeValues[i], triggerType) == 0) {
                DIAG(DIAG_MEDIUM, "check_partial_CDR_triggers(): Partial CDR Trigger Type condition met with value: %s", triggerType);
                *pTrigger = TRIGGER_TYPE;
                return 1;
            }
        }
    }

    if (prm_PartialBufferedACRLimit > 0 && atoi(bufferedACRNum) >= prm_PartialBufferedACRLimit) {
        DIAG(DIAG_MEDIUM, "check_partial_CDR_triggers(): PartialBufferedACRLimit condition met at buffered ACR number: %s", bufferedACRNum);
        return 1;
    }

    return 0;
}

/* This function is called for every input record or for every input
 * file, if input is binary.
 *
 * Arguments:
 *   None.
 * Return values:
 *   None.
 */
void node_process(void) {
    DIAG(DIAG_HIGH, "node_process(): entered the function");

    dump_input_record_to_log();

    if (!check_utilization(prm_TRS_threshold)) {
        DIAG(DIAG_LOW, "node_process(): TRS is full. Please resize the TRS...");

        nb_msg("AGGR012");
    }

    const char* acc_rec_type = i_get("Accounting-Record-Type"); // 1 - EVENT | 2 - START | 3 - INTERIM | 4 - STOP
    int udr_type = atoi(acc_rec_type);
    const char* udr_rec_num = i_get("Accounting-Record-Number");
    const char* triggerType = i_get(".Trigger-Type");
    char* udr_session_id = NULL;
    load_session_id(&udr_session_id);
    //const char* cdr_type = "6";
    const char* cdr_type = i_get(".Service-Information.IMS-Information.Node-Functionality");
    DIAG(DIAG_MEDIUM, "node_process(): session_id = '%s' udr type = '%d'", udr_session_id, udr_type);

    if (strlen(udr_session_id) == 0) {
        nb_msg("AGGR020");
        nb_abort();
    }

    if (udr_type == 1) {
        nb_new_record();
        if (write_ASN1_record(EVENT, cdr_type, triggerType, NULL, 0)) { // rollback if needed
            trs_rollback();
            return;
        }

        handleRecordSequenceNumber(1, 0);
        write_record_to_output(0, 0, 0, EVENT, cdr_type, RULE, "", 0); // zero loss record written out

        dump_output_record_to_log();

    } else if (udr_type == 2 || udr_type == 3 || udr_type == 4) {
        // Get number of records (needed because we need to know beforehand the size of records array)
        int number_of_records = get_number_of_records_from_trs(udr_session_id, cdr_type);

        Record trs_records[number_of_records];

        int startMissing = 0;
        int nextInSequence = 0; // 1 in case this record is next in the sequence
        int secondGapIndex = -1; // -1 means that there is no second gap
        int newRecNum = atoi(udr_rec_num); // record number for the input record

        if (number_of_records > 0) {
            fetchAllRecordsFromTRS(udr_session_id, cdr_type, trs_records, number_of_records, &nextInSequence, newRecNum, &secondGapIndex, &startMissing);
        }

        if (!startMissing && number_of_records == 1) {
            // There were no out of order records found from TRS, check if this record is out of order
            nextInSequence = (newRecNum == atoi(trs_records[0].rec_num) + 1) ? 1 : 0;
            DIAG(DIAG_MEDIUM, "node_process(): No out of sequence records found, this record is '%s'", (nextInSequence ? "next in sequence" : "out of order"));
        } else if (startMissing || number_of_records > 1) {
            // check for gaps and whether this record fills a gap or should be handled as out of order record
            searchForOutOfOrderGaps(startMissing, newRecNum, &nextInSequence, &secondGapIndex, number_of_records, trs_records);
        }

        Record *newRecord; // Pointer for new record, in case we are aggregating, basis is taken from aggregate record in timesten
        char rec_num_to_update[5] = {'\0'};
        if (!startMissing && number_of_records > 0 && nextInSequence) {
            // Records were found from TRS, the aggregated record is always the first one (with the smallest record number, because select orders by rec_num)			
            newRecord = &trs_records[0];
            do_decompress(newRecord->record); // Create output record as basis for aggregation
            sprintf(rec_num_to_update, newRecord->rec_num);
            sprintf(newRecord->buffered_acr_num, "%d", atoi(newRecord->buffered_acr_num) + 1); // Increment buffered ACR num by one
            DIAG(DIAG_MEDIUM, "node_process(): Records were found from TRS and input record is next in sequence, aggregating on top of record number: '%s'", rec_num_to_update);
        } else {
            // No records were found from TRS, this record is out of sequence, or start was missing and this record is start, create new TRS_rec struct for newRecord to point to
            Record newRecordAddress;
            // Clear values, since otherwise there is some leftover information from last node_process
            CLEAR_CHAR_ARRAY(newRecordAddress.buffered_acr_num);
            CLEAR_CHAR_ARRAY(newRecordAddress.rec_num);
            CLEAR_CHAR_ARRAY(newRecordAddress.record);
            CLEAR_CHAR_ARRAY(newRecordAddress.udr_type);
            CLEAR_CHAR_ARRAY(newRecordAddress.start_lost);
            newRecord = &newRecordAddress;
            sprintf(newRecord->buffered_acr_num, "%d", 1); // start buffered ACR num from one
            DIAG(DIAG_MEDIUM, "node_process(): No records were found from TRS or input record is out of sequence, storing input record as single record");
        }
        sprintf(newRecord->rec_num, udr_rec_num);
        sprintf(newRecord->udr_type, acc_rec_type);

        int sendCDR = 0; // Do we send a CDR this time or not (partial CDR or complete CDR)
        partialReason pTrigger = RULE;
        int isPartial = 1;
        int reject = RETURN_SUCCESS; // 1 in case we need to reject this record (actual rejecting is done where failure is detected (where we know the reject reason)

        if (number_of_records <= 0 || !nextInSequence) {
            // No records found from TRS, or input record is out of order, handle this as a new record to store in TimesTen
            reject = handleNewCDR(udr_session_id, cdr_type, triggerType, newRecord, udr_type);
        } else {
            // We can do some aggregation for this record (and possibly to existing out of order records)
            int startIndex = startMissing ? 0 : 1; // In case start was missing, we need to start from 0, otherwise start from 1
            int stopIndex = (secondGapIndex == -1) ? number_of_records : secondGapIndex; // Stop before the possible second gap
            // First aggregate the input record on top of the current aggregated record in TimesTen
            int updateTrs = 0; // Used to disable TRS updates when we are doing aggregation for intermediate out of order records (only update the last aggregated result)
            if (udr_type == 2) {
                // In case we receive start partial out of order, handle it as a new CDR and aggregate the rest on top of it
                DIAG(DIAG_MEDIUM, "node_process(): Start partial received out of order");
                // We are going to aggregate on top of start
                reject = handleNewCDR(udr_session_id, cdr_type, triggerType, newRecord, udr_type);
                if (!reject && stopIndex > startIndex) {
                    DIAG(DIAG_MEDIUM, "node_process(): Preparing for more records to aggregate after out of order start partial");
                    sprintf(rec_num_to_update, newRecord->rec_num); // Start was missing, do the first update on top of input record (start)
                }
            } else {
                if (stopIndex > startIndex) {
                    // We are going to aggregate something on top of this record after handling this one
                    DIAG(DIAG_MEDIUM, "node_process(): Aggregating first record");
                    reject = aggregateCDR(udr_session_id, cdr_type, triggerType, newRecord, udr_type, rec_num_to_update, &sendCDR, &pTrigger, &isPartial, 0);
                    // don't update TRS yet (it will be updated after the last out of order record has been aggregated
                } else {
                    // This is the only record we will handle at this time
                    DIAG(DIAG_MEDIUM, "node_process(): Aggregating record");
                    reject = aggregateCDR(udr_session_id, cdr_type, triggerType, newRecord, udr_type, rec_num_to_update, &sendCDR, &pTrigger, &isPartial, 0);
                    // Update TRS, since this is the only record to aggregate this time
                    updateTrs = 1;
                }
            }

            if (reject) {
                // Record was rejected do rollback and end handling here
                trs_rollback();
                return;
            } else if (sendCDR) {
                sendCDRToOutput(udr_type, cdr_type, triggerType, pTrigger, isPartial);
                sprintf(newRecord->buffered_acr_num, "0"); // Reset number of buffered records when sending CDR
            }

            if (updateTrs) {
                updateTRS(udr_session_id, cdr_type, newRecord, udr_type, rec_num_to_update);
            }

            // Then aggregate all possible out of order records which we are now able to aggregate
            int i;
            int bufferedAcrNum = atoi(newRecord->buffered_acr_num);
            for (i = startIndex; i < stopIndex; i++) { // start from 1 because the first is always the aggregated record (except when start was missing, then start from 0)
                DIAG(DIAG_MEDIUM, "node_process(): Handling out of order record at index: %d", i);
                pTrigger = RULE;
                // Delete record from TRS (needed first, because aggregate record will get the same rec_num)
                delete_rec_from_TRS(udr_session_id, cdr_type, trs_records[i].rec_num);
                // increment buffered ACR num by one
                bufferedAcrNum++;
                sprintf(trs_records[i].buffered_acr_num, "%d", bufferedAcrNum);
                // Aggregate this out of order record
                int trs_udr = atoi(trs_records[i].udr_type);
                updateTrs = (i < stopIndex - 1) ? 0 : 1; // Only update TRS for last record to aggregate
                aggregateCDR(udr_session_id, cdr_type, triggerType, &trs_records[i], trs_udr, rec_num_to_update, &sendCDR, &pTrigger, &isPartial, 1);
                if (sendCDR) {
                    sendCDRToOutput(trs_udr, cdr_type, triggerType, pTrigger, isPartial);
                    bufferedAcrNum = 0; // Reset number of buffered records when sending CDR
                    sprintf(trs_records[i].buffered_acr_num, "0"); // Reset number of buffered records when sending CDR
                }
                DIAG(DIAG_MEDIUM, "node_process(): updateTrs '%d', stopIndex '%d', startIndex '%d'", updateTrs, stopIndex, startIndex);
                if (updateTrs) {
                    DIAG(DIAG_MEDIUM, "node_process(): updating trs");
                    updateTRS(udr_session_id, cdr_type, &trs_records[i], trs_udr, rec_num_to_update);
                }
            }
        }
        if (isPartial) {
            // Update timeout timestamp for each ACR which isn't STOP
            update_timeout_TRS(udr_session_id, cdr_type);
        }
    } else {
        DIAG(DIAG_LOW, "node_process():  Unknown partial detected udr_type '%d'", udr_type);
        nb_msg("AGGR019");
        nb_abort();
    }

    if (prm_SessionIdFields_length > 1) {
        free(udr_session_id);
    }



    DIAG(DIAG_HIGH, "node_process(): returning...");
}

void sendCDRToOutput(int udr_type, const char* cdr_type, const char* triggerType, partialReason pTrigger, int isPartial) {
    flag recordType = EVENT;
    switch (udr_type) {
        case 1:
            recordType = EVENT;
            break;
        case 2:
            recordType = START;
            break;
        case 3:
            recordType = INTERIM;
            break;
        case 4:
            recordType = STOP;
            break;
        default:
            DIAG(DIAG_LOW, "node_process():  Unknown partial detected udr_type '%d'", udr_type);
            nb_msg("AGGR019");
            nb_abort();
    }
    DIAG(DIAG_LOW, "sendCDRToOutput(): Sending CDR, isPartial=%d, triggerType=%s", isPartial, triggerType);
    write_record_to_output(0, 0, 0, recordType, cdr_type, pTrigger, triggerType, isPartial); // zero loss record written out
}

void searchForOutOfOrderGaps(int startMissing, int newRecNum, int* nextInSequence, int* secondGapIndex, int number_of_records, Record trs_records[]) {
    /* We only need to know the possible first and second gaps because:
     * - if this record fills the first gap and there is no second gap -> we can aggregate every record
     * - if this record fills the first gap and there is a second gap -> we can aggregate records up until the second gap
     * - other gaps are irrelevant, since this record can only fill one gap
     */
    DIAG(DIAG_MEDIUM, "searchForOutOfOrderGaps(): Start missing: '%s', out of order records found (%d), checking for gaps and whether input record fills the first gap or not"
            , startMissing ? "true" : "false", number_of_records - 1);
    int i;

    // Search for gaps if needed (start missing case is sometimes an exception)
    if (!startMissing || (*nextInSequence && *secondGapIndex < 0)) {
        for (i = 1; i < number_of_records; i++) { // start at 1 because index 0 is always the aggregated record, or in case of start missing, index 0 has been handled above
            int recNum = atoi(trs_records[i].rec_num);
            int previousRecNum = atoi(trs_records[i - 1].rec_num);
            int nextRecNumInOrder = previousRecNum + 1;
            if (recNum > nextRecNumInOrder) {
                DIAG(DIAG_MEDIUM, "searchForOutOfOrderGaps(): Gap found at %d", nextRecNumInOrder);
                if (!*nextInSequence && newRecNum == nextRecNumInOrder) {
                    // First gap filled, we can aggregate this record and every record until next possible gap (first gap should ALWAYS be at index 1)
                    DIAG(DIAG_MEDIUM, "searchForOutOfOrderGaps(): This was the first gap (index %d) and input record fills it, setting nextInSequence to 1", i);
                    *nextInSequence = 1;
                    // Check if there is another gap after this one
                    if (recNum > nextRecNumInOrder + 1) {
                        // There was a second gap directly after the first gap
                        *secondGapIndex = i; // Second and first gap are at the same index (we can only aggregate the input record
                        DIAG(DIAG_MEDIUM, "searchForOutOfOrderGaps(): Second gap found directly after the first gap, we're able to aggregate records until index %d in records array", *secondGapIndex);
                        break;
                    }
                } else if (*nextInSequence) {
                    // Second gap found, we can aggregate every record in records list before this record						
                    *secondGapIndex = i;
                    DIAG(DIAG_MEDIUM, "searchForOutOfOrderGaps(): Second gap found, we're able to aggregate records until index %d in records array", *secondGapIndex);
                    break; // Second gap found, no need to check further
                } else {
                    DIAG(DIAG_MEDIUM, "searchForOutOfOrderGaps(): First gap found, but input record doesn't fill it -> we can't aggregate anything, store this record as an out of order record");
                    break;
                }
            }
        }
    }
}

void fetchAllRecordsFromTRS(char* udr_session_id, const char* cdr_type, Record trs_records[], int number_of_records,
        int* nextInSequence, int newRecNum, int* secondGapIndex, int* startMissing) {
    // Fetch all records for this session and CDR type from TRS (records will be stored in records array)
    int found_records = select_records_from_trs(udr_session_id, cdr_type, trs_records);
    if (number_of_records != found_records) {
        DIAG(DIAG_LOW, "fetchAllRecordsFromTRS():  Number of found records was different from number of records (%d != %d)!"
                , found_records, number_of_records);
        nb_msg("AGGR008");
        nb_abort();
    }
    *startMissing = atoi(trs_records[0].start_lost);
    if (*startMissing) {
        // start has not come yet, handle first index here
        *nextInSequence = newRecNum == 0 ? 1 : 0; // record is only next in sequence if it's record number 0
        DIAG(DIAG_MEDIUM, "fetchAllRecordsFromTRS(): Start is missing, this record is '%s'", (*nextInSequence ? "next in sequence" : "out of order"));
        if (atoi(trs_records[0].rec_num) > 1) {
            // there is a second gap right after the first one, no need to do any further gap search
            DIAG(DIAG_MEDIUM, "fetchAllRecordsFromTRS(): Start missing case and found a second gap at first index");
            *secondGapIndex = 0;
        }
    }
}

/**
 * Update access network information with previous value.
 */
void updateNetworkInformation(const char* field_name, const char* old_field_name) {
    if (o_field_exists(old_field_name)) {
        // update accessNetworkInformation with accessNetworkInformationOld and remove old from out file
        DIAG(DIAG_HIGH, "updateNetworkInformation():  Updating %s with old value", field_name);
        const char* oldAccessNetworkInformation = o_get(old_field_name);
        ADD_FIELD_DIAG(field_name, oldAccessNetworkInformation);
    } else {
        if (o_field_exists(field_name)) {
            // Field exists only as new field, remove it from output (CDR needs to contain only OLD values)
            DIAG(DIAG_HIGH, "updateNetworkInformation():  Old value for '%s' didn't exist, removing field from output", field_name);
            DEL_FIELD_DIAG(field_name);
        }
    }
}

/* This function is used to create a new CDR in TimesTen
 * 
 * returns 1 in case of failure (record should be rejected in that case)
 */
int handleNewCDR(char* udr_session_id, const char* cdr_type, const char* triggerType, Record *record, int udr_type) {
    DIAG(DIAG_MEDIUM, "handleNewCDR(): Handling new CDR for session '%s'", udr_session_id);

    int returnCode = RETURN_SUCCESS;
    if (udr_type == 2) {
        DIAG(DIAG_MEDIUM, "handleNewCDR():  START partial udr_type '%d'", udr_type);

        nb_new_record();
        returnCode = write_ASN1_record(START, cdr_type, triggerType, NULL, 0);

        handleRecordSequenceNumber(0, 0);

        sprintf(record->start_lost, "0");

        do_compress(record->record);
        insert_into_TRS(udr_session_id, cdr_type, record);

    } else if (udr_type == 3) {
        // This is an out of order INTERIM, we don't need to check for partial CDR sending here (they are checked when these are aggregated)
        DIAG(DIAG_MEDIUM, "handleNewCDR(): Out of order INTERIM partial  udr_type '%d'", udr_type);

        nb_new_record();
        returnCode = write_ASN1_record(INTERIM, cdr_type, triggerType, NULL, 0);
        handleRecordSequenceNumber(0, 0);

        sprintf(record->start_lost, "1"); // All out of order records will set start_lost to 1
        do_compress(record->record);
        insert_into_TRS(udr_session_id, cdr_type, record);

    } else if (udr_type == 4) {
        DIAG(DIAG_MEDIUM, "handleNewCDR(): Out of order STOP partial udr_type '%d'", udr_type);

        nb_new_record();
        returnCode = write_ASN1_record(STOP, cdr_type, triggerType, NULL, 0);
        handleRecordSequenceNumber(0, 1);

        sprintf(record->start_lost, "1"); // All out of order records will set start_lost to 1
        do_compress(record->record);
        insert_into_TRS(udr_session_id, cdr_type, record); // Insert stop record to wait for in order start/interims to arrive
    }

    return returnCode;
}

void initializeRecordValue(RecordValue* value, char* record) {
    DIAG(DIAG_HIGH, "splitting trs record to string array");
    value->length = split_string(&value->values, record, DELIM_FIELD_C);
}

/* This function is used to aggregate a record on top of another record
 * 
 * returns 1 in case of failure (record should be rejected in that case)
 */
int aggregateCDR(char* udr_session_id, const char* cdr_type, const char* triggerType, Record *record, int udr_type, char* rec_num_to_update
        , int *sendCDR, partialReason *pTrigger, int *isPartial, int readFromRecord) {

    DIAG(DIAG_MEDIUM, "aggregateCDR(): Found record in TRS for this record '%s' with session_id '%s'", cdr_type, udr_session_id);
    int returnCode = RETURN_SUCCESS;
    RecordValue value;
    initializeRecordValue(&value, record->record);

    // Reset sendCDR and isPartial (needed in out of order handling)
    *sendCDR = 0;
    *isPartial = 1;

    char* recordTriggerType;
    if (readFromRecord) {
        recordTriggerType = getFieldValueFromTRSRecord(".Trigger-Type", &value);
    }

    if (udr_type == 2) {
        DIAG(DIAG_LOW, "aggregateCDR(): START partial detected in aggregate function, this is a bug!", udr_type);
        split_string_free_array(value.values, value.length);
        return RETURN_REJECTED;
    } else if (udr_type == 3) {
        DIAG(DIAG_MEDIUM, "aggregateCDR():  INTERIM partial detected udr_type '%d'", udr_type);
        if (*sendCDR != -1) { // sendCDR == -1 here means that we shouldn't do sendCDR checking (timeout case)
            if (readFromRecord) {
                *sendCDR = check_partial_CDR_triggers(record->buffered_acr_num, recordTriggerType, pTrigger); // Check this before creating a new record (compare against decompressed record from TRS)
            } else {
                *sendCDR = check_partial_CDR_triggers(record->buffered_acr_num, triggerType, pTrigger); // Check this before creating a new record (compare against decompressed record from TRS)
            }
            if (*sendCDR == -1) { // sendCDR -1 here means that record was rejected during send_partial_CDR
                // Record was rejected, stop processing
                split_string_free_array(value.values, value.length);
                return 1;
            } else if (*sendCDR == 1 || *sendCDR == 0) {
                handleRecordSequenceNumber(0, 0);
            }
        }

        if (readFromRecord) {
            returnCode = write_ASN1_record(INTERIM, cdr_type, recordTriggerType, &value, readFromRecord);
        } else {
            returnCode = write_ASN1_record(INTERIM, cdr_type, triggerType, &value, readFromRecord);
        }

    } else if (udr_type == 4) {
        DIAG(DIAG_HIGH, "aggregateCDR():  STOP partial detected udr_type '%d'", udr_type);

        returnCode = write_ASN1_record(STOP, cdr_type, triggerType, &value, readFromRecord);

        *sendCDR = 1;
        *isPartial = 0;
        handleRecordSequenceNumber(0, 1);
    }

    if (readFromRecord) {
        free(recordTriggerType);
    }

    split_string_free_array(value.values, value.length);
    return returnCode;
}

/**
 * This function is used to update TRS after aggregation when needed. This is not used in new CDR (start/out of order interim/stop)
 * since we update TRS directly in handleNewCDR function in that case.
 * 
 * @param udr_session_id
 * @param cdr_type
 * @param record
 * @param udr_type
 * @param rec_num_to_update
 */
void updateTRS(char* udr_session_id, const char* cdr_type, Record *record, int udr_type, char* rec_num_to_update) {
    if (udr_type == 3) {
        do_compress(record->record);
        //update TRS for current interim
        update_TRS(udr_session_id, cdr_type, rec_num_to_update, record);
    } else if (udr_type == 4) {
        delete_from_TRS(udr_session_id, cdr_type);
    }
}

/* This function is called whenever a control record is received.
 *
 * Arguments:
 *   None.
 * Return values:
 *   None.
 */
void node_control(void) {
    DIAG(DIAG_LOW, "node_control()");
}

/* This function is called when the node commits after processing an
 * input file successfully.
 *
 * Arguments:
 *   None.
 * Return values:
 *   None.
 */
void node_commit(void) {
    DIAG(DIAG_LOW, "node_commit()");

    // commit SQL transactions

    if (trs_commit() == TRS_ERROR) {
        trs_rollback();
    }
}

/* This function is called in the end when the node is shutting down.
 *
 * Arguments:
 *   None.
 * Return values:
 *   None.
 */
void node_end(void) {
    DIAG(DIAG_LOW, "node_end()");
    sql_close(handleSelectUtilization);
    sql_close(handleInsertQuery);
    sql_close(handleRecCountQuery);
    sql_close(handleSelectQuery);
    sql_close(handleUpdateQuery);
    sql_close(handleUpdateTimeoutQuery);
    sql_close(handleDeleteQuery);
    sql_close(handleDeleteRecQuery);
    sql_close(handleTimeoutSelectQuery);
    sql_close(handleAllSelectQuery);
    split_string_free_array(prm_SessionIdFields, prm_SessionIdFields_length);
    freeBlockMemory();
}

/**
 * This function is called when the operator requested the flushing
 * of the steram. If the node stores any records in an internal
 * storage, all records should be retrieved from the storage and
 * written to the output in this function.
 *
 * Arguments:
 *   None.
 * Return values:
 *   None.
 */
void node_flush(void) {
    DIAG(DIAG_LOW, "node_flush(): entered the function");

    fetch_time_out_data(1);

    DIAG(DIAG_LOW, "node_flush(): end");
}

/* This function is called if an error occurs during the processing of a
 * file/record, and should be used to reset the system to the point of 
 * the last commit.
 *
 * Arguments:
 *   None.
 * Return values:
 *   None.
 */
void node_rollback(void) {
    DIAG(DIAG_LOW, "node_rollback()");
    sql_close(handleSelectUtilization);
    sql_close(handleInsertQuery);
    sql_close(handleRecCountQuery);
    sql_close(handleSelectQuery);
    sql_close(handleUpdateQuery);
    sql_close(handleUpdateTimeoutQuery);
    sql_close(handleDeleteQuery);
    sql_close(handleDeleteRecQuery);
    sql_close(handleAllSelectQuery);
    sql_close(handleTimeoutSelectQuery);
    trs_rollback();
}

/*
 * This function is called whenever the node is scheduled to be executed.
 *
 * Arguments:
 *   None.
 * Return values:
 *   NB_OK if the scheduled functionality is executed successfully, NB_ERROR otherwise
 */
int node_schedule(void) {
    DIAG(DIAG_LOW, "node_schedule(): entered the function");


    DIAG(DIAG_LOW, "node_schedule(): returning NB_OK...");
    return NB_OK;
}

/* This function is called in regular intervals, about every second.
 *
 * Arguments:
 *   None.
 * Return values:
 *   None.
 */
void node_timer(void) {
    DIAG(DIAG_MEDIUM, "node_timer() entered the function");

    fetch_time_out_data(0);

    DIAG(DIAG_MEDIUM, "node_timer() end ");
}

/* This function is called for real-time nodes if they have to be stopped.
 *
 * Arguments:
 *   None.
 * Return values:
 *   None.
 */
void node_pause(void) {
    DIAG(DIAG_LOW, "node_pause()");
}

/* This function is called for real-time nodes when they are resumed
 * after having paused.
 *
 * Arguments:
 *   None.
 * Return values:
 *   None.
 */
void node_resume(void) {
    DIAG(DIAG_LOW, "node_resume()");
}

/* This function is called upon receival of external requests.
 *
 * Arguments:
 *   None.
 * Return values:
 *   None.
 */
void node_request(void) {
    DIAG(DIAG_LOW, "node_request()");
}

/***********************************************************************
 *
 * Application-specific functions
 *
 ***********************************************************************/

/**
 * Initialize recordSequenceNumber parameter
 */
void handleRecordSequenceNumber(int single, int last) {

    DIAG(DIAG_HIGH, "handleRecordSequenceNumber(): single=%d, last=%d", single, last);
    //printf("handle rec seq: %d %s %d %d\n",prm_RecordSeqNumberKeepLastEmpty,prm_RecordSeqNumberInitial,single,last);

    // "Empty for Single", not sending for single CDR
    if (prm_RecordSeqNumberKeepLastEmpty == 0 && single == 1) {
        DIAG(DIAG_HIGH, "handleRecordSequenceNumber(): delete sequence number for single");
        o_delete("record-Sequence-Number");
        return;
    }

    // "Always Empty". not sending for single CDR or last CDR of a session
    if (prm_RecordSeqNumberKeepLastEmpty == 1 && (single == 1 || last == 1)) {
        DIAG(DIAG_HIGH, "handleRecordSequenceNumber(): delete sequence number for last or single");
        o_delete("record-Sequence-Number");
        return;
    }

    const char* recordSequenceNumber = o_get("record-Sequence-Number");
    if (strlen(recordSequenceNumber) == 0) {
        if (last == 1 && prm_RecordSeqNumberKeepLastEmpty == 0) {
            // Last and field has no previous value means this is actually single
            DIAG(DIAG_HIGH, "handleRecordSequenceNumber(): sequence doesn't exist and last record");
            return;
        }
        DIAG(DIAG_HIGH, "handleRecordSequenceNumber(): adding sequence number %s", prm_RecordSeqNumberInitial);
        ADD_FIELD_DIAG("record-Sequence-Number", prm_RecordSeqNumberInitial);
        return;
    }

    unsigned int sequenceNumber = 0;
    char str_sequenceNumber[12] = {'\0'};
    sequenceNumber = atoi(recordSequenceNumber) + 1;
    if (sequenceNumber >= MAX_INT) {
        sequenceNumber = 0;
    }
    sprintf(str_sequenceNumber, "%d", sequenceNumber);
    DIAG(DIAG_HIGH, "handleRecordSequenceNumber(): updating sequence number to %s", str_sequenceNumber);
    //printf("seq: %s\n",str_sequenceNumber);
    ADD_FIELD_DIAG("record-Sequence-Number", str_sequenceNumber);
}

/**
 * Calculate timeout for open CDR
 */
long getTimeoutAt() {
    long now = time(NULL);

    if (prm_SessionTimeoutInterimFactor > 0) {
        int timeout = prm_SessionTimeoutInterimInterval;
        if (timeout == 0) {
            const char* value = i_get("Acct-Interim-Interval"); // This can always be read from input
            timeout = atoi(value);
        }
        timeout *= prm_SessionTimeoutInterimFactor;
        if (timeout != 0 && timeout < prm_SessionTimeout) {
            return now + timeout;
        }
    }
    return now + prm_SessionTimeout;
}

/**
 * Construct session key from SessionIdFields
 */
void load_session_id(char** udr_session_id) {
    // The session id fields can always be read from input
    if (prm_SessionIdFields_length == 1) {
        *udr_session_id = (char*) i_get(prm_SessionIdFields[0]);
    } else {
        // Should I create an array to store parameter values
        // instead of reading them twice?
        int length = 1;
        int i = 0;
        for (; i < prm_SessionIdFields_length; i++) {
            length += strlen(i_get(prm_SessionIdFields[i]));
        }

        *udr_session_id =
                (char*) split_string_malloc(sizeof (char) * length);
        (*udr_session_id)[0] = '\0';
        for (i = 0; i < prm_SessionIdFields_length; i++) {
            strcat(*udr_session_id, i_get(prm_SessionIdFields[i]));
        }
    }

    DIAG(DIAG_LOW, "Loaded SessionId: %s", *udr_session_id);
}

/**
 * Auxiliary function for store_record_in_trs().
 *
 * Converts the current output record to a custom string format and saves it in
 * the output argument "a_result_string". The format is like this:
 * ([block_name|]*<field_name>=<field_value>|)*
 *
 * See also do_decompress().
 */
void do_compress(char* a_result_string) {
    *a_result_string = '\0';
    // trace
    dump_output_record_to_log();
    DIAG(DIAG_MEDIUM, "do_compress(): entering...");

    // cycle over the blocks and fields of the record
    int depth = 0;
    o_address(".");
    while (depth > -1) {
        if (o_next("") == 1) {
            // add a block
            if (o_is_block("") == 1) {
                o_enter();
                depth++;
                const char* name = o_get_address();
                int index = getBlockIndex(name);
                if (index != -1) {
                    DIAG(DIAG_HIGH, "block: %s, index: %d", name, index);
                    // currently 78 known blocks so three is enough (two numbers + '\0' character)
                    char indexAsString[3];
                    sprintf(indexAsString, "%d", index);
                    strcat(a_result_string, indexAsString);
                    strcat(a_result_string, DELIM_FIELD_S);
                } else {
                    DIAG(DIAG_LOW, "Unknown block: %s", name);
                    nb_msg1("AGGR022", name);
                }
            }// add a field
            else {
                const char* name = o_get_address();
                int index = getBlockIndex(name);
                if (index != -1) {
                    const char* value = o_get("");
                    DIAG(DIAG_HIGH, "field: %s, index: %d, value: %s", name, index, value);
                    char indexAsString[2];
                    sprintf(indexAsString, "%d", index);
                    strcat(a_result_string, indexAsString);
                    strcat(a_result_string, DELIM_FIELD_VALUE_S);
                    strcat(a_result_string, value);
                    strcat(a_result_string, DELIM_FIELD_S);
                } else {
                    DIAG(DIAG_LOW, "Unknown field: %s", name);
                    nb_msg1("AGGR022", name);
                }
            }
        } else if (depth > -1) {
            o_exit();
            depth--;
        }
    }
    o_address(".");

    // add the terminating character
    size_t len = strlen(a_result_string);
    a_result_string[len - 1] = '\0';
    DIAG(DIAG_HIGH, "Output record converted: %s", a_result_string);
    DIAG(DIAG_MEDIUM, "do_compress(): returning...");
}

/**
 * Converts the current output record to ASCII format and writes it into the log
 * file, if the diagnostic level is high enough.
 */
void dump_output_record_to_log(void) {
    DIAG(DIAG_HIGH, "Dumping record from output interface entered...");
    if (nb_diagnostic_level("APP") < DIAG_HIGH) {
        DIAG(DIAG_HIGH, "Dumping record from output interface  checking the diagnostic..");
        return;
    }
    DIAG(DIAG_HIGH, "Dumping record from output interface...");
    int depth = 0;
    o_address(".");
    while (depth > -1) {
        if (o_next("") == 1) {
            if (o_is_block("") == 1) {
                o_enter();
                depth++;
                DIAG(DIAG_HIGH, "B '%s'", o_get_address());
            } else {
                DIAG(DIAG_HIGH, "F '%s': '%s'", o_get_address(), o_get(""));
            }
        } else if (depth > -1) {
            o_exit();
            depth--;
        }
    }
    DIAG(DIAG_HIGH, "Dumping record from output interface finished...");
}

/**
 * Converts the current input record to ASCII format and writes it into the log
 * file, if the diagnostic level is high enough.
 */
void dump_input_record_to_log(void) {
    if (nb_diagnostic_level("APP") < DIAG_HIGH) {
        return;
    }
    DIAG(DIAG_HIGH, "Dumping record from input interface...");
    int depth = 0;
    i_address(".");
    while (depth > -1) {
        if (i_next("") == 1) {
            if (i_is_block("") == 1) {
                i_enter();
                depth++;
                DIAG(DIAG_HIGH, "B '%s'", i_get_address());
                // printf("%d B: %s\n", depth, i_get_address());
            } else {
                DIAG(DIAG_HIGH, "F '%s': '%s'", i_get_address(), i_get(""));
                // printf("%d F: %s\n", depth, i_get_address());
            }
        } else if (depth > -1) {
            i_exit();
            depth--;
        }
    }
    DIAG(DIAG_HIGH, "Dumping record from input interface finished...");
}

/**
 * Auxiliary function for retrieve_record_from_trs_to_output_interface().
 *
 * Creats a new output record, parses the passed string and saves the
 * information in the output record. The format of the string is like this:
 * ([block_name|]*<field_name>=<field_value>|)*
 *
 * See also output_record_to_string().
 */
void do_decompress(const char *a_string) {
    // variables
    char **fv_pairs = NULL;
    int token_count = 0;

    DIAG(DIAG_HIGH, "string_to_output_record() entered...");

    // create an empty output record
    nb_new_record();

    // TODO - remove this calculation after the migration to the new field
    // separator is finished;
    // just use DELIM_FIELD_C in the call to split_string()
    char *firstNewDelimiter = strchr(a_string, DELIM_FIELD_C);
    char fieldsDelimiter =
            (firstNewDelimiter != NULL) ? DELIM_FIELD_C : DELIM_FIELD_C_OLD;

    // split the custom string by the separator character '\n'
    token_count = split_string(&fv_pairs, a_string, fieldsDelimiter);

    // iterate over the particles of the split string
    char *tmp_fv_pair;
    char *tmp_value_separator;
    int tmp_name_components_num;
    int tmp_name_components_num_previous = 0;
    int i;
    for (i = 0; i < token_count; i++) {

        tmp_fv_pair = fv_pairs[i];
        // the presence of the space character ' ' marks whether the particle is
        // for a block field or a simple field
        tmp_value_separator = strchr(tmp_fv_pair, DELIM_FIELD_VALUE_C);
        int is_block = tmp_value_separator == NULL;
        DBBlock dbBlock;
        // it is a block field
        if (is_block) {
            dbBlock = getBlockByIndex(tmp_fv_pair);
        }// it is a simple field
        else {
            // separate the name from the value of the field
            *tmp_value_separator = '\0';
            dbBlock = getBlockByIndex(tmp_fv_pair);
        }

        // if this field needs to be added in an upper block level than the
        // previous field, move out of blocks as many times as needed
        tmp_name_components_num = dbBlock.level;
        while (tmp_name_components_num_previous-- >
                tmp_name_components_num) {
            o_exit();
        }
        tmp_name_components_num_previous = tmp_name_components_num;

        char* current = dbBlock.shortName;
        // it is a block field
        if (is_block) {
            // add the block to the output record
            DIAG(DIAG_HIGH,
                    "adding %s, %s",
                    dbBlock.longName,
                    current);
            o_add_block_enter(current);
        }// it is a simple field
        else {
            // add the field to the output record 
            DIAG(DIAG_HIGH,
                    "adding %s, %s: %s",
                    dbBlock.longName,
                    current,
                    tmp_value_separator + 1);
            o_add_field(current,
                    tmp_value_separator + 1);
        }
    }

    // free memory
    split_string_free_array(fv_pairs, token_count);

    // set the current position in the output record to root, in case we want to
    // add more fields to it
    o_address(".");

    // trace
    DIAG(DIAG_HIGH, "string_to_output_record():");
    dump_output_record_to_log();
}

/**
 * This function is checking TRS utilization and returns 1 of it's lower than the limit
 */
int check_utilization(int limit) {
    int result = 0;

    DIAG(DIAG_HIGH, "check_utilization(): entered the function ...");

    if (sql_execute(handleSelectUtilization) == TRS_ERROR) {
        DIAG(DIAG_HIGH, "check_utilization(): Cannot obtain TRS utilization. Aborting ...");

        nb_msg("TRSLD201");
        nb_abort();
    }

    if (sql_fetch(handleSelectUtilization) == (TRS_ERROR | TRS_NO_DATA_FOUND)) {
        DIAG(DIAG_HIGH, "check_utilization(): Cannot fetch TRS utilization data. Aborting ...");

        nb_msg("TRSLD202");
        nb_abort();
    }

    char aloc_size[30] = {'\0'};
    char inuse_size[30] = {'\0'};
    char code[20] = {'\0'};

    snprintf(aloc_size, sizeof (aloc_size), "%s", sql_get_value(handleSelectUtilization, 0));
    snprintf(inuse_size, sizeof (inuse_size), "%s", sql_get_value(handleSelectUtilization, 1));
    snprintf(code, sizeof (code), "%s", sql_get_error_code());

    sql_close(handleSelectUtilization);

    if (aloc_size[0] == '\0' || inuse_size[0] == '\0' || code[0] != '\0') {
        DIAG(DIAG_HIGH, "check_utilization(): [%s] TRS utilization data not available '%s/%s'. Aborting ...", code, aloc_size, inuse_size);

        nb_msg("TRSLD203");
        nb_abort();
    }

    float utilization = (atof(inuse_size) * 1.0) / (atof(aloc_size) * 1.0);
    float max_utilization = limit / 100.0;

    if (utilization < max_utilization) {
        result = 1;
    }

    DIAG(DIAG_HIGH, "check_utilization(): returning '%d' ...", result);

    return result;
}

/**
 * Function for inserting a single data record into the TRS table
 *
 */
void insert_into_TRS(const char* tmp_session_id, const char* tmp_cdr_type, Record *record) {

    char timeoutAt [20] = {'\0'};
    sprintf(timeoutAt, "%ld", getTimeoutAt());

    DIAG(DIAG_MEDIUM, "insert_into_TRS(): tmp_session_id '%s', tmp_cdr_type '%s', timeoutAt '%s'", tmp_session_id, tmp_cdr_type, timeoutAt);
    log_TRS_rec(record, "insert_into_TRS");

    if ((sql_set_parameter(handleInsertQuery, 0, tmp_session_id) != TRS_SUCCESS)
            || (sql_set_parameter(handleInsertQuery, 1, record->record) != TRS_SUCCESS)
            || (sql_set_parameter(handleInsertQuery, 2, record->rec_num) != TRS_SUCCESS)
            || (sql_set_parameter(handleInsertQuery, 3, record->buffered_acr_num) != TRS_SUCCESS)
            || (sql_set_parameter(handleInsertQuery, 4, tmp_cdr_type) != TRS_SUCCESS)
            || (sql_set_parameter(handleInsertQuery, 5, record->udr_type) != TRS_SUCCESS)
            || (sql_set_parameter(handleInsertQuery, 6, timeoutAt) != TRS_SUCCESS)
            || (sql_set_parameter(handleInsertQuery, 7, record->start_lost) != TRS_SUCCESS)
            || (sql_execute(handleInsertQuery) != TRS_SUCCESS)
            || (sql_close(handleInsertQuery) != TRS_SUCCESS)) {

        DIAG(DIAG_LOW, "insert_into_TRS(): Error while inserting record in TRS. Aborting ...");
        nb_msg("AGGR009");
        nb_abort();
    }
}

/**
 * Function for updating data record which is already present in the TRS table
 *
 */
void update_TRS(const char* tmp_session_id, const char* tmp_cdr_type, const char* tmp_rec_num_to_update, Record *record) {

    DIAG(DIAG_MEDIUM, "update_TRS(): tmp_session_id '%s', tmp_cdr_type '%s', tmp_rec_num_to_update '%s'", tmp_session_id, tmp_cdr_type, tmp_rec_num_to_update);
    log_TRS_rec(record, "update_TRS");

    if ((sql_set_parameter(handleUpdateQuery, 0, record->record) != TRS_SUCCESS)
            || (sql_set_parameter(handleUpdateQuery, 1, record->rec_num) != TRS_SUCCESS)
            || (sql_set_parameter(handleUpdateQuery, 2, record->buffered_acr_num) != TRS_SUCCESS)
            || (sql_set_parameter(handleUpdateQuery, 3, record->udr_type) != TRS_SUCCESS)
            || (sql_set_parameter(handleUpdateQuery, 4, tmp_session_id) != TRS_SUCCESS)
            || (sql_set_parameter(handleUpdateQuery, 5, tmp_cdr_type) != TRS_SUCCESS)
            || (sql_set_parameter(handleUpdateQuery, 6, tmp_rec_num_to_update) != TRS_SUCCESS)
            || (sql_execute(handleUpdateQuery) != TRS_SUCCESS)
            || (sql_close(handleUpdateQuery) != TRS_SUCCESS)) {

        DIAG(DIAG_LOW, "update_TRS(): Error while updating record in TRS.errcode: %d ; errmsg %s Aborting ...", sql_get_error_code(), sql_get_error_message());
        nb_msg("AGGR009");
        nb_abort();
    }
}

void update_timeout_TRS(const char* tmp_session_id, const char* tmp_cdr_type) {
    char timeoutAt [20] = {'\0'};
    sprintf(timeoutAt, "%ld", getTimeoutAt());

    DIAG(DIAG_MEDIUM, "update_timeout_TRS():tmp_session_id '%s', tmp_cdr_type '%s', timeout_at '%s'", tmp_session_id, tmp_cdr_type, timeoutAt);
    if ((sql_set_parameter(handleUpdateTimeoutQuery, 0, timeoutAt) != TRS_SUCCESS)
            || (sql_set_parameter(handleUpdateTimeoutQuery, 1, tmp_session_id) != TRS_SUCCESS)
            || (sql_set_parameter(handleUpdateTimeoutQuery, 2, tmp_cdr_type) != TRS_SUCCESS)
            || (sql_execute(handleUpdateTimeoutQuery) != TRS_SUCCESS)
            || (sql_close(handleUpdateTimeoutQuery) != TRS_SUCCESS)) {

        DIAG(DIAG_LOW, "update_timeout_TRS(): Error while updating record in TRS.errcode: %d ; errmsg %s Aborting ...", sql_get_error_code(), sql_get_error_message());
        nb_msg("AGGR009");
        nb_abort();
    }

}

/**
 * Function for deleting records based on session id and cdr type from TRS table
 *
 */
void delete_from_TRS(const char* tmp_session_id, const char* tmp_cdr_type) {
    DIAG(DIAG_MEDIUM, "delete_from_TRS() (SESSION_ID, CDR_TYPE) set ('%s', '%s')", tmp_session_id, tmp_cdr_type);
    if ((sql_set_parameter(handleDeleteQuery, 0, tmp_session_id) != TRS_SUCCESS)
            || (sql_set_parameter(handleDeleteQuery, 1, tmp_cdr_type) != TRS_SUCCESS)
            || (sql_execute(handleDeleteQuery) != TRS_SUCCESS)
            || (sql_close(handleDeleteQuery) != TRS_SUCCESS)) {

        DIAG(DIAG_LOW, "delete_from_TRS(): Error while deleting record from TRS. Aborting ...");
        nb_msg("AGGR009");
        nb_abort();
    }
}

/**
 * Function for deleting a single record based on record number (used for out of order handling)
 *
 */
void delete_rec_from_TRS(const char* tmp_session_id, const char* tmp_cdr_type, const char* tmp_rec_num) {
    DIAG(DIAG_MEDIUM, "delete_rec_from_TRS() (SESSION_ID, CDR_TYPE, REC_NUM) set to ('%s', '%s', '%s')", tmp_session_id, tmp_cdr_type, tmp_rec_num);
    if ((sql_set_parameter(handleDeleteRecQuery, 0, tmp_session_id) != TRS_SUCCESS)
            || (sql_set_parameter(handleDeleteRecQuery, 1, tmp_cdr_type) != TRS_SUCCESS)
            || (sql_set_parameter(handleDeleteRecQuery, 2, tmp_rec_num) != TRS_SUCCESS)
            || (sql_execute(handleDeleteRecQuery) != TRS_SUCCESS)
            || (sql_close(handleDeleteRecQuery) != TRS_SUCCESS)) {

        DIAG(DIAG_LOW, "delete_rec_from_TRS(): Error while deleting record from TRS. Aborting ...");
        nb_msg("AGGR009");
        nb_abort();
    }
}

/**
 * Function for fetching all records for session from TRS table by given session_id, returns number of selected rows
 *
 */
int select_records_from_trs(const char* tmp_session_key, const char* tmp_cdr_type, Record tmp_rec[]) {
    DIAG(DIAG_MEDIUM, "select_records_from_trs(): tmp_session_key '%s', tmp_cdr_type: '%s'", tmp_session_key, tmp_cdr_type);

    // read the record in the custom string format and save it in "tmp_rec" array
    if ((sql_set_parameter(handleSelectQuery, 0, tmp_session_key) != TRS_SUCCESS)
            || (sql_set_parameter(handleSelectQuery, 1, tmp_cdr_type) != TRS_SUCCESS)
            || (sql_execute(handleSelectQuery) != TRS_SUCCESS)) {

        DIAG(DIAG_LOW, "select_records_from_trs():errcode '%d' errmsg '%s'", sql_get_error_code(), sql_get_error_message());
        nb_msg("AGGR008");
        nb_abort();
    }

    int selected_row = -1;
    while (sql_fetch(handleSelectQuery) == TRS_SUCCESS) {
        selected_row++;
        DIAG(DIAG_MEDIUM, "select_records_from_trs() fetching a row number: %d", selected_row);
        strcpy(tmp_rec[selected_row].record, sql_get_value(handleSelectQuery, 0));
        strcpy(tmp_rec[selected_row].rec_num, sql_get_value(handleSelectQuery, 1));
        strcpy(tmp_rec[selected_row].udr_type, sql_get_value(handleSelectQuery, 2));
        strcpy(tmp_rec[selected_row].buffered_acr_num, sql_get_value(handleSelectQuery, 3));
        strcpy(tmp_rec[selected_row].start_lost, sql_get_value(handleSelectQuery, 4));
        log_TRS_rec(&tmp_rec[selected_row], "select_records_from_trs");
    }

    sql_close(handleSelectQuery);
    int num_selected_rows = selected_row + 1;
    DIAG(DIAG_MEDIUM, "select_records_from_trs() end with value num_selected_rows %d", num_selected_rows);
    return num_selected_rows;
}

void log_TRS_rec(Record *record, const char* functionName) {
    DIAG(DIAG_HIGH, "%s(): Record: rec_num '%s', buffered_acr_num '%s', start_lost '%s', record '%s'"
            , functionName, record->rec_num, record->buffered_acr_num, record->start_lost, record->record);
}

/**
 * Function for fetching number of records for session in TRS (needed because we need to allocate size for record struct
 * before calling select_record_from_trs
 *
 */
int get_number_of_records_from_trs(const char* tmp_session_key, const char* tmp_cdr_type) {
    DIAG(DIAG_MEDIUM, "get_number_of_records_from_trs(): tmp_session_key '%s', tmp_cdr_type: '%s'", tmp_session_key, tmp_cdr_type);

    // read the record in the custom string format and save it in "tmp_record"
    if ((sql_set_parameter(handleRecCountQuery, 0, tmp_session_key) != TRS_SUCCESS)
            || (sql_set_parameter(handleRecCountQuery, 1, tmp_cdr_type) != TRS_SUCCESS)
            || (sql_execute(handleRecCountQuery) != TRS_SUCCESS)) {

        DIAG(DIAG_LOW, "get_number_of_records_from_trs(): errcode '%d', errmsg '%s'", sql_get_error_code(), sql_get_error_message());
        nb_msg("AGGR008");
        nb_abort();
    }

    int number_of_rows = 0;
    if (sql_fetch(handleRecCountQuery) == TRS_SUCCESS) {
        number_of_rows = atoi(sql_get_value(handleRecCountQuery, 0));
    }

    sql_close(handleRecCountQuery);
    DIAG(DIAG_MEDIUM, "get_number_of_records_from_trs(): number of records was %d", number_of_rows);
    return number_of_rows;
}

/**
 * Function for fetching timeouted records from TRS table by given timeout interval. Also this function is deletind fetched records.
 *
 */
void fetch_time_out_data(int flush) {
    DIAG(DIAG_HIGH, "fetch_time_out_data(): entered the function");


    if (flush == 1) {
        DIAG(DIAG_MEDIUM, "fetch_time_out_data(): handleAllSelectQuery");
        if (sql_execute(handleAllSelectQuery) != TRS_SUCCESS) {
            DIAG(DIAG_LOW, "fetch_time_out_data (): Error while selecting all records from TRS. Aborting ...");
            nb_msg("AGGR013");
            nb_abort();
        }
        do_fetch_time_out_data(handleAllSelectQuery);
    } else {
        char current_time_epoch [20] = {'\0'};
        sprintf(current_time_epoch, "%ld", time(NULL));
        DIAG(DIAG_MEDIUM, "fetch_time_out_data(): handleTimeoutSelectQuery with %s", current_time_epoch);
        if ((sql_set_parameter(handleTimeoutSelectQuery, 0, current_time_epoch) != TRS_SUCCESS)
                || (sql_execute(handleTimeoutSelectQuery) != TRS_SUCCESS)) {

            DIAG(DIAG_LOW, "fetch_time_out_data (): Error while selecting timeout records from TRS. Aborting ...");
            nb_msg("AGGR013");
            nb_abort();
        }
        do_fetch_time_out_data(handleTimeoutSelectQuery);
    }
}

void do_fetch_time_out_data(int queryId) {
    int result = 0;

    //fetching the data selected for write out
    while ((result = sql_fetch(queryId)) != TRS_NO_DATA_FOUND) {
        if (result == TRS_ERROR) {
            DIAG(DIAG_LOW, "do_fetch_time_out_data(): Select fetch failed [%d]: %s", sql_get_error_code(), sql_get_error_message());
            nb_abort();
        } else if (result == TRS_SUCCESS) {
            char* session_id = sql_get_value(queryId, 0);
            char* cdr_type = sql_get_value(queryId, 1);

            // Get number of records (needed because we need to know beforehand the size of records array
            int number_of_records = get_number_of_records_from_trs(session_id, cdr_type);

            Record trs_records[number_of_records];

            if (number_of_records > 0) {
                // Fetch all records for this session and CDR type from TRS (records will be stored in records array)
                int found_records = select_records_from_trs(session_id, cdr_type, trs_records);
                if (number_of_records != found_records) {
                    DIAG(DIAG_LOW, "do_fetch_time_out_data():  Number of found records was different from number of records (%d != %d)!"
                            , found_records, number_of_records);
                    nb_msg("AGGR008");
                    nb_abort();
                }
            }

            int i;
            int start_lost = atoi(trs_records[0].start_lost);
            int interim_lost = 2; // 2 means we can't know
            int stop_lost = 1;
            int firt_interim_lost = 1;

            int previousRecNum = -1;
            int thisRecNum = 0;

            for (i = 0; i < number_of_records; i++) {
                int udr_type = atoi(trs_records[i].udr_type);

                int isPartial = 0;
                int sendCDR = -1; // Don't do sendCDR handling at all

                thisRecNum = atoi(trs_records[i].rec_num);

                if (thisRecNum == 1 && udr_type == 3) {
                    firt_interim_lost = 0; // first_interim is found 
                }

                if (previousRecNum != -1) {
                    // Check if interim was lost
                    if ((interim_lost != 1 && thisRecNum > previousRecNum + 1) || firt_interim_lost) {
                        DIAG(DIAG_MEDIUM, "do_fetch_time_out_data(): Interim lost detected between record numbers '%d' - '%d'", thisRecNum, previousRecNum);
                        interim_lost = 1;
                    }
                }

                previousRecNum = thisRecNum;

                // Check if this is an out of order stop request
                if (stop_lost == 1 && udr_type == 4) {
                    DIAG(DIAG_MEDIUM, "Out of order STOP record found, setting stop lost to 0");
                    stop_lost = 0;
                }


                if (i == 0) {
                    // This is the first trs record (aggregated record, or out of order record with lowest rec num in case no aggregated record exists)
                    RecordValue value;
                    initializeRecordValue(&value, trs_records[i].record);
                    write_ASN1_record(START, cdr_type, '\0', &value, 1);
                    split_string_free_array(value.values, value.length);
                } else {
                    partialReason reason = EXPIRATION;
                    do_decompress(trs_records[i - 1].record);
                    aggregateCDR(session_id, cdr_type, "0", &trs_records[i], udr_type, "", &sendCDR, &reason, &isPartial, 1); // empty for rec to update, since we don't update trs
                }

            }

            if (stop_lost == 0 && interim_lost == 2) {
                DIAG(DIAG_MEDIUM, "Stop received and no gap in interims found -> no interim was lost");
                interim_lost = 0;
            }

            if (interim_lost != 1 && firt_interim_lost) {
                interim_lost = 1;
            }

            // write incomplete CDR to output
            handleRecordSequenceNumber(0, 0);
            // no difference with FLUSH/EXPIRATION
            write_record_to_output(start_lost, interim_lost, stop_lost, INTERIM, cdr_type, EXPIRATION, "", 0);

            delete_from_TRS(session_id, cdr_type);

            DIAG(DIAG_HIGH, "do_fetch_time_out_data(): calling trs_commit(). ");
        }
    }
    sql_close(queryId);

    DIAG(DIAG_HIGH, "do_fetch_time_out_data(): calling trs_commit(). ");
    if (trs_commit() == TRS_ERROR) {
        trs_rollback();
    }

    DIAG(DIAG_HIGH, "fetch_time_out_data(): end");
}

/**
 * This function writes a CDR to output
 * 
 */
void write_record_to_output(int tmp_start_lost, int interim_lost, int stop_lost, flag recordType, const char *cdr_type, partialReason reason, const char* triggerType, int isPartial) {

    DIAG(DIAG_MEDIUM, "write_record_to_output(): isPartial '%d'", isPartial);

    // remove event time tracking field from file
    DEL_FIELD_DIAG("timestamp");
    DEL_FIELD_DIAG("timestamps");

    // Remove Trigger-Type from file
    DEL_FIELD_DIAG("Trigger-Type");

    // Map fields which are only calculated for actual CDRs
    map_causeForRecordClosing(recordType, reason, triggerType);
    DEL_FIELD_DIAG("Cause-Code"); // Not needed anymore after map_causeForRecordClosing
    map_duration(prm_AddDurationMilli); // needs to be mapped after map_recordClosureTime

    // Introduce these variables needed for SDP Media Component old/new handling so they can be used later
    Field sdpNewFields[100];
    int sdpNewFieldCount = 0;

    int sdpTriggerTypeHandling = o_field_exists(".list-Of-SDP-Media-ComponentsOld");
    if (sdpTriggerTypeHandling) {
        Block blocks[MAX_BLOCK_DEPTH];
        Field sdpOldFields[100];
        int sdpOldFieldCount;

        // Read new SDP Media Components fields to memory
        CLEAR_CHAR_ARRAY(blocks[0].blockName);
        sdpNewFieldCount = readBlockHierarchy(sdpNewFields, blocks, ".list-Of-SDP-Media-Components", 0, 0, 0);
        DEL_BLOCK_DIAG(".list-Of-SDP-Media-Components");

        // Read old SDP Media Components fields to memory		
        CLEAR_CHAR_ARRAY(blocks[0].blockName); // re-use blocks array by setting end marker to the beginning
        sdpOldFieldCount = readBlockHierarchy(sdpOldFields, blocks, ".list-Of-SDP-Media-ComponentsOld", 0, 0, 0);

        // Write old SDP Media Components fields to output as .list-Of-SDP-Media-Components (will get sent out)
        write_fields_to_out(sdpOldFieldCount, sdpOldFields, 1, "list-Of-SDP-Media-ComponentsOld", "list-Of-SDP-Media-Components");

        // Delete old SDP Media Components from output (we will not send those out)
        DEL_BLOCK_DIAG(".list-Of-SDP-Media-ComponentsOld");
    }

    map_localRecordSequenceNumber(EVENT); //call local for all records
    //map_aCName();
    //map_valueaddedServiceIdentity();
    //map_userChargingCategory();

    if (tmp_start_lost || interim_lost || stop_lost) {

        DIAG(DIAG_MEDIUM, "write_record_to_output() creating Incomplete-CDR-Indication block");
        o_address(".");
        ADD_BLOCK_DIAG("incomplete-CDR-Indication");

        if (tmp_start_lost == 0) {
            // start is arrived 
            ADD_FIELD_DIAG("aCRStartLost", "0");
        } else {
            // NO start is arrived
            ADD_FIELD_DIAG("aCRStartLost", "1");
        }

        if (interim_lost == 0) {
            ADD_FIELD_DIAG("aCRInterimLost", "0");
        } else if (interim_lost == 1) {
            ADD_FIELD_DIAG("aCRInterimLost", "1");
        } else {
            ADD_FIELD_DIAG("aCRInterimLost", "2");
        }

        if (stop_lost == 0) {
            // stop is arrived 
            ADD_FIELD_DIAG("aCRStopLost", "0");
        } else {
            // NO stop is arrived
            ADD_FIELD_DIAG("aCRStopLost", "1");
        }
        END_BLOCK_DIAG();
    }

    dump_output_record_to_log();

    if (strcmp(cdr_type, "0") == 0) {
        audit_out_type("sCSCFRecord");
        nb_write_record(CSCFOUTPUT);
    } else {
        audit_out_type("aSRecord");
        nb_write_record(OUTPUT);
    }

    DIAG(DIAG_MEDIUM, "node_process(): writing session record to output '%s'", OUTPUT);

    if (isPartial) {
        DIAG(DIAG_HIGH, "write_record_to_output(): adding record back to output");

        if (sdpTriggerTypeHandling) {
            // Delete OLD list-Of-SDP-Media-Components block before adding the new
            DEL_BLOCK_DIAG(".list-Of-SDP-Media-Components");

            // write NEW list-Of-SDP-Media-Components back so it's stored in the database
            int i;
            for (i = 0; i < sdpNewFieldCount; i++) {
                DIAG(DIAG_HIGH, "write_record_to_output(): LISTING: %s, %s, %d", sdpNewFields[i].fieldName, sdpNewFields[i].fieldValue, sdpNewFields[i].blocks[0].blockIndex);
            }
            write_fields_to_out(sdpNewFieldCount, sdpNewFields, 0, NULL, NULL);
        }

        // Handle post partial CDR handling (e.g. remove per CDR fields like duration)
        postPartialCDRHandling();
    }
}

/**
 * Splits the string "aString" into a list of strings. The delimiter character
 * is "aDelimiter". The result is saved in *aTokensArray.
 *
 * *aTokensArray must be freed by the calling function only via the function
 * split_string_free_array().
 */
int split_string(char ***aTokensArray, const char *aString, char aDelimiter) {
    // count the number of tokens
    int i;
    int tokensNumber = 1;
    for (i = 0; aString[i] != '\0'; i++) {
        if (aString[i] == aDelimiter) {
            tokensNumber++;
        }
    }

    // allocate memory for the resulting array of strings
    *aTokensArray = (char**) split_string_malloc(sizeof (char*) * tokensNumber);

    // allocate memory for the resulting strings
    int tmpTokenIndex = 0;
    int tmpTokenLength = 1;
    for (i = 0; aString[i] != '\0'; i++) {
        // the end of the current token is reached
        if (aString[i] == aDelimiter) {
            // allocate memory for it
            (*aTokensArray)[tmpTokenIndex] =
                    (char*) split_string_malloc(sizeof (char) * tmpTokenLength);

            // prepare to allocate memory for the next token
            tmpTokenLength = 0;
            tmpTokenIndex++;
        }

        // continue calculating the length of the current token
        tmpTokenLength++;
    }
    // allocate memory for the last token
    (*aTokensArray)[tmpTokenIndex] =
            (char*) split_string_malloc(sizeof (char) * tmpTokenLength);

    // copy the tokens from the input string into the output strings
    tmpTokenIndex = 0;
    char *tmpSourceChar = (char*) aString;
    char *tmpDestChar = (*aTokensArray)[tmpTokenIndex];
    while (*tmpSourceChar != '\0') {
        // the end of the current token is not reached
        if (*tmpSourceChar != aDelimiter && *tmpSourceChar != '\0') {
            *tmpDestChar = *tmpSourceChar;
            tmpDestChar++;
        }// the end of the current token is reached
        else {
            *tmpDestChar = '\0';
            tmpTokenIndex++;
            tmpDestChar = (*aTokensArray)[tmpTokenIndex];
        }

        tmpSourceChar++;
    }
    *tmpDestChar = '\0';

    return tokensNumber;
}

/**
 * Auxiliary function for split_string().
 *
 * Allocates a memory block of the given size in bytes and returns a pointer to
 * it. If the memory can not be allocated, a message is written in the log,
 * error "AGGR021" is written in the UI and the node aborts.
 */
void* split_string_malloc(int aSize) {
    // allocate the memory
    char *result = malloc(aSize);

    // check the operation
    if (result == NULL) {
        char* msg = "split_string(): Unable to allocate memory. Aborting...";
        DIAG(DIAG_LOW, msg);
        nb_msg("AGGR021");
        nb_abort();
    }

    // return result
    return result;
}

/**
 * "aTokensArray" and "aTokensNumber" are the output of function split_string().
 * This function frees the memory allocated for the list of strings which is
 * pointed by "aTokensArray".
 */
void split_string_free_array(char **aTokensArray, int aTokensNumber) {
    // release the memory of the strings
    int i;
    DIAG(DIAG_HIGH, "split_string_free_array(): free string array size of %d from memory", aTokensNumber);
    for (i = 0; i < aTokensNumber; i++) {
        free(aTokensArray[i]);
    }

    // release the memory of the array
    free(aTokensArray);
}

int write_ASN1_record(flag recordType, const char *cdr_type, const char* triggerType, RecordValue *trs_record, int readFromRecord) {
    int returnCode = RETURN_SUCCESS;
    const char* record;
    switch (recordType) {
        case EVENT: record = "EVENT";
            break;
        case START: record = "START";
            break;
        case INTERIM: record = "INTERIM";
            break;
        case STOP: record = "STOP";
            break;
        default: record = "UNKNOWN";
            DIAG(DIAG_MEDIUM, "write_ASN1_record(): %s record, returning...", record);
            return returnCode;
    }
    DIAG(DIAG_MEDIUM, "write_ASN1_record(): RECORD %s", record);

    if (strcmp(cdr_type, "0") == 0) {
        returnCode = map_sCSCFRecord(recordType, triggerType, triggerTypeOneEnabled, trs_record, readFromRecord, 1);
    } else {
        returnCode = map_aSRecord(recordType, triggerType, triggerTypeOneEnabled, trs_record, readFromRecord, 0);
    }

    DIAG(DIAG_HIGH, "write_ASN1_record(): returning...");
    return returnCode;
}

