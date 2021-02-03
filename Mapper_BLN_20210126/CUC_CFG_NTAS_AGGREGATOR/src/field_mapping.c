#include "field_mapping.h"

/*
 ************************ field conversion/mapping functions ************************
 */

int getRecordType(int nodeFunctionality) {
    switch (nodeFunctionality) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            nodeFunctionality += 63;
            return nodeFunctionality;
        case 7: return 82;
        case 8: return 84;
        case 9: return 85;
        case 10: return 101;
        case 11: return 70;
        case 20: return 102;
        case 24: return 254;
        case 25: return 255;
    }

    return -1;
}

void setRecordType(char* outputType, int typeId) {
    switch (typeId) {
        case 63:
            strcpy(outputType, "sCSCFRecord");
            break;
        case 64:
            strcpy(outputType, "pCSCFRecord");
            break;
        case 65:
            strcpy(outputType, "iCSCFRecord");
            break;
        case 66:
            strcpy(outputType, "mRFCRecord");
            break;
        case 67:
            strcpy(outputType, "mGCFRecord");
            break;
        case 68:
            strcpy(outputType, "bGCFRecord");
            break;
        case 69:
        case 254:
        case 255:
            strcpy(outputType, "aSRecord");
            break;
        case 70:
            strcpy(outputType, "eCSCFRecord");
            break;
        case 82:
            strcpy(outputType, "iBCFRecord");
            break;
    }
}

int mappingRuleSkipped(const char* outputField, mappingRule rule, flag recordType) {
    if (rule == FIRST_NOT_NULL && o_field_exists(outputField)) {
        return 1;
    }
    if (rule == START_EVENT && recordType != START && recordType != EVENT) {
        return 1;
    }
    if (rule == STOP_EVENT && recordType != STOP && recordType != EVENT) {
        return 1;
    }
    if (rule == STOP_ONLY && recordType != STOP) {
        return 1;
    }

    return 0;
}

void mappingDirect(const char* inputField, const char* outputField, mappingRule rule, flag recordType, RecordValue *record, int readFromRecord) {
    if (mappingRuleSkipped(outputField, rule, recordType)) {
        return;
    }
    if (readFromRecord) {
        copyFieldToOut(outputField, record);
        return;
    }
    ADD_FIELD_DIAG(outputField, i_get(inputField));
}

void mappingDirectBlock(const char* inputField, const char* outputBlock, const char* outputField, mappingRule rule, flag recordType, RecordValue *record, int readFromRecord) {
    if (mappingRuleSkipped(outputBlock, rule, recordType)) {
        return;
    }

    if (readFromRecord) {
        copyBlockToOut(outputBlock, record);
        return;
    }
    if (i_field_exists(inputField)) {
        if (o_field_exists(outputBlock)) {
            clearOutBlock(outputBlock);
        }
        ENTER_BLOCK_DIAG(outputBlock);
        ADD_FIELD_DIAG(outputField, i_get(inputField));
        END_BLOCK_DIAG();
    }
}

void mappingDirectBlockBA(const char* inputField, const char* outputBlock, const char* outputField, mappingRule rule, flag recordType, RecordValue *record, int readFromRecord) {
    if (mappingRuleSkipped(outputBlock, rule, recordType)) {
        return;
    }

    if (readFromRecord) {
        copyBlockToOut(outputBlock, record);
        return;
    }
    if (i_field_exists(inputField)) {
        if (o_field_exists(outputBlock)) {
            clearOutBlock(outputBlock);
        }
        ENTER_BLOCK_DIAG(outputBlock);
        add_ba_field_diag(outputField, i_get(inputField));
        END_BLOCK_DIAG();
    }
}

void mappingTimestamp(const char* inputField, const char* outputField, mappingRule rule, flag recordType, RecordValue *record, int readFromRecord) {
    if (mappingRuleSkipped(outputField, rule, recordType)) {
        return;
    }
    if (readFromRecord) {
        copyFieldToOut(outputField, record);
        return;
    }
    char out[20] = {'\0'}; // 18 chars
    epoch_to_compactLocal(out, i_get(inputField), 1);
    ADD_FIELD_DIAG(outputField, out);
}

void map_recordType(RecordValue *record, int readFromRecord, int isSCSCF) {
    // Fixed value: 69 (CMCC VoLTE)a
    if (isSCSCF) {
        ADD_FIELD_DIAG("record-Type", "63");
    } else {
        ADD_FIELD_DIAG("record-Type", "69");
    }
}

void map_retransmission(flag recordType, RecordValue *record, int readFromRecord) {
    //changed to be rewritten when start comes after interim //cpt2v2m
    if (readFromRecord && fieldExistsInTRSRecord(".retransmission", record)) {
        o_set("retransmission", "");
        DIAG(DIAG_HIGH, "ADD_FIELD_DIAG(): F retransmission ");
        o_begin();
    } else if (!readFromRecord && i_field_exists(".Header-Retransmit")) {
        o_set("retransmission", "");
        DIAG(DIAG_HIGH, "ADD_FIELD_DIAG(): F retransmission ");
        o_begin();
    }
}

void map_sDPMediaIdentifier(flag recordType, RecordValue *record, int readFromRecord) {
    if (readFromRecord) {
        copyFieldToOut(".sDP-Media-Identifier", record);
        return;
    }

    if (i_field_exists(".CMCC-SDP-Media-Identifier")) {
        ADD_FIELD_DIAG("sDP-Media-Identifier", i_get(".CMCC-SDP-Media-Identifier"));
        ADD_FIELD_DIAG("sDP-Media-Identifier_LATEST", i_get(".CMCC-SDP-Media-Identifier")); // we might overwrite the sDP-Media-Identifier so keep another copy so we can save it
    }

}

void map_serviceIdentity(flag recordType, RecordValue *record, int readFromRecord) {
    if (readFromRecord) {
        copyRecordFieldsRemoveDuplicate(".service-Identity.Service-Identity", ".service-Identity", "Service-Identity", record);
        return;
    }

    i_address(".");
    i_enter();

    if (i_field_exists(".Service-Information.IMS-Information.CMCC-Service-Identity")) {
        i_address(".Service-Information.IMS-Information");
        i_enter();
 
        while (i_next("CMCC-Service-Identity")) {
            addFieldRemoveDuplicate("service-Identity", "Service-Identity", i_get("CMCC-Service-Identity"));
        }

        i_exit();
    }

    i_address(".");

}

void map_tadsIndication(flag recordType, RecordValue *record, int readFromRecord) {
    if (readFromRecord) {
        copyFieldToOut(".tADS-Indication", record);
        return;
    }

    if (i_field_exists(".TADS-Identifier")) {
        ADD_FIELD_DIAG("tADS-Indication", i_get(".TADS-Identifier"));
    }

}

void map_serviceInteractInfo(flag recordType, RecordValue *record, int readFromRecord) {
    mappingDirectBlockBA(".Service-Interact-Info.Executed-Service", ".service-Interact-Info", "executed-Service",
            MAPPING_SERVICE_INTERACT_INFO, recordType, record, readFromRecord);
}

void map_sNMDServiceType(flag recordType, RecordValue *record, int readFromRecord) {
    if (readFromRecord) {
        copyFieldToOut(".sNMD-Service-Type", record);
        return;
    }

    if (i_field_exists(".SNMD-Service-Type")) {
        add_ba_field_diag("sNMD-Service-Type", i_get(".SNMD-Service-Type"));
    }

}

void map_sIPMethod(flag recordType, RecordValue *record, int readFromRecord) {
    if (recordType == EVENT) { // .Accounting-Record-Type == 1
        ADD_FIELD_DIAG("sIP-Method", i_get(".Service-Information.IMS-Information.Event-Type.SIP-Method"));
    }
}

void map_roleOfNode(flag recordType, RecordValue *record, int readFromRecord) {
    mappingDirect(".Service-Information.IMS-Information.Role-Of-Node", ".role-of-Node",
            MAPPING_ROLE_OF_NODE, recordType, record, readFromRecord);
}

void map_nodeAddress(flag recordType, RecordValue *record, int readFromRecord) {
    mappingDirectBlock(".Origin-Host", ".node-Address", "domainName",
            MAPPING_NODE_ADDRESS, recordType, record, readFromRecord);
}

void map_sessionId(flag recordType, RecordValue *record, int readFromRecord) {
    mappingDirect(".Service-Information.IMS-Information.User-Session-Id", ".session-ID",
            MAPPING_SESSION_ID, recordType, record, readFromRecord);
}

void map_listOfCallingPartyAddress(flag recordType, RecordValue *record, int readFromRecord) {
    // Take first not null value
    if (!o_field_exists("list-Of-Calling-Party-Address")) {
        if (readFromRecord) {
            copyRecordFieldsRemoveDuplicate(".list-Of-Calling-Party-Address.tEL-URI", ".list-Of-Calling-Party-Address", "tEL-URI", record);
            copyRecordFieldsRemoveDuplicate(".list-Of-Calling-Party-Address.sIP-URI", ".list-Of-Calling-Party-Address", "sIP-URI", record);
            return;
        }
        const char* CallingPartyAddress = i_get(".Service-Information.IMS-Information.Calling-Party-Address");

        if (*CallingPartyAddress != '\0') {
            i_address(".Service-Information.IMS-Information");
            i_enter();

            while (i_next("Calling-Party-Address")) {
                CallingPartyAddress = i_get("Calling-Party-Address");
                map_involvedPartyRemoveDuplicate(".list-Of-Calling-Party-Address", CallingPartyAddress);
            }

            i_address(".");
        }
    }
}

void map_calledPartyAddress(flag recordType, RecordValue *record, int readFromRecord) {
    if (mappingRuleSkipped(".called-Party-Address", MAPPING_CALLED_PARTY_ADDRESS, recordType)) {
        return;
    }

    //changed to be rewritten when start comes after interim //cpt2v2m
    if (readFromRecord) {
        clearAndCopyBlockToOut(".called-Party-Address", record);
        return;
    }
    const char* CalledPartyAddress = i_get(".Service-Information.IMS-Information.Called-Party-Address");
    if (*CalledPartyAddress != '\0') {
        if (o_field_exists("called-Party-Address")) {
            clearOutBlock("called-Party-Address");
        }

        map_involvedParty("called-Party-Address", CalledPartyAddress);
    }
}

void map_serviceRequestTimeStamp(flag recordType, RecordValue *record, int readFromRecord) {
    mappingTimestamp(".Service-Information.IMS-Information.Time-Stamps.SIP-Request-Timestamp", ".service-Request-Time-Stamp",
            MAPPING_SERVICE_REQUEST_TIMESTAMP, recordType, record, readFromRecord);
}

void map_serviceRequestTimeStampFraction(flag recordType, RecordValue *record, int readFromRecord) {
    mappingDirect(".Service-Information.IMS-Information.Time-Stamps.SIP-Request-Timestamp-Fraction", ".serviceRequestTimeStampFraction",
            MAPPING_SERVICE_REQUEST_TIMESTAMP_FRACTION, recordType, record, readFromRecord);
}

void map_serviceDeliveryStartTimeStamp(flag recordType, RecordValue *record, int readFromRecord) {
    mappingTimestamp(".Service-Information.IMS-Information.Time-Stamps.SIP-Response-Timestamp", ".service-Delivery-Start-Time-Stamp",
            MAPPING_SERVICE_DELIVERY_START_TIMESTAMP, recordType, record, readFromRecord);
}

void map_serviceDeliveryStartTimeStampFraction(flag recordType, RecordValue *record, int readFromRecord) {
    mappingDirect(".Service-Information.IMS-Information.Time-Stamps.SIP-Response-Timestamp-Fraction", ".serviceDeliveryStartTimeStampFraction",
            MAPPING_SERVICE_DELIVERY_START_TIMESTAMP_FRACTION, recordType, record, readFromRecord);
}

void map_serviceDeliveryEndTimeStamp(flag recordType, RecordValue *record, int readFromRecord) {
    mappingTimestamp(".Service-Information.IMS-Information.Time-Stamps.SIP-Request-Timestamp", ".service-Delivery-End-Time-Stamp",
            MAPPING_SERVICE_DELIVERY_END_TIMESTAMP, recordType, record, readFromRecord);
}

void map_serviceDeliveryEndTimeStampFraction(flag recordType, RecordValue *record, int readFromRecord) {
    mappingDirect(".Service-Information.IMS-Information.Time-Stamps.SIP-Request-Timestamp-Fraction", ".serviceDeliveryEndTimeStampFraction",
            MAPPING_SERVICE_DELIVERY_END_TIMESTAMP_FRACTION, recordType, record, readFromRecord);
}

void map_recordOpeningTime(flag recordType, RecordValue *record, int readFromRecord) {
    if (readFromRecord) {
        if (!o_field_exists("record-Opening-Time")) {
            copyFieldToOut(".record-Opening-Time", record);
        }
        return;
    }

    if (recordType == START || recordType == EVENT || (!o_field_exists("record-Opening-Time") && recordType == INTERIM)) {

        char recordOpeningTime[20] = {'\0'}; // 18 chars
        if (i_field_exists(".Service-Information.IMS-Information.Time-Stamps.SIP-Response-Timestamp")) {
            epoch_to_compactLocal(recordOpeningTime, i_get(".Service-Information.IMS-Information.Time-Stamps.SIP-Response-Timestamp"), 1);
            DIAG(DIAG_MEDIUM, "map_recordOpeningTime(): SIP-Response-Timestamp chosen for record-Opening-Time '%s'", recordOpeningTime);
        } else if (i_field_exists(".Event-Timestamp")) {
            epoch_to_compactLocal(recordOpeningTime, i_get(".Event-Timestamp"), 1);
            DIAG(DIAG_MEDIUM, "map_recordOpeningTime(): Event-Timestamp chosen for record-Opening-Time '%s'", recordOpeningTime);
        } else {
            localtime_to_compactLocal(recordOpeningTime);
            DIAG(DIAG_MEDIUM, "map_recordOpeningTime(): local time chosen for record-Opening-Time '%s'", recordOpeningTime);
        }

        ADD_FIELD_DIAG("record-Opening-Time", recordOpeningTime);

    }
}

void map_recordOpeningTimeFraction(flag recordType, RecordValue *record, int readFromRecord) {
    if (readFromRecord) {
        if (!o_field_exists("record-Opening-Time-Fraction")) {
            copyFieldToOut(".record-Opening-Time-Fraction", record);
        }
        return;
    }

    if (recordType == START || recordType == EVENT || (!o_field_exists("record-Opening-Time-Fraction") && recordType == INTERIM)) {

        char recordOpeningTimeFraction[20] = {'\0'}; // 18 chars

        if (i_field_exists(".Service-Information.IMS-Information.Time-Stamps.SIP-Response-Timestamp-Fraction")) {

            if (i_field_exists(".Event-Timestamp") && !i_field_exists(".Service-Information.IMS-Information.Time-Stamps.SIP-Response-Timestamp")) {
                // Do nothing
                strcpy(recordOpeningTimeFraction, i_get(".Event-Timestamp-Fraction")); 
                
            }  
            else {
                strcpy(recordOpeningTimeFraction, i_get(".Service-Information.IMS-Information.Time-Stamps.SIP-Response-Timestamp-Fraction"));
                DIAG(DIAG_MEDIUM, "map_recordOpeningTime(): SIP-Response-Timestamp chosen for record-Opening-Time-Fraction '%s'", recordOpeningTimeFraction);
            }
        } else if (i_field_exists(".Event-Timestamp-Fraction")) {
      
            if (i_field_exists(".Service-Information.IMS-Information.Time-Stamps.SIP-Response-Timestamp") && !i_field_exists(".Event-Timestamp")) {
                // Do nothing  
                strcpy(recordOpeningTimeFraction,i_get(".Service-Information.IMS-Information.Time-Stamps.SIP-Response-Timestamp-Fraction")); 
            }
            else {   
                strcpy(recordOpeningTimeFraction, i_get(".Event-Timestamp-Fraction"));
                printf("open_frac %s\n",recordOpeningTimeFraction);
                DIAG(DIAG_MEDIUM, "map_recordOpeningTimeFraction(): Event-Timestamp-Fraction chosen for record-Opening-Time-Fraction '%s'", recordOpeningTimeFraction);
            } 
        } else {
            strcpy(recordOpeningTimeFraction,"0");
            DIAG(DIAG_MEDIUM, "map_recordOpeningTimeFraction(): local time chosen for record-Opening-Time-Fraction '%s'", recordOpeningTimeFraction);
        }

        ADD_FIELD_DIAG("record-Opening-Time-Fraction", recordOpeningTimeFraction);

    }
}


void map_recordClosureTime(flag recordType, RecordValue *record, int readFromRecord) {
    if (readFromRecord) {
        copyFieldToOut(".record-Closure-Time", record);
        return;
    }
    if (recordType == STOP) {
        char recordClosureTime[20] = {'\0'}; // 18 chars		
        if (i_field_exists(".Service-Information.IMS-Information.Time-Stamps.SIP-Request-Timestamp")) {
            epoch_to_compactLocal(recordClosureTime, i_get(".Service-Information.IMS-Information.Time-Stamps.SIP-Request-Timestamp"), 1);
        } else if (i_field_exists(".Event-Timestamp")) {
            epoch_to_compactLocal(recordClosureTime, i_get(".Event-Timestamp"), 1);
        } else {
            localtime_to_compactLocal(recordClosureTime);
        }
        ADD_FIELD_DIAG("record-Closure-Time", recordClosureTime);

    } else if (recordType == EVENT || recordType == INTERIM) {
        char recordClosureTime[20] = {'\0'}; // 18 chars		
        if (i_field_exists(".Service-Information.IMS-Information.Time-Stamps.SIP-Response-Timestamp")) {
            epoch_to_compactLocal(recordClosureTime, i_get(".Service-Information.IMS-Information.Time-Stamps.SIP-Response-Timestamp"), 1);
            DIAG(DIAG_MEDIUM, "map_recordClosureTime(): SIP-Response-Timestamp chosen for record-Closure-Time '%s'", recordClosureTime);
        } else if (i_field_exists(".Event-Timestamp")) {
            epoch_to_compactLocal(recordClosureTime, i_get(".Event-Timestamp"), 1);
            DIAG(DIAG_MEDIUM, "map_recordClosureTime(): Event-Timestamp chosen for record-Closure-Time '%s'", recordClosureTime);
        } else {
            localtime_to_compactLocal(recordClosureTime);
            DIAG(DIAG_MEDIUM, "map_recordClosureTime(): local time chosen for record-Closure-Time '%s'", recordClosureTime);
        }
        ADD_FIELD_DIAG("record-Closure-Time", recordClosureTime);

    }
}

void map_recordClosureTimeFraction(flag recordType, RecordValue *record, int readFromRecord) {
    if (readFromRecord) {
        copyFieldToOut(".record-Closure-Time-Fraction", record);
        return;
    }
    if (recordType == STOP) {
        char recordClosureTimeFraction[20] = {'\0'}; // 18 chars

        if (i_field_exists(".Service-Information.IMS-Information.Time-Stamps.SIP-Request-Timestamp-Fraction")) {
            
            if (i_field_exists(".Event-Timestamp") && !i_field_exists(".Service-Information.IMS-Information.Time-Stamps.SIP-Request-Timestamp")){
                // Do nothing 
                strcpy(recordClosureTimeFraction,i_get(".Event-Timestamp-Fraction"));
            }   
            else {
                strcpy(recordClosureTimeFraction, i_get(".Service-Information.IMS-Information.Time-Stamps.SIP-Request-Timestamp-Fraction"));
            } 
        } else if (i_field_exists(".Event-Timestamp-Fraction")) {
            if (i_field_exists(".Service-Information.IMS-Information.Time-Stamps.SIP-Request-Timestamp") && !i_field_exists(".Event-Timestamp")){
                // Do nothing
                strcpy(recordClosureTimeFraction,i_get(".Service-Information.IMS-Information.Time-Stamps.SIP-Request-Timestamp-Fraction"));
            } 
            else {
                strcpy(recordClosureTimeFraction, i_get(".Event-Timestamp-Fraction"));
                printf("close frac: %s\n",recordClosureTimeFraction);
            }
        } else {
            strcpy(recordClosureTimeFraction,"0");
        }
        ADD_FIELD_DIAG("record-Closure-Time-Fraction", recordClosureTimeFraction);

    } else if (recordType == EVENT || recordType == INTERIM) {
        char recordClosureTimeFraction[20] = {'\0'}; // 18 chars
 
        if (i_field_exists(".Service-Information.IMS-Information.Time-Stamps.SIP-Response-Timestamp-Fraction")) {
            
            if (i_field_exists(".Event-Timestamp") && !i_field_exists(".Service-Information.IMS-Information.Time-Stamps.SIP-Response-Timestamp")) {
                // Do nothing 
                strcpy(recordClosureTimeFraction,i_get(".Event-Timestamp-Fraction"));
            } 
            else { 
                strcpy(recordClosureTimeFraction, i_get(".Service-Information.IMS-Information.Time-Stamps.SIP-Response-Timestamp-Fraction"));
                DIAG(DIAG_MEDIUM, "map_recordClosureTime(): SIP-Response-Timestamp-Fraction chosen for record-Closure-Time-Fraction '%s'"
                 , recordClosureTimeFraction);
            }
        } else if (i_field_exists(".Event-Timestamp-Fraction")) {
            
            if (i_field_exists(".Service-Information.IMS-Information.Time-Stamps.SIP-Response-Timestamp") && !i_field_exists(".Event-Timestamp")) {
                // Do nothing
                strcpy(recordClosureTimeFraction,i_get(".Service-Information.IMS-Information.Time-Stamps.SIP-Response-Timestamp-Fraction"));
            } 
            else { 
                strcpy(recordClosureTimeFraction, i_get(".Event-Timestamp-Fraction"));
                DIAG(DIAG_MEDIUM, "map_recordClosureTimeFraction(): Event-Timestamp-Fraction chosen for record-Closure-Time-Fraction '%s'"
                 , recordClosureTimeFraction);
            }
        } else {
            strcpy(recordClosureTimeFraction,"0");
            DIAG(DIAG_MEDIUM, "map_recordClosureTimeFraction(): local time chosen for record-Closure-Time-Fraction '%s'", recordClosureTimeFraction);
        }
        ADD_FIELD_DIAG("record-Closure-Time-Fraction", recordClosureTimeFraction);

    }
}

void map_event_timestamp(flag recordType) {
    char timestamp[20] = {'\0'}; // 18 chars
    if (i_field_exists(".Service-Information.IMS-Information.Time-Stamps.SIP-Response-Timestamp")) {
        epoch_to_compactLocal(timestamp, i_get(".Service-Information.IMS-Information.Time-Stamps.SIP-Response-Timestamp"), 1);
    } else {
        epoch_to_compactLocal(timestamp, i_get(".Event-Timestamp"), 1);
    }
    ADD_FIELD_DIAG("timestamp", timestamp);
}

void map_timestamps_block_exists(flag recordType) {
    if (i_is_block(".Service-Information.IMS-Information.Time-Stamps")) {
        ADD_FIELD_DIAG("timestamps", "1");
    }
}

void map_interOperatorIdentifiers(flag recordType, RecordValue *record, int readFromRecord) {
    int blockExists = 1;
    int i = 0;
    while (blockExists) {
        blockExists = map_interOperatorIdentifiersByIndex(recordType, record, readFromRecord, i++);
    }
    removeDuplicatesFromOut(".inter-Operator-Identifiers", "Inter-Operator-Identifiers");
}

int map_interOperatorIdentifiersByIndex(flag recordType, RecordValue *record, int readFromRecord, int i) {
    DIAG(DIAG_HIGH, "map_interOperatorIdentifiers(%d)", i);
    int blockExists = 0;
    if (readFromRecord) {
        blockExists = blockExistsInTRSRecordByIndex(".inter-Operator-Identifiers.Inter-Operator-Identifiers", record, i);
    } else {
        char field[70] = {'\0'};
        sprintf(field, ".Service-Information.IMS-Information.Inter-Operator-Identifier[%d]", i);
        blockExists = i_is_block(field);
    }
    if (blockExists) {
        char* I_Originating_IOI;
        char* I_Terminating_IOI;
        if (readFromRecord) {
            I_Originating_IOI = getFieldValueFromTRSRecordByIndex(".inter-Operator-Identifiers.Inter-Operator-Identifiers.originating-IOI", record, i);
            I_Terminating_IOI = getFieldValueFromTRSRecordByIndex(".inter-Operator-Identifiers.Inter-Operator-Identifiers.terminating-IOI", record, i);
        } else {
            char field1[85] = {'\0'};
            sprintf(field1, ".Service-Information.IMS-Information.Inter-Operator-Identifier[%d].Originating-IOI", i);
            char field2[85] = {'\0'};
            sprintf(field2, ".Service-Information.IMS-Information.Inter-Operator-Identifier[%d].Terminating-IOI", i);
            I_Originating_IOI = (char*) i_get(field1);
            I_Terminating_IOI = (char*) i_get(field2);
        }
        DIAG(DIAG_HIGH, "I_Originating_IOI = %s", I_Originating_IOI);
        DIAG(DIAG_HIGH, "I_Terminating_IOI = %s", I_Terminating_IOI);
        o_address(".");
        if (*I_Originating_IOI != '\0' || *I_Terminating_IOI != '\0') {
            // .inter-Operator-Identifiers.Inter-Operator-Identifiers
            ENTER_BLOCK_DIAG("inter-Operator-Identifiers");
            ADD_BLOCK_DIAG("Inter-Operator-Identifiers");
            ADD_FIELD_DIAG("originating-IOI", I_Originating_IOI);
            ADD_FIELD_DIAG("terminating-IOI", I_Terminating_IOI);
            EXIT_BLOCK_DIAG();
            EXIT_BLOCK_DIAG();
            i_address(".");
        }
        if (readFromRecord) {
            free(I_Originating_IOI);
            free(I_Terminating_IOI);
        }
    }
    return blockExists;
}

void map_localRecordSequenceNumber(flag recordType) {
    /* Unique record number within the CCF (within stream CDF_Aggregator_and_CGF in terms of the EL implementation of CCF). 
    min = 0, max = 4294967295 = 2^32 - 1 */

    const char* localRecordSequenceNumber = nb_store_get("localRecordSequenceNumber");
    unsigned int sequenceNumber = 1;
    char str_sequenceNumber[12] = {'\0'};

    if (*localRecordSequenceNumber != '\0') {
        sequenceNumber = atoi(localRecordSequenceNumber);
        if (sequenceNumber >= MAX_INT) sequenceNumber = 1;
    }
    sprintf(str_sequenceNumber, "%d", sequenceNumber++);

    ADD_FIELD_DIAG("local-Record-Sequence-Number", str_sequenceNumber);

    // update nb_store for localRecordSequenceNumber, sequenceNumber already incremented
    sprintf(str_sequenceNumber, "%d", sequenceNumber);
    nb_store_set("localRecordSequenceNumber", str_sequenceNumber);
}

void map_causeCode(flag recordType, RecordValue *record, int readFromRecord) {
    if (readFromRecord) {
        copyFieldToOut(".Cause-Code", record);
        return;
    }
    ADD_FIELD_DIAG("Cause-Code", i_get(".Service-Information.IMS-Information.Cause-Code"));
}

void map_callDescription(flag recordType, RecordValue *record, int readFromRecord) {
    if (readFromRecord) {
        copyFieldToOut(".call-Description", record);
        return;
    }
    ADD_FIELD_DIAG("call-Description", i_get(".Call-Description"));
}

void map_ctxGroupNumber(flag recordType, RecordValue *record, int readFromRecord) {
    if (readFromRecord) {
        copyFieldToOut(".ctxgroup-Number", record);
        return;
    }
    ADD_FIELD_DIAG("ctxgroup-Number", i_get(".CtxGroup-Number"));
}

void map_shortNumber(flag recordType, RecordValue *record, int readFromRecord) {
    if (readFromRecord) {
        copyFieldToOut(".short-Number", record);
        return;
    }
    ADD_FIELD_DIAG("short-Number", i_get(".Short-Number"));
}

void map_causeForRecordClosing(flag recordType, partialReason reason, const char* triggerType) {
    /*
            0: if ACR stop / event is received, and the cause-code AVP has value <=0.
            1: if ACR stop / event is received, and the cause-code AVP has value > 0.
            3: No ACR is received by CCF for a session after a predefined period of time (Long duration call, LDC, timer expires), or generically this CDR is written because of timeout.
            4. service change (audio / video change), i.e., ACR with Trigger-Type AVP == 1 is received, which triggers the generation of a partial CDR.
            5. ACR with Trigger-Type AVP == 2, or any value other than 1 and 2, is received, which triggers the generation of a partial CDR.
            6. The number of Buffered ACRs reaches a pre-defined threshold. The priority of this cause is lower than all defined above.
     */
    if (recordType == STOP || recordType == EVENT) {
        char causeForRecordClosing[2] = {'\0'};
        if (atoi(o_get("Cause-Code")) > 0) {
            DIAG(DIAG_HIGH, "Cause-Code was > 0");
            strcpy(causeForRecordClosing, "1");
        } else {
            DIAG(DIAG_HIGH, "Cause-Code was <= 0");
            strcpy(causeForRecordClosing, "0");
        }

        ADD_FIELD_DIAG("cause-For-Record-Closing", causeForRecordClosing);
    } else if (recordType == INTERIM) {
        char causeForRecordClosing[2] = {'\0'};
        if (reason == EXPIRATION || reason == FLUSH) {
            strcpy(causeForRecordClosing, "3");
        } else if (reason == TRIGGER_TYPE && strcmp(triggerType, "\0") != 0) {
            if (strcmp(triggerType, "1") == 0) {
                strcpy(causeForRecordClosing, "4");
            } else {
                strcpy(causeForRecordClosing, "5");
            }
        } else {
            // None of the other reasons matched, so this has to be buffered ACR case
            strcpy(causeForRecordClosing, "6");
        }
        ADD_FIELD_DIAG("cause-For-Record-Closing", causeForRecordClosing);
    }
}

void map_iMSChargingIdentifier(flag recordType, RecordValue *record, int readFromRecord) {
    if (readFromRecord) {
        copyFieldToOut(".iMS-Charging-Identifier", record);
        return;
    }
    add_ba_field_diag("iMS-Charging-Identifier", i_get(".Service-Information.IMS-Information.IMS-Charging-Identifier"));
}

void map_aCName(flag recordType, RecordValue *record, int readFromRecord) {

    if (readFromRecord) {
        copyFieldToOut(".aCName", record);
        return;
    }

    ADD_FIELD_DIAG("aCName", i_get(".ACName"));
}

void map_valueaddedServiceIdentity(flag recordType, RecordValue *record, int readFromRecord) {

    mappingDirectBlock(".Valueadded-Service-Identity", ".valueadded-Service-Identity", "INTEGER",
            MAPPING_VALUE_ADDED_SERVICE, recordType, record, readFromRecord);

}

void map_userChargingCategory(flag recordType, RecordValue *record, int readFromRecord) {

    if (readFromRecord) {
        copyFieldToOut(".user-Charging-Category", record);
        return;
    }

    ADD_FIELD_DIAG("user-Charging-Category", i_get(".User-Charging-Category"));
}

/**
 * This function copies output block list-Of-SDP-Media-Components to list-Of-SDP-Media-ComponentsOld
 */
void store_oldListOfSDPMediaComponents() {
    DIAG(DIAG_HIGH, "store_oldListOfSDPMediaComponents(): Storing list-Of-SDP-Media-Components to list-Of-SDP-Media-ComponentsOld");
    Field fields[100];
    Block blocks[MAX_BLOCK_DEPTH];
    CLEAR_CHAR_ARRAY(blocks[0].blockName);
    int fieldCount = readBlockHierarchy(fields, blocks, ".list-Of-SDP-Media-Components", 0, 0, 0);
    DIAG(DIAG_HIGH, "store_oldListOfSDPMediaComponents(): fieldCount was %d", fieldCount);
    write_fields_to_out(fieldCount, fields, 1, "list-Of-SDP-Media-Components", "list-Of-SDP-Media-ComponentsOld");
}

/**
 * This function writes an array of Field structs to output. It's also possible to replace one block name
 * 
 * @param fieldCount the amount of fields in the fields array
 * @param fields the actual array holding the Field structs which need to be written out
 * @param replaceBlockName if set to 1, this function will use the next two parameters to handle block name replacement
 * @param originalBlockName if replaceBlockName is set to 1, this parameter is used to indicate the original block name which we need to replace
 * @param replacementBlockName if replaceBlockName is set to 1, this parameter is used to indicate the replacement block name
 */
void write_fields_to_out(int fieldCount, Field fields[], int replaceBlockName, char* originalBlockName, char* replacementBlockName) {
    DIAG(DIAG_HIGH, "write_fields_to_out(): function called with fieldCount %d, replaceBlockName %d, originalBlockName %s, replacementBlockName %s"
            , fieldCount, replaceBlockName, originalBlockName, replacementBlockName);
    int i;
    for (i = 0; i < fieldCount; i++) {
        if (strcmp(fields[i].fieldValue, "")) {
            DIAG(DIAG_HIGH, "write_fields_to_out(): Writing field '%s' with value '%s'", fields[i].fieldName, fields[i].fieldValue);
            if (replaceBlockName) {
                int blockIndex = 0;
                while (strcmp(fields[i].blocks[blockIndex].blockName, "") != 0) {
                    if (strcmp(fields[i].blocks[blockIndex].blockName, originalBlockName) == 0) {
                        // correct block found for replacement
                        DIAG(DIAG_HIGH, "write_fields_to_out(): Replacing block name '%s' with new name '%s'", fields[i].blocks[blockIndex].blockName, replacementBlockName);
                        sprintf(fields[i].blocks[blockIndex].blockName, replacementBlockName);
                        break; // No need to continue searching, since we only want to replace the name of one block
                    }
                }
            }
            write_field_to_output(&fields[i]);
        } else {
            DIAG(DIAG_HIGH, "write_fields_to_out(): Skipping empty field '%s' with value '%s'", fields[i].fieldName, fields[i].fieldValue);
        }
    }
}

/**
 * This function reads an entire block hierarchy into a Field array, indexing is supported.
 * 
 * Remember to have an end marker at the end of blocks array (Block struct with an empty blockName)
 * 
 * blockIndex doesn't matter for first run, since there it's given in blockName, e.g.:
 * .list.block[2]
 * 
 * @param fields array for fields which will be read from hierarchy
 * @param blocks array for blocks which will be kept up to date when running this function
 * @param blockName name for the block to read (when calling this function, give name in absolute address format, including possible indexing)
 * @param blockIndex give 0 when calling this function from outside
 * @param nextBlockLevel give 0 when calling this function from outside
 * @param nextFieldIndex give 0 when calling this function from outside
 */
int readBlockHierarchy(Field fields[], Block blocks[], const char* blockName, int blockIndex, int nextBlockLevel, int nextFieldIndex) {
    DIAG(DIAG_HIGH, "readBlockHierarchy(): Reading block hierarchy for %s", blockName);

    // Init blocks array or keep it up to date (depending on is this the first run, or recursive run)
    if (nextBlockLevel == 0) {
        // This is the first run since no blocks have been added, read hierarchy from blockName
        char **blockNames = NULL;
        int blockCount = split_string(&blockNames, blockName, '.');

        int emptyElementsFound = 0;
        int i;
        for (i = 0; i < blockCount; i++) {
            int index = 0;
            char **indexingSplitOne = NULL;
            int tokens = split_string(&indexingSplitOne, blockNames[i], '[');

            if (strcmp(indexingSplitOne[0], "") == 0) {
                // In case string starts with a '.', the first element will be empty
                emptyElementsFound++;
                split_string_free_array(indexingSplitOne, tokens);
                continue;
            }

            if (tokens > 1) {
                char **indexingSplitTwo = NULL;
                int tokensTwo = split_string(&indexingSplitTwo, indexingSplitOne[1], ']');
                index = atoi(indexingSplitTwo[0]);
                split_string_free_array(indexingSplitTwo, tokensTwo);
            }
            int actualIndex = i - emptyElementsFound;

            blocks[actualIndex + 1] = blocks[actualIndex]; // Move endMarker forward in array
            sprintf(blocks[actualIndex].blockName, indexingSplitOne[0]);
            blocks[actualIndex].blockIndex = index;

            DIAG(DIAG_HIGH, "readBlockHierarchy(): Adding block to index %d, block name is %s", blocks[actualIndex].blockIndex, blocks[actualIndex].blockName);
            split_string_free_array(indexingSplitOne, tokens);
        }
        DIAG(DIAG_HIGH, "readBlockHierarchy(): First block after %s with index %d", blocks[0].blockName, blocks[0].blockIndex);
        split_string_free_array(blockNames, blockCount);
        nextBlockLevel = blockCount - emptyElementsFound;
    } else {
        DIAG(DIAG_HIGH, "readBlockHierarchy(): nextBlockLevel is %d", nextBlockLevel);
        blocks[nextBlockLevel + 1] = blocks[nextBlockLevel]; // Move endMarker forward in array
        sprintf(blocks[nextBlockLevel].blockName, blockName);
        blocks[nextBlockLevel].blockIndex = blockIndex;
        nextBlockLevel++;
    }

    DIAG(DIAG_HIGH, "readBlockHierarchy(): Incremented nextBlockLevel to %d", nextBlockLevel);

    // Read possible fields under this block
    enter_existing_output_block(blocks);
    DIAG(DIAG_HIGH, "readBlockHierarchy(): Address is %s", o_get_address());

    if (nextBlockLevel < 1 || strcmp(o_get_address(), ".") == 0) {
        DIAG(DIAG_HIGH, "readBlockHierarchy(): No block found from %s", blockName);
        return 0;
    }

    int size = 10;
    Block subBlocks[size];
    int subBlockCount = 0;
    while (o_next("")) {
        const char* fieldName = o_get_field_name();
        if (!o_is_field("")) {
            // store block for later handling
            int i;
            int index = 0;
            for (i = 0; i < subBlockCount; i++) {
                if (strcmp(subBlocks[i].blockName, fieldName) == 0) {
                    index++;
                }
            }

            sprintf(subBlocks[subBlockCount].blockName, fieldName);
            subBlocks[subBlockCount].blockIndex = index;

            DIAG(DIAG_HIGH, "readBlockHierarchy(): %s was a block with index %d", subBlocks[subBlockCount].blockName, subBlocks[subBlockCount].blockIndex);
            subBlockCount++;
        } else {
            // handle field
            readField(&fields[nextFieldIndex], blocks, nextBlockLevel, "");
            DIAG(DIAG_HIGH, "readBlockHierarchy(): Added field '%s' with value %s, fieldIndex was %d", fields[nextFieldIndex].fieldName, fields[nextFieldIndex].fieldValue, nextFieldIndex);
            nextFieldIndex++;
        }
    }

    if (subBlockCount > 0) {
        // Handle possible sub blocks	
        DIAG(DIAG_HIGH, "readBlockHierarchy(): Starting to handle sub blocks, count %d", subBlockCount);
        int i;
        for (i = 0; i < subBlockCount; i++) {
            nextFieldIndex = readBlockHierarchy(fields, blocks, subBlocks[i].blockName, subBlocks[i].blockIndex, nextBlockLevel, nextFieldIndex);
        }
    }
    // Exit this level of hierarchy
    nextBlockLevel--;
    blocks[nextBlockLevel] = blocks[nextBlockLevel + 1]; // move end marker
    DIAG(DIAG_HIGH, "readBlockHierarchy(): Leaving hierarchy level for block %s, returning fieldCount %d", blockName, nextFieldIndex);
    return nextFieldIndex;
}

void readField(Field *outputField, Block blocks[], int blockCount, char* address) {
    // handle field	

    // Populate blocks array for new field
    int i;
    for (i = 0; i <= blockCount; i++) {
        sprintf(outputField->blocks[i].blockName, blocks[i].blockName);
        outputField->blocks[i].blockIndex = blocks[i].blockIndex;
    }

    int changeAddress = 0;
    if (strcmp(address, "") != 0) {
        changeAddress = 1;
        o_address(address);
    }

    outputField->fieldName = o_get_field_name();
    outputField->fieldValue = o_get("");
    DIAG(DIAG_HIGH, "readField(): Field read '%s' with value %s", outputField->fieldName, outputField->fieldValue);

    if (changeAddress) {
        o_address(".");
    }
}

void map_listOfSDPMediaComponents(flag recordType, const char* triggerType, int triggerTypeOneEnabled, RecordValue *record, int readFromRecord) {
    if (readFromRecord) {
        const char *repeatBlockNames[3];
        repeatBlockNames[0] = "List-of-Media-Components";
        repeatBlockNames[1] = "SDP-Media-Component";
        repeatBlockNames[2] = NULL;
        updateBlockToOut(".list-Of-SDP-Media-Components", record, repeatBlockNames, "GraphicString");
        removeDuplicatesFromOut(".list-Of-SDP-Media-Components", "List-of-Media-Components");
        return;
    }

    if (i_is_block(".Service-Information.IMS-Information.SDP-Media-Component")) {
        if (triggerTypeOneEnabled && strcmp(triggerType, "1") == 0 && o_field_exists(".list-Of-SDP-Media-Components")) {
            // Trigger-Type is 1, we need to send OLD value in output CDR, store old value here
            DIAG(DIAG_HIGH, "map_listOfSDPMediaComponents(): Trigger-Type was one, storing OLD value for list-Of-SDP-Media-Components");
            store_oldListOfSDPMediaComponents();

            // clean up old list-Of-SDP-Media-Components (so we don't aggregate the NEW value on top of that)
            DEL_BLOCK_DIAG(".list-Of-SDP-Media-Components");
        }

        char sIPRequestTimestamp[20] = {'\0'}; // 18 chars
        epoch_to_compactLocal(sIPRequestTimestamp, i_get(".Service-Information.IMS-Information.Time-Stamps.SIP-Request-Timestamp"), 1);

        char sIPResponseTimestamp[20] = {'\0'}; // 18 chars
        epoch_to_compactLocal(sIPResponseTimestamp, i_get(".Service-Information.IMS-Information.Time-Stamps.SIP-Response-Timestamp"), 1);

        const char *sIPRequestTimestampFraction = i_get(".Service-Information.IMS-Information.Time-Stamps.SIP-Request-Timestamp-Fraction");
        const char *sIPResponseTimestampFraction = i_get(".Service-Information.IMS-Information.Time-Stamps.SIP-Response-Timestamp-Fraction");

        i_address(".Service-Information.IMS-Information");
        i_enter();

        ENTER_BLOCK_DIAG("list-Of-SDP-Media-Components"); // main block wrapper
        ADD_BLOCK_DIAG("List-of-Media-Components"); // internal block wrapper
        ADD_FIELD_DIAG("sIP-Request-Time-Stamp", sIPRequestTimestamp);
        ADD_FIELD_DIAG("sIP-Response-Time-Stamp", sIPResponseTimestamp);

        if (i_field_exists("SDP-Session-Description")) {
            ADD_BLOCK_DIAG("sDP-Session-Description");
            while (i_next("SDP-Session-Description")) {
                ADD_NEW_FIELD_DIAG("GraphicString", i_get("SDP-Session-Description"));
            }
            EXIT_BLOCK_DIAG();
        }

        ADD_FIELD_DIAG("sIP-Request-Timestamp-Fraction", sIPRequestTimestampFraction);
        ADD_FIELD_DIAG("sIP-Response-Timestamp-Fraction", sIPResponseTimestampFraction);

        /* loop all blocks and obtain needed fields */
        const char* mediaInitiatorFlag;
        const char* sdpType = '\0';
        ADD_BLOCK_DIAG("sDP-Media-Components");
        while (i_next("SDP-Media-Component")) {
            i_enter();
            o_begin();

            ADD_BLOCK_DIAG("SDP-Media-Component");
            ADD_FIELD_DIAG("sDP-Media-Name", i_get("SDP-Media-Name"));

            ADD_BLOCK_DIAG("sDP-Media-Descriptions");
            while (i_next("SDP-Media-Description")) {
                ADD_NEW_FIELD_DIAG("GraphicString", i_get("SDP-Media-Description"));
            }
            EXIT_BLOCK_DIAG();

            if (strlen(i_get("3GPP-Charging-ID")) > 0) {
                char gPRSChargingId[25] = {'\0'};
                sprintf(gPRSChargingId, "%ld", strtol(i_get("3GPP-Charging-ID"), NULL, 16));
                ADD_FIELD_DIAG("gPRS-Charging-Id", gPRSChargingId);
            }

            EXIT_BLOCK_DIAG(); // SDP-Media-Component
            mediaInitiatorFlag = i_get("Media-Initiator-Flag");
            sdpType = i_get("SDP-Type");
            i_exit(); // SDP-Media-Component

        } // end while
        EXIT_BLOCK_DIAG(); // sDP-Media-Components

        // Use value from lastest SDP-Media-Component
        if (strcmp(mediaInitiatorFlag, "0") == 0) {
            ADD_FIELD_DIAG("media-Initiator-Flag", mediaInitiatorFlag);
            DIAG(DIAG_HIGH, "map_listOfSDPMediaComponents(): media-Initiator-Flag added");
        }

        // Use value from lastest SDP-Media-Component
        ADD_FIELD_DIAG("sDP-Type", sdpType);

        END_BLOCK_DIAG(); // go to root in output

        i_address(".");

        removeDuplicatesFromOut(".list-Of-SDP-Media-Components", "List-of-Media-Components");
    }
}

void map_serviceReasonReturnCode(flag recordType, RecordValue *record, int readFromRecord) {
    if (readFromRecord) {
        copyFieldToOut(".service-Reason-Return-Code", record);
        return;
    }
    ADD_FIELD_DIAG("service-Reason-Return-Code", i_get(".Service-Information.IMS-Information.Cause-Code"));
}

void addOrUpdateBlock(const char* blockName, int index) {
    char block[40] = "";
    sprintf(block, "%s[%d]", blockName, index);
    if (o_is_block(block)) {
        DIAG(DIAG_HIGH, "addOrUpdateBlock(): updating %s", block);
        enterBlockWithIndex(block);
    } else {
        DIAG(DIAG_HIGH, "addOrUpdateBlock(): adding %s", block);
        ADD_BLOCK_DIAG(blockName);
    }
}

void enterBlockWithIndex(char* indexedBlockName) {
    o_address(indexedBlockName);
    o_enter();
}

void map_listOfMessageBodies(flag recordType, RecordValue *record, int readFromRecord) {
    if (readFromRecord) {
        const char* repeatBlockNames[2];
        repeatBlockNames[0] = "MessageBody";
        repeatBlockNames[1] = NULL;
        updateBlockToOut(".list-Of-Message-Bodies", record, repeatBlockNames, NULL);
        return;
    }

    if (i_is_block(".Service-Information.IMS-Information.Message-Body")) {
        i_address(".Service-Information.IMS-Information");
        i_enter();

        ENTER_BLOCK_DIAG("list-Of-Message-Bodies");
        int i = 0;
        while (i_next("Message-Body")) {
            i_enter();
            o_begin();
            ADD_BLOCK_DIAG("MessageBody");

            ADD_FIELD_DIAG("content-Type", i_get("Content-Type"));
            ADD_FIELD_DIAG("content-Disposition", i_get("Content-Disposition"));
            ADD_FIELD_DIAG("content-Length", i_get("Content-Length"));

            const char* originator;
            if (strcmp(i_get("Originator"), "0") == 0) {
                originator = i_get(".Service-Information.IMS-Information.Calling-Party-Address");
            } else {
                originator = i_get(".Service-Information.IMS-Information.Called-Party-Address");
            }

            char lc_originator[ strlen(originator) + 1 ];
            memset(lc_originator, '\0', strlen(originator));
            str_to_low(lc_originator, originator);

            ENTER_BLOCK_DIAG("originator");
            if (strncmp("tel:", lc_originator, 4) == 0) {
                ADD_FIELD_DIAG("tEL-URI", originator);
            } else {
                ADD_FIELD_DIAG("sIP-URI", originator);
            }
            EXIT_BLOCK_DIAG();

            EXIT_BLOCK_DIAG(); // MessageBodyblock
            i_exit();
            i++;
        }
        END_BLOCK_DIAG();

        i_address(".");
    }
}

void map_dialledPartyAddress(flag recordType, RecordValue *record, int readFromRecord) {
    if (readFromRecord) {
        copyFieldToOut(".dialled-Party-Address", record);
        return;
    }

    //Only originating ticket require this field
    if (strcmp(i_get(".Service-Information.IMS-Information.Role-Of-Node"), "0") == 0) {
        const char* firstRequestedPartyAddress = i_get(".Service-Information.IMS-Information.Requested-Party-Address[0]");
        if (*firstRequestedPartyAddress != '\0') {
            if (o_field_exists("dialled-Party-Address")) {
                clearOutBlock("dialled-Party-Address");
            }
            map_involvedParty("dialled-Party-Address", firstRequestedPartyAddress);
        } else {
            const char* CalledPartyAddress = i_get(".Service-Information.IMS-Information.Called-Party-Address");
            if (*CalledPartyAddress != '\0') {
                if (o_field_exists("dialled-Party-Address")) {
                    clearOutBlock("dialled-Party-Address");
                }
                map_involvedParty("dialled-Party-Address", CalledPartyAddress);
            }
        }
    }
}

/*
void map_callProperty(flag recordType, RecordValue *record, int readFromRecord) {
        if (readFromRecord) {
                copyFieldToOut(".Call-Property", record);
                return;
        }
        ADD_FIELD_DIAG("Call-Property", i_get(".Call-Property"));
}

void map_callingUserType(flag recordType, RecordValue *record, int readFromRecord) {
        if (readFromRecord) {
                copyFieldToOut(".Calling-User-Type", record);
                return;
        }
        ADD_FIELD_DIAG("Calling-User-Type", i_get(".Calling-User-Type"));
}*/


void map_accessNetworkInformation(flag recordType, RecordValue *record, int readFromRecord) {
    if (readFromRecord) {
        copyFieldToOut(".access-Network-Information", record);
        return;
    }
    add_ba_field_diag(".access-Network-Information", i_get(".Service-Information.IMS-Information.Access-Network-Information"));
}

void map_onlinechargingflag(flag recordType, RecordValue *record, int readFromRecord) {
    mappingDirect(".Service-Information.IMS-Information.CMCC-Online-Charging-Flag", ".online-Charging-Flag",
            MAPPING_ONLINE_CHARGING_FLAG, recordType, record, readFromRecord);
}

void map_cSLocationInformation(flag recordType, RecordValue *record, int readFromRecord) {
    if (readFromRecord) {
        // this will always add CS-Location-Information block so we need to remove duplicates
        const char* repeatBlockNames[2];
        repeatBlockNames[0] = "CS-Location-Information";
        repeatBlockNames[1] = NULL;
        updateBlockToOut(".cS-Location-Information", record, repeatBlockNames, NULL);
        removeDuplicatesFromOut(".cS-Location-Information", "CS-Location-Information");
        return;
    }
    // make sure there is at least one child
    if (i_size(".CS-Location-Information") > 0) {
        ENTER_BLOCK_DIAG(".cS-Location-Information");
        ADD_BLOCK_DIAG("CS-Location-Information");
        add_ba_field_diag("mscNumber", i_get(".CS-Location-Information.MSC-Number"));
	//        add_ba_field_diag("location-Area", i_get(".CS-Location-Information.Location-Area"));
        add_integer_field_diag("location-Area", i_get(".CS-Location-Information.Location-Area"));
        add_ba_field_diag("cell-Identification", i_get(".CS-Location-Information.Cell-Identification"));
	//        add_ba_field_diag("service-Area-Code", i_get(".CS-Location-Information.Service-Area-Code"));
	add_integer_field_diag("service-Area-Code", i_get(".CS-Location-Information.Service-Area-Code"));
        EXIT_BLOCK_DIAG();
        EXIT_BLOCK_DIAG();
        removeDuplicatesFromOut(".cS-Location-Information", "CS-Location-Information");
    }
}

/**
 * This function is used to calculate and map input blocks which contain a Value-Digits and Exponent field.
 * Note that before calling this function, the input address where we are reading from
 * needs to be at the level where the input field which has the Value-Digits and Exponent 
 * fields exists.
 * 
 * This function also converts the output into Octet String, since we need to do that to every exponent field.
 * 
 * @param inputFieldName name of the field from input (e.g. Scale-Factor)
 * @param outputFieldName name of the field in output (e.g. scaleFactor)
 */
void map_exponentField(char* inputFieldName, char* outputFieldName) {
    if (i_address(inputFieldName)) {
        i_enter();
        double valueDigits = atof(i_get("Value-Digits"));
        double exponent = i_field_exists("Exponent") ? atof(i_get("Exponent")) : 0;
        DIAG(DIAG_MEDIUM, "map_exponentField():  Calculating '%s' based on Value-Digits '%g' and Exponent '%g'", inputFieldName, valueDigits, exponent);
        double calculated = valueDigits * pow(10, exponent);
        char* outputValue = double_without_trailing_zeros_to_str(calculated);
        add_ba_field_diag(outputFieldName, outputValue);
        free(outputValue);
        i_exit();
    }
}

char* double_without_trailing_zeros_to_str(double input) {
    int size = snprintf(NULL, 0, "%f", input);
    char *output = malloc(size + 1);
    snprintf(output, size + 1, "%f", input);

    // remove trailing zeros
    int i;
    int end = size;
    for (i = size - 1; i >= 0; i--) {
        if (output[i] == '0') {
            if (end == i + 1) {
                end = i;
            }
        } else if (output[i] == '.') {
            if (end == i + 1) {
                end = i;
            }
            output[end] = '\0';
            break;
        }
    }

    return output;
}

void map_iMSVisitedNetworkIdentifier(flag recordType, RecordValue *record, int readFromRecord) {
    mappingDirect(".Service-Information.IMS-Information.IMS-Visited-Network-Identifier", ".ims-Visited-Network-Identifier",
            MAPPING_IMS_VISITED_NETWORK_IDENTIFIER, recordType, record, readFromRecord);
}

void map_subscriberEquipmentNumber(flag recordType, RecordValue *record, int readFromRecord) {
    if (readFromRecord) {
        copyBlockToOut(".subscriber-Equipment-Number", record);
        return;
    }

    if (i_is_block(".Service-Information.PS-Information.User-Equipment-Info")) {

        char* outputBlock = "subscriber-Equipment-Number";
        if (o_field_exists(outputBlock)) {
            clearOutBlock(outputBlock);
        }

        ENTER_BLOCK_DIAG(outputBlock);
        ADD_FIELD_DIAG("subscriber-Equipment-Number-Type", i_get(".Service-Information.PS-Information.User-Equipment-Info.User-Equipment-Info-Type"));
        add_ba_field_diag("subscriber-Equipment-Number-Data", i_get(".Service-Information.PS-Information.User-Equipment-Info.User-Equipment-Info-Value"));
        END_BLOCK_DIAG();
    }
}

void map_requestedPartyAddress(flag recordType, RecordValue *record, int readFromRecord) {
    if (mappingRuleSkipped("requested-Party-Address", MAPPING_REQUESTED_PARTY_ADDRESS, recordType)) {
        return;
    }
    if (readFromRecord) {
        copyBlockToOut(".requested-Party-Address", record);
        return;
    }
    const char* firstRequestedPartyAddress = i_get(".Service-Information.IMS-Information.Requested-Party-Address[0]");
    if (*firstRequestedPartyAddress != '\0') {
        if (o_field_exists("requested-Party-Address")) {
            clearOutBlock("requested-Party-Address");
        }

        map_involvedParty("requested-Party-Address", firstRequestedPartyAddress);
    }
}

void map_subscriptionID(flag recordType, RecordValue *record, int readFromRecord) {
    const char* outputBlock = "list-of-subscription-ID.SubscriptionID";
    if (readFromRecord) {
        copyBlockToOut(outputBlock, record);
        return;
    }

    const char* SubscriptionId = i_get("Service-Information.Subscription-Id.Subscription-Id-Type");
    if (*SubscriptionId != '\0') {

        i_address(".Service-Information");
        i_enter();

        ENTER_BLOCK_DIAG("list-of-subscription-ID");
        while (i_next("Subscription-Id")) {
            ADD_BLOCK_DIAG("SubscriptionID");
            ADD_NEW_FIELD_DIAG("subscriptionIDType", i_get("Subscription-Id.Subscription-Id-Type"));
            ADD_NEW_FIELD_DIAG("subscriptionIDData", i_get("Subscription-Id.Subscription-Id-Data"));
            EXIT_BLOCK_DIAG();
        }
        END_BLOCK_DIAG();

        i_address(".");
    }

}

void map_calledAssertedID(flag recordType, RecordValue *record, int readFromRecord) {

    const char* outputBlock = ".list-Of-Called-Asserted-Identity";
    if (readFromRecord) {
        copyBlockToOut(outputBlock, record);
        return;
    }

    const char* calledAssertedIdentity = i_get("Service-Information.IMS-Information.Called-Asserted-Identity");
    if (*calledAssertedIdentity != '\0') {

        i_address(".Service-Information.IMS-Information");
        i_enter();

        while (i_next("Called-Asserted-Identity")) {
            map_involvedPartyRemoveDuplicate(outputBlock, i_get("Called-Asserted-Identity"));
        }

        i_address(".");
    }

}

void map_chargedParty(flag recordType, RecordValue *record, int readFromRecord) {
    if (readFromRecord) {
        clearAndCopyBlockToOut(".charged-Party", record);
        return;
    }

    const char* chargedParty = i_get(".Service-Information.IMS-Information.Charged-Party");
    if (*chargedParty != '\0') {
        map_involvedParty("charged-Party", chargedParty);
    }
}

void map_callingParty(flag recordType, RecordValue *record, int readFromRecord) {
    if (readFromRecord) {
        clearAndCopyBlockToOut(".calling-Party", record);
        return;
    }

    const char* callingParty = i_get(".Calling-Party");
    if (*callingParty != '\0') {
        map_involvedParty("calling-Party", callingParty);
    }
}

void map_connectedNumber(flag recordType, RecordValue *record, int readFromRecord) {
    if (readFromRecord) {
        clearAndCopyBlockToOut(".connected-Number", record);
        return;
    }

    const char* connectedNumber = i_get(".Connected-Number");
    if (*connectedNumber != '\0') {
        map_involvedParty("connected-Number", connectedNumber);
    }
}

void map_involvedParty(const char* blockName, const char* fieldValue) {
    char lc_field[strlen(fieldValue) + 1];
    memset(lc_field, '\0', strlen(fieldValue));
    str_to_low(lc_field, fieldValue);

    if (o_field_exists(blockName)) {
        clearOutBlock(blockName);
    }
    ENTER_BLOCK_DIAG(blockName);
    if (strncmp("tel:", lc_field, 4) == 0) {
        ADD_NEW_FIELD_DIAG("tEL-URI", fieldValue);
    } else {
        ADD_NEW_FIELD_DIAG("sIP-URI", fieldValue);
    }
    END_BLOCK_DIAG();
}

void map_involvedPartyRemoveDuplicate(const char* blockName, const char* fieldValue) {
    char lc_field[strlen(fieldValue) + 1];
    memset(lc_field, '\0', strlen(fieldValue));
    str_to_low(lc_field, fieldValue);

    if (strncmp("tel:", lc_field, 4) == 0) {
        addFieldRemoveDuplicate(blockName, "tEL-URI", fieldValue);
    } else {
        addFieldRemoveDuplicate(blockName, "sIP-URI", fieldValue);
    }
}

int isFieldChanged(const char* inputFieldName, const char* outFieldName, int encodedOut, RecordValue *record, int readFromRecord) {
    int different = 0;
    const char* oldFieldValue = o_get(outFieldName);
    DIAG(DIAG_HIGH, "isFieldChanged(): oldFieldValue = '%s'", oldFieldValue);
    if (readFromRecord) {
        char* newFieldValue = getFieldValueFromTRSRecord(outFieldName, record);
        DIAG(DIAG_HIGH, "isFieldChanged(): newFieldValue = '%s'", newFieldValue);
        different = *newFieldValue != '\0' && strcmp(newFieldValue, oldFieldValue) != 0;
        free(newFieldValue);
    } else {
        const char* input = i_get(inputFieldName);
        DIAG(DIAG_HIGH, "isFieldChanged(): input = '%s'", input);
        if (*input != '\0') {
            if (encodedOut) {
                char newFieldValue[(strlen(input) * 2 + 1)];
                memset(newFieldValue, '\0', sizeof (newFieldValue));
                stringToByteArray(newFieldValue, input);
                DIAG(DIAG_HIGH, "isFieldChanged(): newFieldValue = '%s'", newFieldValue);
                different = strcmp(newFieldValue, oldFieldValue) != 0;
            } else {
                different = strcmp(input, oldFieldValue) != 0;
            }
        }
    }
    DIAG(DIAG_HIGH, "isFieldChanged(): different = '%d'", different);
    return different;
}

/**
 * This function needs to be called AFTER recordClosingTime and record-Opening-Time have
 * been mapped (it needs their values).
 * 
 * Duration only needs to be mapped when writing a CDR
 */
void map_duration(int mapDurationMilli) {
    const char* openingTime = o_get(".record-Opening-Time");
    const char* closureTime = o_get(".record-Closure-Time");
    const char* openingTimeFraction = o_get(".record-Opening-Time-Fraction");
    const char* closureTimeFraction = o_get(".record-Closure-Time-Fraction");

    DIAG(DIAG_HIGH, "map_duration(): read from output - record-Closure-Time: %s, record-Opening-Time: %s", closureTime, openingTime);
    DIAG(DIAG_HIGH, "map_duration(): read from output - record-Closure-Time-Fraction: %s, record-Opening-Time-Fraction: %s", closureTimeFraction, openingTimeFraction);

    if (strcmp(openingTime, "") == 0 || strcmp(closureTime, "") == 0) {
        DIAG(DIAG_HIGH, "map_duration(): not calculating duration because one of these is empty: record-Closure-Time: %s, record-Opening-Time: %s"
                , closureTime, openingTime);
        return;
    }

    char recordOpeningComparable[13] = {'\0'};
    char recordClosureComparable[13] = {'\0'};
    compactLocalToComparableDateString(openingTime, recordOpeningComparable);
    compactLocalToComparableDateString(closureTime, recordClosureComparable);
    DIAG(DIAG_HIGH, "map_duration(): record-Closure-Time: %s, record-Opening-Time: %s", recordClosureComparable, recordOpeningComparable);
 
    double durDouble = getTimeDiff(recordClosureComparable, recordOpeningComparable);
 
    if (mapDurationMilli)
    {
        // round duration - (>=0.5 -> 1, <0.5 -> 0) 
        durDouble = round(durDouble*10 + (atoi(closureTimeFraction) - atoi(openingTimeFraction))/100.0); // convert to 1/10 of a second unit.
    } 
    else 
    {
        durDouble = round(durDouble); // customer wants the unit to be seconds in this case
    }   

    // Make sure we don't calculate negative duration (could happen if timestamps are incorrect)
    if (durDouble < 0) {
        DIAG(DIAG_LOW, "map_duration(): duration calculation result was negative: '%g'", durDouble);
        durDouble = 0;
    }

    char* duration = double_without_trailing_zeros_to_str(durDouble);
    ADD_FIELD_DIAG("duration", duration);
    free(duration);
  
    // For testing purposes these fields can be present at the Aggregator output ... 
    //DEL_FIELD_DIAG("record-Opening-Time-Fraction");
    //DEL_FIELD_DIAG("record-Closure-Time-Fraction");
    
}

void map_trigger_type(flag recordType, const char* triggerType, RecordValue *record, int readFromRecord) {
    if (readFromRecord) {
        copyFieldToOut(".Trigger-Type", record);
        return;
    }
    ADD_FIELD_DIAG("Trigger-Type", triggerType);
}

void map_privateUserID(flag recordType, RecordValue *record, int readFromRecord) {
    //	if (!o_field_exists("privateUserID")) {
    if (readFromRecord) {
        copyFieldToOut(".privateUserID", record);
        return;
    }
    ADD_FIELD_DIAG("privateUserID", i_get(".User-Name"));
    //	}
}

void map_applicationServersInformation(flag recordType, RecordValue *record, int readFromRecord) {
    if (readFromRecord) {
        copyBlockToOut(".applicationServersInformation", record);
        return;
    }
    if (i_is_block(".Service-Information.IMS-Information.Application-Server-Information")) {
        i_address(".Service-Information.IMS-Information");
        i_enter();

        ENTER_BLOCK_DIAG("applicationServersInformation");
        while (i_next("Application-Server-Information")) {
            i_enter();
            o_begin();
            ADD_BLOCK_DIAG("ApplicationServersInformation");
            ADD_BLOCK_DIAG("applicationServersInvolved");
            ADD_FIELD_DIAG("domainName", i_get("Application-Server"));
            EXIT_BLOCK_DIAG();
            while (i_next("Application-Provided-Called-Party-Address")) {
                o_begin();
                ADD_BLOCK_DIAG("applicationProvidedCalledParties");
                const char* appProvidedCalledPartyAddr = i_get("Application-Provided-Called-Party-Address");
                char lc_associatedURI[strlen(appProvidedCalledPartyAddr) + 1];
                memset(lc_associatedURI, '\0', strlen(appProvidedCalledPartyAddr));
                str_to_low(lc_associatedURI, appProvidedCalledPartyAddr);
                if (strncmp("tel:", lc_associatedURI, 4) == 0) {
                    ADD_NEW_FIELD_DIAG("tEL-URI", appProvidedCalledPartyAddr);
                } else {
                    ADD_NEW_FIELD_DIAG("sIP-URI", appProvidedCalledPartyAddr);
                }
                EXIT_BLOCK_DIAG();
            }
            EXIT_BLOCK_DIAG();
            i_exit();
        }
        END_BLOCK_DIAG();

        i_address(".");
    }
}

void map_total_volume(flag recordType, RecordValue *record, int readFromRecord) {
       if (readFromRecord) {
        copyFieldToOut(".Total_volume", record);
        return;
    }
    ADD_FIELD_DIAG("Total_volume", i_get(".Nokia-Siemens-Information.Total-Volume"));
}

int map_common(flag recordType, RecordValue *record, int readFromRecord, int isSCSCF) {
    int returnCode = RETURN_SUCCESS;

    map_recordType(record, readFromRecord, isSCSCF); //0,1,2,3,4,5,6,7,11
    map_retransmission(recordType, record, readFromRecord); //0,1,2,3,4,5,6,7,11
    map_sIPMethod(recordType, record, readFromRecord); //0,1,2,3,4,5,6,7,11
    map_nodeAddress(recordType, record, readFromRecord); //0,1,2,3,4,5,6,7,11
    map_sessionId(recordType, record, readFromRecord); //0,1,2,3,4,5,6,7,11
    map_listOfCallingPartyAddress(recordType, record, readFromRecord); //0,1,2,3,4,5,6,7,11
    map_calledPartyAddress(recordType, record, readFromRecord); //0,1,2,3,4,5,6,7,11
    map_serviceRequestTimeStamp(recordType, record, readFromRecord); //0,1,2,3,4,5,6,7,11
    map_iMSChargingIdentifier(recordType, record, readFromRecord); //0,1,2,3,4,5,6,7,11
    map_serviceReasonReturnCode(recordType, record, readFromRecord); //0,1,2,3,4,5,6,7,11
    map_serviceRequestTimeStampFraction(recordType, record, readFromRecord); //0,1,2,3,4,5,6,7,11
    map_accessNetworkInformation(recordType, record, readFromRecord); //0,1,2,3,4,5,6,7,11
    if (!readFromRecord) {
        map_event_timestamp(recordType);
        map_timestamps_block_exists(recordType);
    }

    return returnCode;
}

int map_aSRecord(flag recordType, const char* triggerType, int triggerTypeOneEnabled, RecordValue *record, int readFromRecord, int isSCSCF) {
    map_listOfMessageBodies(recordType, record, readFromRecord); //0,1,6,7,11
    map_dialledPartyAddress(recordType, record, readFromRecord);
    //map_callProperty(recordType, record, readFromRecord);
    //map_callingUserType(recordType, record, readFromRecord);
    int returnCode = RETURN_SUCCESS;
    returnCode = returnCode || map_common(recordType, record, readFromRecord, isSCSCF);
    map_privateUserID(recordType, record, readFromRecord);
    map_interOperatorIdentifiers(recordType, record, readFromRecord); //0,1,2,3,4,5,6,7,11
    map_roleOfNode(recordType, record, readFromRecord); //0,1,2,4,5,6,7,11
    map_serviceDeliveryStartTimeStamp(recordType, record, readFromRecord); //0,1,3,4,5,6,7,11
    map_serviceDeliveryEndTimeStamp(recordType, record, readFromRecord); //0,1,3,4,5,6,7,11
    map_serviceDeliveryStartTimeStampFraction(recordType, record, readFromRecord);
    map_serviceDeliveryEndTimeStampFraction(recordType, record, readFromRecord);
    map_recordOpeningTime(recordType, record, readFromRecord); //0,1,3,4,5,6,7,11
    map_recordClosureTime(recordType, record, readFromRecord);
    map_recordOpeningTimeFraction(recordType, record, readFromRecord); //0,1,3,4,5,6,7,11
    map_recordClosureTimeFraction(recordType, record, readFromRecord); 
    map_listOfSDPMediaComponents(recordType, triggerType, triggerTypeOneEnabled, record, readFromRecord); // needs to be mapped AFTER map_roleOfNode
    map_onlinechargingflag(recordType, record, readFromRecord);
    map_iMSVisitedNetworkIdentifier(recordType, record, readFromRecord);
    map_subscriberEquipmentNumber(recordType, record, readFromRecord);
    map_requestedPartyAddress(recordType, record, readFromRecord);
    map_serviceIdentity(recordType, record, readFromRecord);
    map_chargedParty(recordType, record, readFromRecord);
    map_callingParty(recordType, record, readFromRecord);
    map_connectedNumber(recordType, record, readFromRecord);
    map_trigger_type(recordType, triggerType, record, readFromRecord); // Only for mapper node use, don't write this to output CDR!	
    map_causeCode(recordType, record, readFromRecord); // Only for mapper node use, don't write this to output CDR!
    map_sDPMediaIdentifier(recordType, record, readFromRecord);
    map_aCName(recordType, record, readFromRecord);
    map_valueaddedServiceIdentity(recordType, record, readFromRecord);
    map_cSLocationInformation(recordType, record, readFromRecord);
    map_serviceInteractInfo(recordType, record, readFromRecord);
    map_sNMDServiceType(recordType, record, readFromRecord);
    map_tadsIndication(recordType, record, readFromRecord);
    map_userChargingCategory(recordType, record, readFromRecord);
    map_subscriptionID(recordType, record, readFromRecord);
    map_calledAssertedID(recordType, record, readFromRecord);
    map_callDescription(recordType, record, readFromRecord);
    map_ctxGroupNumber(recordType, record, readFromRecord);
    map_shortNumber(recordType, record, readFromRecord);
    map_applicationServersInformation(recordType, record, readFromRecord);

    return returnCode;
}

int map_sCSCFRecord(flag recordType, const char* triggerType, int triggerTypeOneEnabled, RecordValue *record, int readFromRecord, int isSCSCF) {

    map_listOfMessageBodies(recordType, record, readFromRecord); //0,1,6,7,11
    map_dialledPartyAddress(recordType, record, readFromRecord);
    int returnCode = RETURN_SUCCESS;
    returnCode = returnCode || map_common(recordType, record, readFromRecord, isSCSCF);
    map_privateUserID(recordType, record, readFromRecord);
    map_interOperatorIdentifiers(recordType, record, readFromRecord); //0,1,2,3,4,5,6,7,11
    map_roleOfNode(recordType, record, readFromRecord); //0,1,2,4,5,6,7,11
    map_serviceDeliveryStartTimeStamp(recordType, record, readFromRecord); //0,1,3,4,5,6,7,11
    map_serviceDeliveryEndTimeStamp(recordType, record, readFromRecord); //0,1,3,4,5,6,7,11
    map_serviceDeliveryStartTimeStampFraction(recordType, record, readFromRecord);
    map_serviceDeliveryEndTimeStampFraction(recordType, record, readFromRecord);
    map_recordOpeningTime(recordType, record, readFromRecord); //0,1,3,4,5,6,7,11
    map_recordClosureTime(recordType, record, readFromRecord);
    map_recordOpeningTimeFraction(recordType, record, readFromRecord); //0,1,3,4,5,6,7,11
    map_recordClosureTimeFraction(recordType, record, readFromRecord);
    map_listOfSDPMediaComponents(recordType, triggerType, triggerTypeOneEnabled, record, readFromRecord); // needs to be mapped AFTER map_roleOfNode
    map_subscriptionID(recordType, record, readFromRecord);
    map_requestedPartyAddress(recordType, record, readFromRecord);
    // application-Server-State
    // list-of-Application-Servers-Abnormal
    map_applicationServersInformation(recordType, record, readFromRecord);
	 map_total_volume(recordType,record, readFromRecord);
     
    return returnCode;
}

/***** helper functions for conversion rules *****/

int localtime_to_compactLocal(char* output) {
    char epochString[20] = {'\0'};
    sprintf(epochString, "%ld", time(NULL));
    DIAG(DIAG_LOW, "localtime_to_compactLocal(): epochString before epochToCompactLocal %s", epochString);
    return epoch_to_compactLocal(output, epochString, 0);

}

int epoch_to_compactLocal(char* output, const char* input, int is1900) {
    DIAG(DIAG_LOW, "TIME_CONVERT input is %s", input);
    if (*input == '\0') return 0; // no dot convert for empty
    time_t epoch;
    if (is1900) {
        epoch = atol(input) - TIME_DIFF_1900_1970;
    } else {
        epoch = atol(input);
    }
    struct tm* timeinfo = localtime(&epoch);

    int hdiff = timeinfo->tm_gmtoff / 3600;
    int mdiff = (timeinfo->tm_gmtoff % 3600) / 60;
    char sign = (timeinfo->tm_gmtoff > 0) ? '+' : '-';

    sprintf(output, "%02d%02d%02d%02d%02d%02d%02x%02d%02d",
            timeinfo->tm_year % 100, // years since 1900
            timeinfo->tm_mon + 1,
            timeinfo->tm_mday,
            timeinfo->tm_hour,
            timeinfo->tm_min,
            timeinfo->tm_sec,
            (int) sign,
            abs(hdiff),
            abs(mdiff)
            );

    DIAG(DIAG_LOW, "TIME_CONVERT output is %s", output);
    return 0;
}

int compactLocalToComparableDateString(const char* input, char* output) {
    DIAG(DIAG_HIGH, "compactLocalToComparableDateString(): input: '%s'", input);
    if (*input == '\0') return 0;

    substring(output, input, 0, 12);

    return 0;
}

/*
 * This function takes in dates in format yyMMddHHmmss format and return the time difference
 * in seconds
 */
double getTimeDiff(const char* endDate, const char* startDate) {
    time_t startTime = stringToTime(startDate);
    time_t endTime = stringToTime(endDate);
    DIAG(DIAG_HIGH, "getTimeDiff(): Trying to get difference between strings: '%s' and '%s'", endDate, startDate);


    if (startTime == (time_t) - 1 || endTime == (time_t) - 1) {
        DIAG(DIAG_HIGH, "getTimeDiff(): failed to convert dates to time");
        return 1;
    }

    double diff = difftime(endTime, startTime);
    DIAG(DIAG_HIGH, "getTimeDiff(): Difference was: '%f'", diff);

    return diff;
}

/*
 * This function converts a date in format yyMMddHHmmss to a time_t
 * The return date is only used for getting second difference between two dates.
 */
time_t stringToTime(const char* input) {
    int year = 0;
    int month = 0;
    int day = 0;
    int hour = 0;
    int min = 0;
    int sec = 0;
    time_t returnTime = 0;

    if (sscanf(input, "%2d%2d%2d%2d%2d%2d", &year, &month, &day, &hour, &min, &sec) == 6) {
        struct tm timeInfo = {0};
        timeInfo.tm_year = year + 100; /* years since 1900 */
        timeInfo.tm_mon = month - 1;
        timeInfo.tm_mday = day;
        timeInfo.tm_hour = hour;
        timeInfo.tm_min = min;
        timeInfo.tm_sec = sec;

        if ((returnTime = mktime(&timeInfo)) == (time_t) - 1) {
            DIAG(DIAG_HIGH, "stringToTime(): cound not convert '%s' to time_t", input);
            return returnTime;
        }

        return returnTime;
    } else {
        DIAG(DIAG_HIGH, "stringToTime(): date '%s' was in wrong format", input);
        return (time_t) - 1;
    }
}

void substring(char* output, const char *input, int offset, int length) {
    if (*input == '\0') return;
    int i;

    if (offset >= 0 && offset < strlen(input)) {
        for (i = 0; i < length && input[offset + i] != '\0'; i++) {
            output[i] = input[offset + i];
        }
        output[i] = '\0';
    } else {
        strcpy(output, "");
    }
}

void str_to_low(char* output, const char *input) {
    if (*input == '\0') return;
    int i;

    for (i = 0; i < strlen(input); i++) {
        output[i] = tolower(input[i]);
    }
}

int stringToByteArray(char* output, const char* input) {
    if (*input == '\0') return 0;
    char tmp[3] = {'\0'};


    while (*input != '\0') {
        sprintf(tmp, "%02X", (int) *input++);
        strcat(output, tmp);
        memset(tmp, '\0', sizeof (tmp));
    }

    return 0;
}

int byteArrayToString(char* output, const char* input, int size) {
    if (*input == '\0') return 0;

    int i;
    char tmp[2] = {'\0'};

    for (i = 0; i < size; i += 2) {
        int c = hexToInt(input[i]) * 16 + hexToInt(input[i + 1]);
        sprintf(tmp, "%c", (char) c);
        strcat(output, tmp);
    }

    return 0;
}

int hexToInt(char hex) {
    switch (hex) {
        case 'F':
        case 'f':
            return 15;
        case 'E':
        case 'e':
            return 14;
        case 'D':
        case 'd':
            return 13;
        case 'C':
        case 'c':
            return 12;
        case 'B':
        case 'b':
            return 11;
        case 'A':
        case 'a':
            return 10;
        case '9':
        case '8':
        case '7':
        case '6':
        case '5':
        case '4':
        case '3':
        case '2':
        case '1':
        case '0':
            return (int) (hex - '0');
    }

    return 0;
}

void replaceUserLocationInfoFirstByte(char* output, const char* input) {
    if (*input == '\0') return;

    strcpy(output, input);

    if (strlen(input) > 2) {
        if (output[0] == '0' && output[1] == '0') {
            output[1] = '1'; // 00 -> 01
        } else if (output[0] == '0' && output[1] == '1') {
            output[1] = '2'; // 01 -> 02
        } else if (output[0] == '0' && output[1] == '2') {
            output[1] = '4'; // 02 -> 04
        } else if (output[0] == '8' && output[1] == '0') {
            output[0] = '0';
            output[1] = '8'; // 80 -> 08
        } else if (output[0] == '8' && output[1] == '1') {
            output[0] = '1';
            output[1] = '0'; // 81 -> 10
        } else if (output[0] == '8' && output[1] == '2') {
            output[0] = '1';
            output[1] = '8'; // 82 -> 18
        }
    }
}

void add_ba_field_diag(const char* output, const char* input) {
    if (*input == '\0') return;

    char value[(strlen(input) * 2 + 1)];
    memset(value, '\0', sizeof (value));

    stringToByteArray(value, input);
    ADD_BA_FIELD_DIAG(output, value, input);

}
void add_integer_field_diag(const char* output, const char* input) {
    if (*input != '\0') { 
      o_set(output, input); 
      DIAG(DIAG_HIGH, "ADD_INTEGER_FIELD_DIAG(): F %s %s", output, input); 
      o_begin(); 
    }
}

/**
 * This function is used to read a field value from a record which has been stored
 * in TimesTen. It's used instead of reading from input in case we are handling out of
 * order records.
 * 
 * @param fieldName The name for the field (e.g. ".serviceDeliveryStartTimeStamp")
 * @param record char array containing the TRS record (can be empty when reading from input)
 * 
 * @return the resulting field value (empty string in case field was not found)
 */
char* getFieldValueFromTRSRecord(const char* fieldName, RecordValue *record) {
    return getFieldValueFromTRSRecordByIndex(fieldName, record, 0);
}

char* getFieldValueFromTRSRecordByIndex(const char* fieldName, RecordValue *record, int index) {
    DIAG(DIAG_HIGH, "getFieldValueFromTRSRecordByIndex: %s, %d", fieldName, index);
    char* dest = NULL;
    int i;
    int foundIndex = 0;
    for (i = 0; i < record->length; i++) {
        char* value = strchr(record->values[i], ' ');
        if (value != NULL) {
            *value = '\0';
            if (strcmp(fieldName, getBlockByIndex(record->values[i]).longName) == 0) {
                if (index == foundIndex) {
                    DIAG(DIAG_HIGH, "getFieldValueFromTRSRecordByIndex: found '%s=%s'", fieldName, value + 1);
                    dest = (char*) split_string_malloc(sizeof (char) * (strlen(value + 1) + 1));
                    strcpy(dest, value + 1);
                    *value = ' ';
                    return dest;
                } else {
                    DIAG(DIAG_HIGH, "getFieldValueFromTRSRecordByIndex: found but wrong index");
                    foundIndex++;
                }
            }
            *value = ' ';
        }
    }
    dest = (char*) split_string_malloc(sizeof (char));
    *dest = '\0';
    return dest;
}

/**
 * This function is used to read a field value from a record which has been stored
 * in TimesTen. It's used instead of reading from input in case we are handling out of
 * order records.
 * 
 * @param fieldName The name for the field (e.g. ".serviceDeliveryStartTimeStamp")
 * @param record char array containing the TRS record (can be empty when reading from input)
 * 
 * @return 0 if field didn't exist, 1 if it exists
 */
int fieldExistsInTRSRecord(const char* fieldName, RecordValue *record) {
    DIAG(DIAG_HIGH, "fieldExistsInTRSRecord: %s", fieldName);
    int i;
    for (i = 0; i < record->length; i++) {
        char* value = strchr(record->values[i], ' ');
        if (value != NULL) {
            // field
            *value = '\0';
            if (strcmp(fieldName, getBlockByIndex(record->values[i]).longName) == 0) {
                DIAG(DIAG_HIGH, "fieldExistsInTRSRecord: found '%s=%s'", fieldName, value + 1);
                // update record row back in case it's read again
                *value = ' ';
                return 1;
            }
            *value = ' ';
        }
    }
    return 0;
}

int blockExistsInTRSRecordByIndex(const char* blockName, RecordValue *record, int index) {
    DIAG(DIAG_HIGH, "blockExistsInTRSRecordByIndex: %s", blockName);
    int i;
    int foundIndex = 0;
    for (i = 0; i < record->length; i++) {
        char* value = strchr(record->values[i], ' ');
        if (value == NULL) {
            // block
            size_t length = strlen(blockName);
            if (strncmp(blockName, getBlockByIndex(record->values[i]).longName, length) == 0) {
                if (index == foundIndex) {
                    DIAG(DIAG_HIGH, "blockExistsInTRSRecordByIndex: found '%s'", blockName);
                    return 1;
                } else {
                    DIAG(DIAG_HIGH, "blockExistsInTRSRecordByIndex: found but wrong index");
                }
                foundIndex++;
            }
        }
    }
    return 0;
}

int blockExistsInTRSRecord(const char* blockName, RecordValue *record) {
    return blockExistsInTRSRecordByIndex(blockName, record, 0);
}

/**
 * Copy single field from record and set it to out.
 * @param field_name name of output field
 * @param record char array containing the TRS record
 */
void copyFieldToOut(const char* field_name, RecordValue *record) {
    DIAG(DIAG_HIGH, "copyFieldToOut(): %s", field_name);
    char* value = getFieldValueFromTRSRecord(field_name, record);
    ADD_FIELD_DIAG(field_name, value);
    free(value);
}

void clearAndCopyBlockToOut(const char* block_name, RecordValue *record) {
    if (blockExistsInTRSRecord(block_name, record)) {
        DEL_BLOCK_DIAG(block_name);
    }
    updateBlockToOut(block_name, record, NULL, NULL);
}

void copyBlockToOut(const char* block_name, RecordValue *record) {
    updateBlockToOut(block_name, record, NULL, NULL);
}

/**
 * Copy block from record and set it to out.
 * @param block_name name of output block
 * @param record char array containing the TRS record
 */
void updateBlockToOut(const char* block_name, RecordValue *record, const char** repeat_block_name, const char* add_field_name) {
    DIAG(DIAG_HIGH, "copyBlockToOut(): %s", block_name);
    int i;
    int tmp_name_components_num;
    int tmp_name_components_num_previous = 0;
    if (!blockExistsInTRSRecord(block_name, record)) {
        DIAG(DIAG_HIGH, "copyBlockToOut(): '%s' not found", block_name);
        return;
    }
    o_address(".");
    for (i = 0; i < record->length; i++) {
        char* tmp_value_separator = strchr(record->values[i], ' ');
        int is_block = tmp_value_separator == NULL;
        if (!is_block) {
            // separate name and value
            *tmp_value_separator = '\0';
        }
        DBBlock dbBlock = getBlockByIndex(record->values[i]);
        // put space back between field name and value
        if (!is_block) {
            *tmp_value_separator = ' ';
        }
        if (strstr(dbBlock.longName, block_name) != NULL) {
            // if this field needs to be added in an upper block level than the
            // previous field, move out of blocks as many times as needed
            char* current;
            tmp_name_components_num = dbBlock.level;
            while (tmp_name_components_num_previous-- > tmp_name_components_num) {
                DIAG(DIAG_HIGH, "copyBlockToOut(): exit one level");
                o_exit();
            }
            tmp_name_components_num_previous = tmp_name_components_num;
            if (is_block) {
                current = dbBlock.shortName;
                // add the block to the output record
                if (repeat_block_name != NULL) {
                    int k = -1, found = 0;
                    while (repeat_block_name[++k] != NULL) {
                        if (strcmp(current, repeat_block_name[k]) == 0) {
                            found = 1;
                            break;
                        }
                    }
                    if (found) {
                        DIAG(DIAG_HIGH, "copyBlockToOut(): adding %s", current);
                        ADD_BLOCK_DIAG(current);
                    } else {
                        DIAG(DIAG_HIGH, "copyBlockToOut(): entering %s", current);
                        ENTER_BLOCK_DIAG(current);
                    }
                } else {
                    DIAG(DIAG_HIGH, "copyBlockToOut(): entering %s", current);
                    ENTER_BLOCK_DIAG(current);
                }
            } else {
                current = dbBlock.shortName;
                // add or update the field to the output record
                if (add_field_name != NULL && strcmp(current, add_field_name) == 0) {
                    DIAG(DIAG_HIGH, "copyBlockToOut(): adding '%s=%s'", current, tmp_value_separator + 1);
                    o_add_field(current, tmp_value_separator + 1);
                } else {
                    DIAG(DIAG_HIGH, "copyBlockToOut(): setting '%s=%s'", current, tmp_value_separator + 1);
                    o_set(current, tmp_value_separator + 1);
                }

            }
        }
    }
    o_address(".");
}

/**
 * This function is used to read a Field value from output. It reads the value into field.fieldValue.
 * It can handle fields with or without blocks and will always set the output address
 * to the root of the hierarchy when it's done.
 * 
 * @param field
 */
void read_field_value_from_output(Field *field) {
    enter_field_block(field);

    field->fieldValue = o_get(field->fieldName);
    // Call this to make sure we're in the root of the hierarchy after exiting this function
    END_BLOCK_DIAG();
}

/**
 * This function is used to write a Field struct into output.
 * It can handle fields with or without blocks and will always set the output address
 * to the root of the hierarchy when it's done.
 * 
 * @param field
 */
void write_field_to_output(Field *field) {
    enter_field_block(field);

    ADD_NEW_FIELD_DIAG(field->fieldName, field->fieldValue);
    // Call this to make sure we're in the root of the hierarchy after exiting this function
    END_BLOCK_DIAG();
}

/**
 * This function is used to enter the block in which this Field resides (as indicated by the blocks array).
 * 
 * @param field
 */
void enter_field_block(Field *field) {
    // Call this to make sure we're in the root of the hierarchy
    END_BLOCK_DIAG();

    int i = 0;
    while (strcmp(field->blocks[i].blockName, "") != 0) {
        DIAG(DIAG_HIGH, "enter_field_block(): Found block '%s[%d]' in field '%s'", field->blocks[i].blockName, field->blocks[i].blockIndex, field->fieldName);
        addOrUpdateBlock(field->blocks[i].blockName, field->blocks[i].blockIndex);
        i++;
    }
}

void enter_existing_output_block(Block blocks[]) {
    o_address(".");
    int i = 0;
    while (strcmp(blocks[i].blockName, "") != 0 && o_field_exists(blocks[i].blockName)) {
        char block[40] = "";
        if (blocks[i].blockIndex > 0) {
            sprintf(block, "%s[%d]", blocks[i].blockName, blocks[i].blockIndex);
        } else {
            sprintf(block, "%s", blocks[i].blockName);
        }
        DIAG(DIAG_HIGH, "enter_existing_output_block(): Found block '%s'", block);
        o_address(block);
        o_enter();
        i++;
    }
}

void postPartialCDRHandling() {
    DIAG(DIAG_HIGH, "postPartialCDRHandling(): updating record-Opening-Time to value from old record-Closure-Time");
    ADD_FIELD_DIAG("record-Opening-Time", o_get("record-Closure-Time"));

    if (strcmp(i_get(".Trigger-Type"), "1") == 0) {
        DIAG(DIAG_HIGH, "postPartialCDRHandling(): updating sDP-Media-Identifier to value from old sDP-Media-Identifier");
        ADD_FIELD_DIAG("sDP-Media-Identifier", o_get("sDP-Media-Identifier_LATEST"));
    }

    DIAG(DIAG_HIGH, "postPartialCDRHandling(): removing output fields which are only needed for CDRs");
    DEL_FIELD_DIAG("record-Closure-Time");
    DEL_FIELD_DIAG("cause-For-Record-Closing");
    DEL_FIELD_DIAG("duration");
}

/**
 * Sets ip address to output parameter and returns type of address.
 * Returns:
 * 	-1 if address is in illegal format
 *	 0 if IPv4
 * 	 1 if IPv6
 *
 */
int get_ip_address(const char* address, char* output) {
    char iPAddress[strlen(address) + 1];
    memset(iPAddress, '\0', strlen(address));
    strcpy(iPAddress, address);
    DIAG(DIAG_HIGH, "get_ip_address(): ip address=%s", iPAddress);
    if (strncmp(iPAddress, "/", 1) == 0) {
        int dots = 0;
        int colons = 0;
        char delimiter[2];
        int i;
        int size = strlen(iPAddress) - 1;
        memmove(iPAddress, iPAddress + 1, strlen(iPAddress));
        for (i = 0; i < size; i++) {
            if (iPAddress[i] == '.')
                dots++;
            if (iPAddress[i] == ':')
                colons++;
        }
        if (dots == 3 || colons == 7) {
            if (dots == 3)
                sprintf(delimiter, "."); //IPv4
            else
                sprintf(delimiter, ":"); //IPv6
            char *p = strtok(iPAddress, delimiter);
            while (p != NULL) {
                char *ptr;
                char tmp[3] = {'\0'};
                if (strcmp(delimiter, ".") == 0) {
                    sprintf(tmp, "%02X", (int) strtoul(p, &ptr, 10));
                } else {
                    sprintf(tmp, "%04X", (int) strtoul(p, &ptr, 16));
                }
                strcat(output, tmp);
                memset(tmp, '\0', sizeof (tmp));
                p = strtok(NULL, delimiter);
            }
            if (strcmp(delimiter, ".") == 0) {
                DIAG(DIAG_HIGH, "get_ip_address(): IPv4 address=%s", output);
                return 0;
            } else {
                DIAG(DIAG_HIGH, "get_ip_address(): IPv6 address=%s", output);
                return 1;
            }
        } else {
            return -1;
        }
    } else {
        return -1;
    }
}

void removeDuplicatesFromOut(const char* rootBlockName, const char* repeatingBlockName) {
    DIAG(DIAG_HIGH, "removeDuplicatesFromOut(): %s, %s", rootBlockName, repeatingBlockName);
    if (o_address(rootBlockName)) {
        o_enter();
    } else {
        // Block didn't exist at all
        DIAG(DIAG_HIGH, "removeDuplicatesFromOut(): %s didn't exist", rootBlockName);
        return;
    }
    int i = 0, j = -1;
    while (o_next(repeatingBlockName)) {
        i++;
    }
    if (i == 0) {
        // Repeating block didn't exist at all
        DIAG(DIAG_HIGH, "removeDuplicatesFromOut(): %s didn't exist", repeatingBlockName);
        return;
    }
    DIAG(DIAG_HIGH, "removeDuplicatesFromOut(): blockCount = %d", i);
    char a_result_string[i][40000];
    int duplicateIndex[i];
    for (j = 0; j < i; j++) {
        memset(a_result_string[j], '\0', strlen(a_result_string[j]));
        duplicateIndex[j] = -1;
    }
    j = -1;
    int depth = 0;
    o_address(rootBlockName);
    o_enter();
    while (depth > -1) {
        if (o_next("") == 1) {
            // add a block
            if (o_is_block("") == 1) {
                o_enter();
                if (depth == 0) {
                    j++;
                }
                depth++;
                strcat(a_result_string[j], o_get_address());
                strcat(a_result_string[j], "\n");
            }// add a field
            else if (j > -1) {
                strcat(a_result_string[j], o_get_address());
                strcat(a_result_string[j], " ");
                strcat(a_result_string[j], o_get(""));
                strcat(a_result_string[j], "\n");
            }
        } else if (depth > -1) {
            o_exit();
            depth--;
        }
    }
    o_address(".");
    int k = 0, l = 0, m = 0;
    for (j = 0; j < i; j++) {
        for (k = j + 1; k < i; k++) {
            if (strcmp(a_result_string[j], a_result_string[k]) == 0) {
                int found = 0;
                for (m = 0; m < i; m++) {
                    if (duplicateIndex[m] == k) {
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    duplicateIndex[l++] = k;
                }
            }
        }
    }
    // we need to sort indexes
    int temp;
    for (j = 0; j < i - 1; j++) {
        for (k = 0; k < i - 1 - j; k++) {
            if (duplicateIndex[k] > duplicateIndex[k + 1]) {
                temp = duplicateIndex[k];
                duplicateIndex[k] = duplicateIndex[k + 1];
                duplicateIndex[k + 1] = temp;
            }
        }
    }
    for (j = i - 1; j >= 0; j--) {
        if (duplicateIndex[j] != -1) {
            char blockName[200] = {'\0'};
            sprintf(blockName, "%s.%s[%d]", rootBlockName, repeatingBlockName, duplicateIndex[j]);
            DIAG(DIAG_HIGH, "removeDuplicatesFromOut(): remove block %s", blockName);
            o_delete(blockName);
        }
    }
}

void addFieldRemoveDuplicate(const char* rootBlockName, const char* name, const char* value) {
    DIAG(DIAG_HIGH, "addFieldRemoveDulicate(): %s, %s, %s", rootBlockName, name, value);
    if (*value == '\0') {
        return;
    }
    ENTER_BLOCK_DIAG(rootBlockName);

    int exists = 0;

    while (o_next(name)) {
        const char* oldValue = o_get("");
        DIAG(DIAG_HIGH, "addFieldRemoveDulicate(): old value %s", oldValue);
        if (strcmp(value, oldValue) == 0) {
            exists = 1;
            break;
        }
    }
    if (exists == 0) {
        o_add_field(name, value);
    }
    END_BLOCK_DIAG();
}

void copyRecordFieldsRemoveDuplicate(const char* recordFullName, const char* block, const char* name, RecordValue *record) {
    int i;
    for (i = 0; i < record->length; i++) {
        char* tmp_value_separator = strchr(record->values[i], ' ');
        // ignore blocks
        if (tmp_value_separator == NULL) {
            continue;
        }
        // separate name and value
        *tmp_value_separator = '\0';
        DBBlock dbBlock = getBlockByIndex(record->values[i]);
        // put space back between field name and value
        *tmp_value_separator = ' ';
        if (strcmp(dbBlock.longName, recordFullName) == 0) {
            addFieldRemoveDuplicate(block, name, tmp_value_separator + 1);
        }
    }
}

void clearOutBlock(const char* blockName) {
    if (o_address(blockName)) {

        o_enter();
        while (o_next("")) {
            const char* nextName = o_get_field_name();
            o_delete(nextName);
        }
        o_address(".");
    }
}

/**
 * Return hierarchy level of block or field.
 *
 * Field example: .list-Of-Called-Asserted-Identity.sIP-URI
 * Block example: .list-Of-Calling-Party-Address.
 */
int get_level(const char *aString) {
    size_t length = strlen(aString);
    int i;
    int tokensNumber = 1;
    for (i = 0; aString[i] != '\0'; i++) {
        if (aString[i] == '.' && i != (length - 1)) {
            tokensNumber++;
        }
    }
    return tokensNumber;
}

/**
 * Get field or block name from line.
 *
 * Field example: .list-Of-Called-Asserted-Identity.sIP-URI
 * Block example: .list-Of-Calling-Party-Address.
 */
char* get_field_name(char* line) {
    size_t length = strlen(line);
    int is_block = line[length - 1] == '.';
    int i, block_length = 0;
    // get last block length
    // ignore last . in case of block
    int end = is_block ? length - 2 : length - 1;
    for (i = end; i >= 0; i--) {
        if (line[i] == '.') {
            break;
        } else {
            block_length++;
        }
    }
    char* block_name = split_string_malloc(sizeof (char) * (block_length + 1));
    char* start = line + length - block_length;
    if (is_block) {
        start = start - 1;
    }
    strncpy(block_name, start, block_length);
    block_name[block_length] = '\0';
    return block_name;
}


