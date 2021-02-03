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
void write_IMS_SP();
void write_IMS_PRM();
void write_IMS_DWH();
