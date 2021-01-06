/************************** File Information ***********************************
* File Name                     :  app_togocel_processing_bln.h                 *
* Client                        :  TogoCell                               *
* Application                   :  Comptel Mediation EventLink                 *
* Functionality/Description     :  Header to app_togocel_processing_bln.c       *
* Last modified/Latest version  :  1.0.0                                       *
* Last modified date            :  18 Sept 2019                                 *
*******************************************************************************/

/************************** Version History ************************************
* New Version                   :  1.0.0                                       *
* Base Version                  :  None                                        *
* Created date                  :  18 Sept 2019                                 *
* Created by                    :  Manickashree                                     *
* Description of Changes done   :  Initial Version                             *
*******************************************************************************/

/*******************************************************************************
*
* MACRO Constants
*
*******************************************************************************/
/* Diagnostic level settings */
#define DIAG_LOW    10
#define DIAG_MEDIUM 50
#define DIAG_HIGH   90

#define PASS 0
#define FAIL -1
#define TRUE  1
#define FALSE 0
#define DATA_LEN 64


/* Constants used by the trimining function */
#define TRIM_BEGIN 0
#define TRIM_END   1
#define TRIM_BOTH  2

/*******************************************************************************
*
* Input Record Variables
*
*******************************************************************************/
char record_Type[25];
char el_cdr_type[5];
char el_service_id[5];
char el_hni[20];
char el_call_date[20];
char el_call_date_t[20];
char el_call_duration[10];  
char el_partial_indicator[10]; 
char el_camel_flag[5];
char el_subscriber_type[5];
char el_cell_id[20];						   
char el_default_duration[10];					
char el_network_time[10];
char el_imei_number[20];
char el_imsi[20];
char el_record_type[6];
char el_a_number[20];
char el_b_number[20];
char el_originator[20];
char el_destination[20]; 
char el_utc_offset[20];
char el_updated_location_info[20] ; 
char el_destination_updated[20];  
char el_b_number_updated[20];
char I_charging_start_time[20];
char I_charging_end_time[20];
char I_in_channel_allocated_time[20];
char I_call_reference_time[20];
char I_orig_calling_number[20];
char I_called_number[20];  
char I_calling_subs_first_mcc[20];
char I_calling_subs_first_mnc[20];
char I_called_subs_first_mcc[20] ;
char I_called_subs_first_mnc[20] ;
char I_calling_number[20];
char I_forwarding_number[20];  
char I_forwarded_to_number[20];
char I_served_subs_mcc[20] ;
char I_served_subs_mnc[20] ; 
char el_trimmed_IMSI[10];    
char el_PRM_ANumber_Series[10]; 
char el_PRM_BNumber_Series[10]; 
char el_originator_updated[20];
char el_HNI_IMSI[20];  
char el_a_number_updated[20];   
char el_Tele_Service[5]; 
char el_Destination_pre[4];  
char el_BNumber_pre[4]; 
char el_facility_usage[5];
char el_Camel_Call_REF[5];
char el_Camel_Call_FLAG[5];
char el_Camel_Call_SUP[5]; 
char el_short_code[200];
char el_camel_level[5];
char I_Calling[30];
char el_CallREF[50];
char el_CDRType[10];
char el_date_time[30];

/********************************************************************************
*
* Function ProtoType declarations
*
*******************************************************************************/

const char * iGet(const char *NameOfField);
void initialize_input_output_fields();
void read_input_output_fields();
int app_validate(void);
int Postpaid_Validate(void);
int TGCTGC_Validate(void);
int ROAM_Validate(void);
void handle_conversion_rules();
void write_output_fields();
int CheckDate(char *datestr);
int CheckTime(char *timestr);
void substring(const char *input,int offset,int length,char *output);
void trimstring(const char *input,char trimmed_char,int pos,char *output);
void write_surepay_voice();
void write_special_number_CDR();
void write_surepay_sms();
void write_prm_roam();
void write_prm_roam_corr();
void write_prm_interco();
time_t convert_to_epoch(const char *date);
char * convert_date_time(char * i_date_time);
char * replace(char* str, char* a, char* b);
char * concatinate(const char *s1,const char *s2,const char *s3,const char *s4,const char *s5);
char * Call_Reference_concatinate(const char *s1,const char *s2);
char * CDR_Type_cat(const char *s1,const char *s2);
int get_LookupTable_Result();
int  get_Special_Numbers_Status(int lookupTableID, char* MSISDN);
int Short_code_validate();
char* swap_number_F(char *num);
void swap(char *x, char *y);