#ifndef APP_TRS_AGGR_H
#define APP_TRS_AGGR_H
#define TAS_OUT "AGGREGATED"

#define CLEAR_CHAR_FIELD(name) { name = ""; }

//#define ADD_FIELD_DIAG(name, value) if (*value != '\0') { o_set(name, value); DIAG(DIAG_HIGH, "ADD_FIELD_DIAG(): F %s %s", name, value); o_begin(); }
//#define DEL_FIELD_DIAG(name) if (*name != '\0') { o_delete(name); DIAG(DIAG_HIGH, "DEL_FIELD_DIAG(): delete %s", name); }
//#define ADD_BLOCK_DIAG(name) if (*name != '\0') { o_add_block_enter(name); DIAG(DIAG_HIGH, "ADD_BLOCK_DIAG(): B %s", name); }
//#define ENTER_BLOCK_DIAG(name) if (*name != '\0') { o_address(name); o_enter(); else o_add_block_enter(name); DIAG(DIAG_HIGH, "ENTER_BLOCK_DIAG(): B %s", name); }
//#define END_BLOCK_DIAG() { o_exit(); DIAG(DIAG_HIGH, "END_BLOCK_DIAG(): ."); }
//#define ENTER_BLOCK_DIAG(name) if (*name != '\0') { if (o_address(name)) o_enter(); else o_add_block_enter(name); DIAG(DIAG_HIGH, "ENTER_BLOCK_DIAG(): B %s", name); }
//#define END_BLOCK_DIAG() { o_address("."); DIAG(DIAG_HIGH, "END_BLOCK_DIAG(): ."); }

//#define TIME_DIFF_1900_1970 2208988800
//#define TZ_OFFSET_DEFAULT "+0200"
//typedef enum { EVENT, START, INTERIM, STOP } flag;
#define DELIM_FIELD_S                 "\n"
#define DELIM_FIELD_C                 '\n'
#define DELIM_FIELD_VALUE_S           " "
#define DELIM_FIELD_VALUE_C           ' '

#define DELIM_FIELD_S_OLD             "|"
#define DELIM_FIELD_C_OLD             '|'
#define DELIM_FIELD_VALUE_S_OLD       "~"
#define DELIM_FIELD_VALUE_C_OLD       '~'

/* Diagnostic level settings */
#define DIAG_LOW 10
#define DIAG_MEDIUM 50
#define DIAG_HIGH 90

#define IBCFRECORD "iBCFRecord"
#define ASRECORD "ASRecord"
#define OUTPUT "OUTPUT"
#define CSCFOUTPUT "CSCFOUTPUT"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "nodebase.h"
#include "trs_library.h"
#include "field_mapping.h"

/* constants */
#define MAX_RECORD_SIZE 100000

#define SQL_CREATE_TABLE_CDRS  "CREATE TABLE RECORD_TABLE(SESSION_ID VARCHAR2(250) NOT NULL, RECORD VARCHAR2(100000), REC_NUM INT, BUF_ACR_NUM INT, CDR_TYPE VARCHAR2(2) NOT NULL, UDR_TYPE VARCHAR2(2) NOT NULL, TIMEOUT_AT BIGINT, START_LOST INT)"
#define SQL_CREATE_INDEX_CDRS  "CREATE UNIQUE INDEX RECORD_TABLE_UNQIDX ON RECORD_TABLE(SESSION_ID, CDR_TYPE, REC_NUM)"
#define SQL_CREATE_INDEX_TIMEOUT  "CREATE INDEX RECORD_TABLE_TIMEOUTIDX ON RECORD_TABLE(TIMEOUT_AT)"

#define SQL_SELECT_UTILIZATION "SELECT PERM_ALLOCATED_SIZE, PERM_IN_USE_SIZE FROM SYS.MONITOR"
#define SQL_INSERT_CDR         "INSERT INTO RECORD_TABLE (SESSION_ID, RECORD, REC_NUM, BUF_ACR_NUM, CDR_TYPE, UDR_TYPE, TIMEOUT_AT, START_LOST) VALUES (?, ?, ?, ?, ?, ?, ?, ?)"
#define SQL_SELECT_REC_COUNT   "SELECT COUNT(REC_NUM) FROM RECORD_TABLE WHERE SESSION_ID = ? AND CDR_TYPE = ?"
#define SQL_SELECT_CDR         "SELECT RECORD, REC_NUM, UDR_TYPE, BUF_ACR_NUM, START_LOST FROM RECORD_TABLE WHERE SESSION_ID = ? AND CDR_TYPE = ? ORDER BY REC_NUM"
#define SQL_UPDATE_CDR         "UPDATE RECORD_TABLE SET RECORD = ?, REC_NUM = ?, BUF_ACR_NUM = ?, UDR_TYPE = ? WHERE SESSION_ID = ? AND CDR_TYPE = ? AND REC_NUM = ?"
#define SQL_UPDATE_CDR_TIMEOUT "UPDATE RECORD_TABLE SET TIMEOUT_AT = ? WHERE SESSION_ID = ? AND CDR_TYPE = ?"
#define SQL_DELETE_CDR         "DELETE FROM RECORD_TABLE WHERE SESSION_ID = ? AND CDR_TYPE = ?"
#define SQL_DELETE_CDR_REC     "DELETE FROM RECORD_TABLE WHERE SESSION_ID = ? AND CDR_TYPE = ? AND REC_NUM = ?"
#define SQL_SELECT_CDR_ALL     "SELECT DISTINCT SESSION_ID, CDR_TYPE FROM RECORD_TABLE"
#define SQL_SELECT_CDR_TIMEOUT "SELECT DISTINCT SESSION_ID, CDR_TYPE FROM RECORD_TABLE WHERE TIMEOUT_AT < ?"

/* structs */

struct TRS_rec {
    char start_lost[2];
    char record[MAX_RECORD_SIZE];
    char rec_num[5];
    char udr_type[2];
    char buffered_acr_num[5];
};

typedef struct TRS_rec Record;

/* functions */

void do_compress(char* a_result_string);
void dump_output_record_to_log(void);
void dump_input_record_to_log(void);
void do_decompress(const char *a_string);
int check_utilization(int limit);
int check_partial_CDR_triggers(const char* bufferedACRNum, const char* triggerType, partialReason *pTrigger);
void load_session_id(char** udr_session_id);
long getTimeoutAt();
void handleRecordSequenceNumber(int single, int last);

void insert_into_TRS(const char* tmp_session_id, const char* tmp_cdr_type, Record *record);
void update_TRS(const char* tmp_session_id, const char* tmp_cdr_type, const char* tmp_rec_num_to_update, Record *record);
void update_timeout_TRS(const char* tmp_session_id, const char* tmp_cdr_type);
void delete_from_TRS(const char* tmp_session_id, const char* tmp_cdr_type);
void delete_rec_from_TRS(const char* tmp_session_id, const char* tmp_cdr_type, const char* tmp_rec_num);
int select_records_from_trs(const char* tmp_session_key, const char* tmp_cdr_type, Record tmp_rec[]);
void fetch_time_out_data(int flush);
void do_fetch_time_out_data(int queryId);
void write_record_to_output(int tmp_start_lost, int interim_lost, int stop_lost, flag recordType, const char* cdr_type, partialReason reason, const char* triggerType, int isPartial);
int get_number_of_records_from_trs(const char* tmp_session_key, const char* tmp_cdr_type);
void log_TRS_rec(Record *record, const char* functionName);
int aggregateCDR(char* udr_session_id, const char* cdr_type, const char* triggerType, Record *record, int udr_type, char* rec_num_to_update
        , int *sendCDR, partialReason *pTrigger, int *isPartial, int readFromRecord);
int handleNewCDR(char* udr_session_id, const char* cdr_type, const char* triggerType, Record *record, int udr_type);
void updateNetworkInformation(const char* field_name, const char* old_field_name);
void sendCDRToOutput(int udr_type, const char* cdr_type, const char* triggerType, partialReason pTrigger, int isPartial);
void searchForOutOfOrderGaps(int startMissing, int newRecNum, int* nextInSequence, int* secondGapIndex, int number_of_records, Record trs_records[]);
void fetchAllRecordsFromTRS(char* udr_session_id, const char* cdr_type, Record trs_records[], int number_of_records,
        int* nextInSequence, int newRecNum, int* secondGapIndex, int* startMissing);
//mapping functions from ivan
int write_ASN1_record(flag recordType, const char* cdr_type, const char* triggerType, RecordValue *record, int readFromRecord);
void updateTRS(char* udr_session_id, const char* cdr_type, Record *record, int udr_type, char* rec_num_to_update);
void initializeRecordValue(RecordValue* value, char* record);
void addBlock(int numberCode, char* longName);
void freeBlockMemory();
#endif
