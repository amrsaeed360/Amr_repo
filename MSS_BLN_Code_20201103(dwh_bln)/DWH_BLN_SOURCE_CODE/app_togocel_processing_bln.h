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
char I_charging_start_time[20];
char I_charging_end_time[20];
char I_in_channel_allocated_time[20];
char I_call_reference_time[20];
char I_calling_subs_first_mcc[20];
char I_calling_subs_first_mnc[20];
char I_called_subs_first_mcc[20] ;
char I_called_subs_first_mnc[20] ;
char I_served_subs_mcc[20] ;
char I_served_subs_mnc[20] ; 
char EL_FIRST_LAC[30];
char EL_LAST_LAC[30];
char EL_IN_CIRCUIT_GROUP_NAME[50];
char EL_OUT_CIRCUIT_GROUP_NAME[50];
char EL_SMMT_CALLING_NUMBER[50];
char SMMT_CAL_NUM[30];
char USSD_PARAM[50];
char EL_SUP_PARAM[50];
char EL_USSD_PARAMETER[50];
/********************************************************************************
*
* Function ProtoType declarations
*
*******************************************************************************/

const char * iGet(const char *NameOfField);
void initialize_input_output_fields();
void read_input_output_fields();
int app_validate(void);
void handle_conversion_rules();
void write_output_fields();
int CheckDate(char *datestr);
int CheckTime(char *timestr);
void write_MOC();		 
void write_MTC();
void write_CF();
void write_SMMO();
void write_SMMT();	
void write_POC();
void write_PTC();
void write_COC();
void write_CTC();
void write_SUP();
void write_SOC();
void write_STC();
char* swap_number(char *num);
void swap(char *x, char *y);
void convert_HEX_DEC(const char *input,char *output);
void set_FIRST_LAC_DEC();
void set_LAST_LAC_DEC();
void set_IN_CIRCUIT_GROUP_NAME();
void set_OUT_CIRCUIT_GROUP_NAME();
void lowercase(const char *input,char *output);
void convert_HEX_ASCII(const char *input,char *output);
void substring(const char *input,int offset,int length,char *output);
void set_SMMT_CAlling_NUMBER();
char* swap_number_F(char *num);
char* Remove_Filler(char *num);
void set_SUP_Parameters();