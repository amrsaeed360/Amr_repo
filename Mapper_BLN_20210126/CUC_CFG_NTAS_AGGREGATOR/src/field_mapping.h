#ifndef FIELD_MAPPING_H
#define FIELD_MAPPING_H
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <math.h>

#include "nodebase.h"

#define ADD_FIELD_DIAG(name, value) if (*value != '\0') { o_set(name, value); DIAG(DIAG_HIGH, "ADD_FIELD_DIAG(): F %s %s", name, value); o_begin(); }
#define ADD_NEW_FIELD_DIAG(name, value) if (*value != '\0') { o_add_field(name, value); DIAG(DIAG_HIGH, "ADD_NEW_FIELD_DIAG(): F %s %s", name, value); o_begin(); }
#define DEL_FIELD_DIAG(name) if (*name != '\0') { o_delete(name); DIAG(DIAG_HIGH, "DEL_FIELD_DIAG(): delete %s", name); }
#define DEL_BLOCK_DIAG(name) if (*name != '\0') { if (o_is_block(name)){ o_delete(name); DIAG(DIAG_HIGH, "DEL_BLOCK_DIAG(): delete %s", name); } else { DIAG(DIAG_HIGH, "DEL_BLOCK_DIAG(): block %s does not exist", name); } }
#define ADD_BLOCK_DIAG(name) if (*name != '\0') { o_add_block_enter(name); DIAG(DIAG_HIGH, "ADD_BLOCK_DIAG(): B %s", name); }
#define ENTER_BLOCK_DIAG(name) if (*name != '\0') { if (o_address(name)) o_enter(); else o_add_block_enter(name); DIAG(DIAG_HIGH, "ENTER_BLOCK_DIAG(): B %s", name); }
#define END_BLOCK_DIAG() { o_address("."); DIAG(DIAG_HIGH, "END_BLOCK_DIAG(): . and moving . "); }
#define EXIT_BLOCK_DIAG() { o_exit(); DIAG(DIAG_HIGH, "EXIT_BLOCK_DIAG(): ."); }
#define ADD_BA_FIELD_DIAG(output, value, input) if (*value != '\0') { o_set(output, value); DIAG(DIAG_HIGH, "ADD_BA_FIELD_DIAG(): F %s %s [%s]", output, value, input); o_begin(); }

#define TIME_DIFF_1900_1970 2208988800
//#define TZ_OFFSET_DEFAULT "+0200"

#define MAX_BLOCK_DEPTH 20

#define DIAG_LOW 10
#define DIAG_MEDIUM 50
#define DIAG_HIGH 90

#define MAX_INT 4294967295

#define REJECTED_STORAGE "REJ_INVALID"
#define RETURN_SUCCESS 0
#define RETURN_REJECTED -1
#define CLEAR_CHAR_ARRAY(name) { memset(name, '\0', sizeof(name)); }

/* Data structures */
typedef enum {
    EVENT, START, INTERIM, STOP
} flag;

typedef enum {
    FLUSH, EXPIRATION, TRIGGER_TYPE, RULE
} partialReason;

typedef enum {
    FIRST_NOT_NULL, LAST_NOT_NULL, START_EVENT, STOP_EVENT, STOP_ONLY
} mappingRule;

struct Array {
    char **values;
    int length;
};

typedef struct Array RecordValue;

struct Indexed_block {
    char blockName[80];
    int blockIndex;
};

struct DataBaseBlock {
    char* longName;
    char* shortName;
    int level;
};
typedef struct DataBaseBlock DBBlock;

typedef struct Indexed_block Block;

struct Field_struct {
    const char* fieldName;
    const char* fieldValue;
    Block blocks[MAX_BLOCK_DEPTH]; // use block with an empty block name as end marker for this array
};

typedef struct Field_struct Field;

static const mappingRule MAPPING_ROLE_OF_NODE = LAST_NOT_NULL;
static const mappingRule MAPPING_SERVICE_REQUEST_TIMESTAMP_FRACTION = START_EVENT;
static const mappingRule MAPPING_SERVICE_DELIVERY_START_TIMESTAMP_FRACTION = START_EVENT;
static const mappingRule MAPPING_SERVICE_DELIVERY_END_TIMESTAMP_FRACTION = STOP_ONLY;
static const mappingRule MAPPING_NODE_ADDRESS = LAST_NOT_NULL;
static const mappingRule MAPPING_SESSION_ID = LAST_NOT_NULL;
static const mappingRule MAPPING_CALLED_PARTY_ADDRESS = LAST_NOT_NULL;
static const mappingRule MAPPING_SERVICE_REQUEST_TIMESTAMP = START_EVENT;
static const mappingRule MAPPING_SERVICE_DELIVERY_START_TIMESTAMP = START_EVENT;
static const mappingRule MAPPING_SERVICE_DELIVERY_END_TIMESTAMP = STOP_ONLY;
static const mappingRule MAPPING_REQUESTED_PARTY_ADDRESS = LAST_NOT_NULL;
static const mappingRule MAPPING_IMS_VISITED_NETWORK_IDENTIFIER = LAST_NOT_NULL;
static const mappingRule MAPPING_ONLINE_CHARGING_FLAG = LAST_NOT_NULL;
static const mappingRule MAPPING_SERVICE_INTERACT_INFO = LAST_NOT_NULL;
static const mappingRule MAPPING_VALUE_ADDED_SERVICE = LAST_NOT_NULL;

int mappingRuleCheck(const char* inputField, mappingRule rule, flag recordType);
void mappingDirect(const char* inputField, const char* outputField, mappingRule rule, flag recordType, RecordValue *record, int readFromRecord);
void mappingDirectBlock(const char* inputField, const char* outputBlock, const char* outputField, mappingRule rule, flag recordType, RecordValue *record, int readFromRecord);
void mappingDirectBlockBA(const char* inputField, const char* outputBlock, const char* outputField, mappingRule rule, flag recordType, RecordValue *record, int readFromRecord);
void mappingTimestamp(const char* inputField, const char* outputField, mappingRule rule, flag recordType, RecordValue *record, int readFromRecord);

void map_recordType(RecordValue *record, int readFromRecord, int isSCSCF);
void map_retransmission(flag recordType, RecordValue *record, int readFromRecord);
void map_sIPMethod(flag recordType, RecordValue *record, int readFromRecord);
void map_roleOfNode(flag recordType, RecordValue *record, int readFromRecord);
void map_nodeAddress(flag recordType, RecordValue *record, int readFromRecord);
void map_sessionId(flag recordType, RecordValue *record, int readFromRecord);
void map_listOfCallingPartyAddress(flag recordType, RecordValue *record, int readFromRecord);
void map_calledPartyAddress(flag recordType, RecordValue *record, int readFromRecord);
void map_serviceRequestTimeStamp(flag recordType, RecordValue *record, int readFromRecord);
void map_serviceDeliveryStartTimeStamp(flag recordType, RecordValue *record, int readFromRecord);
void map_recordOpeningTime(flag recordType, RecordValue *record, int readFromRecord);
void map_serviceDeliveryEndTimeStamp(flag recordType, RecordValue *record, int readFromRecord);
void map_recordClosureTime(flag recordType, RecordValue *record, int readFromRecord);
void map_interOperatorIdentifiers(flag recordType, RecordValue *record, int readFromRecord);
int map_interOperatorIdentifiersByIndex(flag recordType, RecordValue *record, int readFromRecord, int i);
void map_localRecordSequenceNumber(flag recordType);
void map_recordSequenceNumber(flag recordType, int recordNumber);
void map_causeCode(flag recordType, RecordValue *record, int readFromRecord);
void map_causeForRecordClosing(flag recordType, partialReason reason, const char* triggerType);
void map_iMSChargingIdentifier(flag recordType, RecordValue *record, int readFromRecord);
void map_listOfSDPMediaComponents(flag recordType, const char* triggerType, int triggerTypeOneEnabled, RecordValue *record, int readFromRecord);
void map_serviceReasonReturnCode(flag recordType, RecordValue *record, int readFromRecord);
void map_listOfMessageBodies(flag recordType, RecordValue *record, int readFromRecord);
void map_dialledPartyAddress(flag recordType, RecordValue *record, int readFromRecord);
//void map_callProperty(flag recordType, RecordValue *record, int readFromRecord);
//void map_callingUserType(flag recordType, RecordValue *record, int readFromRecord);
void map_accessNetworkInformation(flag recordType, RecordValue *record, int readFromRecord);
void map_onlinechargingflag(flag recordType, RecordValue *record, int readFromRecord);
void map_iMSVisitedNetworkIdentifier(flag recordType, RecordValue *record, int readFromRecord);
void map_serviceRequestTimeStampFraction(flag recordType, RecordValue *record, int readFromRecord);
void map_serviceDeliveryStartTimeStampFraction(flag recordType, RecordValue *record, int readFromRecord);
void map_serviceDeliveryEndTimeStampFraction(flag recordType, RecordValue *record, int readFromRecord);
void map_subscriberEquipmentNumber(flag recordType, RecordValue *record, int readFromRecord);
void map_requestedPartyAddress(flag recordType, RecordValue *record, int readFromRecord);
void map_chargedParty(flag recordType, RecordValue *record, int readFromRecord);
void map_duration(int mapDurationMilli);
void map_trigger_type(flag recordType, const char* triggerType, RecordValue *record, int readFromRecord);
void map_callingParty(flag recordType, RecordValue *record, int readFromRecord);
void map_connectedNumber(flag recordType, RecordValue *record, int readFromRecord);
void map_sDPMediaIdentifier(flag recordType, RecordValue *record, int readFromRecord);
void map_serviceIdentity(flag recordType, RecordValue *record, int readFromRecord);
void map_serviceInteractInfo(flag recordType, RecordValue *record, int readFromRecord);
void map_sNMDServiceType(flag recordType, RecordValue *record, int readFromRecord);
void map_tadsIndication(flag recordType, RecordValue *record, int readFromRecord);
void map_cSLocationInformation(flag recordType, RecordValue *record, int readFromRecord);
void map_subscriptionID(flag recordType, RecordValue *record, int readFromRecord);
void map_calledAssertedID(flag recordType, RecordValue *record, int readFromRecord);
void map_callDescription(flag recordType, RecordValue *record, int readFromRecord);
void map_ctxGroupNumber(flag recordType, RecordValue *record, int readFromRecord);
void map_shortNumber(flag recordType, RecordValue *record, int readFromRecord);
void map_applicationServersInformation(flag recordType, RecordValue *record, int readFromRecord);
void map_privateUserID(flag recordType, RecordValue *record, int readFromRecord);

void map_recordOpeningTimeFraction(flag recordType, RecordValue *record, int readFromRecord);
void map_recordClosureTimeFraction(flag recordType, RecordValue *record, int readFromRecord);

void replaceUserLocationInfoFirstByte(char* output, const char* input);
void add_ba_field_diag(const char* output, const char* input);
void add_integer_field_diag(const char* output, const char* input);
void map_incompleteCDRIndication(flag recordType, RecordValue *record, int readFromRecord);
void map_total_volume(flag recordType, RecordValue *record, int readFromRecord);


int map_common(flag recordType, RecordValue *record, int readFromRecord, int isSCSCF);
int map_aSRecord(flag recordType, const char* triggerType, int triggerTypeOneEnabled, RecordValue *record, int readFromRecord, int isSCSCF);
int map_sCSCFRecord(flag recordType, const char* triggerType, int triggerTypeOneEnabled, RecordValue *record, int readFromRecord, int isSCSCF);

void map_aCName(flag recordType, RecordValue *record, int readFromRecord);
void map_valueaddedServiceIdentity(flag recordType, RecordValue *record, int readFromRecord);
void map_userChargingCategory(flag recordType, RecordValue *record, int readFromRecord);

void readField(Field *outputField, Block blocks[], int blockCount, char* address);
void store_oldListOfSDPMediaComponents();
int readBlockHierarchy(Field fields[], Block blocks[], const char* blockName, int blockIndex, int nextBlockLevel, int nextFieldIndex);
void enter_existing_output_block(Block blocks[]);
void write_fields_to_out(int fieldCount, Field fields[], int replaceBlockName, char* originalBlockName, char* replacementBlockName);
int epoch_to_compactLocal(char* output, const char* input, int is1900);
int compactLocalToComparableDateString(const char* input, char* output);
double getTimeDiff(const char* endDate, const char* startDate);
time_t stringToTime(const char* input);
void substring(char* output, const char *input, int offset, int length);
void str_to_low(char* output, const char *input);
int stringToByteArray(char* output, const char* input);
int byteArrayToString(char* output, const char* input, int size);
int hexToInt(char hex);
char* getFieldValueFromTRSRecord(const char* fieldName, RecordValue *record);
char* getFieldValueFromTRSRecordByIndex(const char* fieldName, RecordValue *record, int index);
int fieldExistsInTRSRecord(const char* fieldName, RecordValue *record);
int blockExistsInTRSRecord(const char* blockName, RecordValue *record);
int blockExistsInTRSRecordByIndex(const char* blockName, RecordValue *record, int index);
void clearAndCopyBlockToOut(const char* block_name, RecordValue *record);
void copyBlockToOut(const char* block_name, RecordValue *record);
void copyFieldToOut(const char* field_name, RecordValue *record);
void* split_string_malloc(int aSize);
void split_string_free_array(char **aTokensArray, int aTokensNumber);
int split_string(char ***aTokensArray, const char *aString, char aDelimiter);
int isFieldChanged(const char* inputFieldName, const char* outFieldName, int encodedOut, RecordValue *record, int readFromRecord);
void map_timestamps_block_exists(flag recordType);
void map_exponentField(char* inputFieldName, char* outputFieldName);
int get_ip_address(const char* address, char* output);
void write_field_to_output(Field *field);
void read_field_value_from_output(Field *field);
void enter_field_block(Field *field);
void addOrUpdateBlock(const char* blockName, int index);
void updateBlockToOut(const char* block_name, RecordValue *record, const char** repeat_block_name, const char* add_field_name);
void postPartialCDRHandling();
void enterBlockWithIndex(char* indexedBlockName);
char* double_without_trailing_zeros_to_str(double input);
int localtime_to_compactLocal(char* output);
void removeDuplicatesFromOut(const char* rootBlockName, const char* repeatingBlockName);
void map_involvedParty(const char* blockName, const char* fieldValue);
void map_involvedPartyRemoveDuplicate(const char* blockName, const char* fieldValue);
void addFieldRemoveDuplicate(const char* rootBlockName, const char* name, const char* value);
void copyRecordFieldsRemoveDuplicate(const char* recordFullName, const char* block, const char* name, RecordValue *record);
char* concatStr(const char *first, const char *second);
void clearOutBlock(const char* block);
DBBlock getBlockByIndex(const char* indexAsString);
int getBlockIndex(const char* mappedName);
char* get_field_name(char* line);
int get_level(const char *aString);
#endif /* FIELD_MAPPING_H */
