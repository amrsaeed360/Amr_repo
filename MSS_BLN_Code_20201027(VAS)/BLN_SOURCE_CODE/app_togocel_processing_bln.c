/************************** File Information ***********************************
* File Name                     :  app_togocel_processing_bln.c                *
* Client                        :  TogoCell                                    *
* Application                   :  Comptel Mediation EventLink                 *
* Functionality/Description     :  Performs branching and validation           *
* Last modified/Latest version  :  1.0.0                                       *
* Last modified date            :  17 OCT 2019                                 *
*******************************************************************************/

/************************** Version History ************************************
* New Version                   :  1.0.0                                       *
* Base Version                  :  None                                        *
* Created date                  :  18 Sept 2019                                 *
* Created by                    :  Manickashree                                *
* Description of Changes done   :  Initial Version                             *
*******************************************************************************/


/***********************************************************************
*
* Included libraries
*
***********************************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <regex.h>
#include <time.h>
#include <lookup_library.h>
#include <nodebase.h>
#include "app_togocel_processing_bln.h"

/* Include if needed
  #include <math.h>
  #include <stdbool.h>
*/

/***********************************************************************
*
* Reserved functions
*
***********************************************************************/

/* This function is called in the beginning when the node is starting up.
*
* Arguments:
*   None.
* Return values:
*   None.
*/
int LT_Special_Numbers;
char* Lookup_Server_Name;
char* VAS_VERT_Lookup_Table_Name ;
char* VAS_Special_Numbers_LookupTableName;
char* VAS_SMS_LookupTableName ;
char* VAS_USSD_LookupTableName ;
char*  Short_code ;
void node_init(void)
{
	DIAG(DIAG_LOW, "node_init(): entered the function");
    Lookup_Server_Name = (char*)nb_get_parameter_string("LookupServerName");
   if(strcmp(Lookup_Server_Name, "") == 0)
    {
        DIAG(DIAG_LOW, "LookupServerName mandatory parametr is not defined.");
        nb_abort();
    }
	
	char* Lookup_Table_Name = (char*)nb_get_parameter_string("LookupTableName");
	
	 VAS_VERT_Lookup_Table_Name = (char*)nb_get_parameter_string("VAS_VERT_LookupTableName");
	 VAS_Special_Numbers_LookupTableName = (char*)nb_get_parameter_string("VAS_Special_Numbers_LookupTableName");
	 VAS_SMS_LookupTableName = (char*)nb_get_parameter_string("VAS_SMS_LookupTableName");
	 VAS_USSD_LookupTableName = (char*)nb_get_parameter_string("VAS_USSD_LookupTableName");
	
   if(strcmp(Lookup_Table_Name, "") == 0 || strcmp(VAS_VERT_Lookup_Table_Name, "") == 0 || strcmp(VAS_Special_Numbers_LookupTableName, "") == 0 || strcmp(VAS_SMS_LookupTableName, "") == 0 || strcmp(VAS_USSD_LookupTableName, "") == 0)
    {
        DIAG(DIAG_LOW, "LookupTable mandatory parametr is not defined.");
        nb_abort();
    }
	DIAG(DIAG_LOW, "Lookup Server Name: %s", Lookup_Server_Name);
	DIAG(DIAG_LOW, "Lookup Table Name: %s", Lookup_Table_Name);
	DIAG(DIAG_LOW, "VAS_VERT_Lookup_Table_Name: %s", VAS_VERT_Lookup_Table_Name);
	DIAG(DIAG_LOW, "VAS_Special_Numbers_LookupTableName: %s", VAS_Special_Numbers_LookupTableName);
	DIAG(DIAG_LOW, "VAS_SMS_LookupTableName: %s", VAS_SMS_LookupTableName);
	DIAG(DIAG_LOW, "VAS_USSD_LookupTableName: %s", VAS_USSD_LookupTableName);
	
	 nb_load_library("server_lookup");
    if( ls_initialize() == -1 )
    {
        DIAG(DIAG_HIGH, "Failed to load Lookup Server library.");
        nb_abort();
    }
    LT_Special_Numbers = ls_lookup_get_table(Lookup_Server_Name, Lookup_Table_Name);
	
	
    if(LT_Special_Numbers < 0)
    {
        DIAG(DIAG_LOW, "Failed to load lookup table LT_Special_Numbers.");
        nb_msg1( "BLN001", "Lookup table 'LT_Special_Numbers' initialization failed" );
        nb_abort();
    }
	

	  Short_code = (char*)nb_get_parameter_string("short_codes");

    //********************** 
    // Read node parameters 
    //**********************
    DIAG(DIAG_LOW, "node_init(): exiting the function...");
}


/* This function is called for every input record or for every input
* file, if input is binary.
*
* Arguments:
*   None.
* Return values:
*   None.
*/
void node_process(void)
{
	DIAG(DIAG_HIGH, "node_process(): entered the function");
  
	initialize_input_output_fields();
	read_input_output_fields(); 
	//Validation Rules 
	if(app_validate() == FAIL) 
	{	  
		DIAG(DIAG_MEDIUM, "node_process(): Record failed validation.");
		return;
	}
	handle_conversion_rules();
	write_output_fields();	
	
	DIAG(DIAG_HIGH, "node_process(): returning...");
} 
   

/* This function is called whenever a control record is received.
*
* Arguments:
*   None.
* Return values:
*   None.
*/
void node_control(void)
{
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
void node_commit(void)
{
    DIAG(DIAG_LOW, "node_commit()");
}

/* This function is called in the end when the node is shutting down.
*
* Arguments:
*   None.
* Return values:
*   None.
*/
void node_end(void)
{
  DIAG(DIAG_LOW, "node_end()");
}

/* This function is called when the operator requested the flushing
* of the steram. If the node stores any records in an internal
* storage, all records should be retrieved from the storage and
* written to the output in this function.
*
* Arguments:
*   None.
* Return values:
*   None.
*/
void node_flush(void)
{
  DIAG(DIAG_LOW, "node_flush()");
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
void node_rollback(void)
{
  DIAG(DIAG_LOW, "node_rollback()");
}


/* This function is called in regular intervals, about every second.
*
* Arguments:
*   None.
* Return values:
*   None.
*/
void node_timer(void)
{
  DIAG(DIAG_HIGH, "node_timer()");  
}

/* This function is called for real-time nodes if they have to be stopped.
*
* Arguments:
*   None.
* Return values:
*   None.
*/
void node_pause(void)
{
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
void node_resume(void)
{
  DIAG(DIAG_LOW, "node_resume()");
}

/* This function is called upon receival of external requests.
 *
 * Arguments:
 *   None.
 * Return values:
 *   None.
 */
void node_request(void)
{
  DIAG(DIAG_LOW, "node_request()");
}

/***********************************************************************
*
* Application-specific functions
*
***********************************************************************/

/* -----------------------------------------------------------------------------
* Function  : iGet()
* Purpose   : Check Existence and Read Input Field
* Arguments : NameOfField - Name of the Input Field to be read
* Return    : Value of Field
* --------------------------------------------------------------------------- */
const char * iGet(const char *NameOfField)
{
  if(i_field_exists(NameOfField))
  {
    DIAG(DIAG_HIGH, "iGet(): %-30s = [%s]", NameOfField, i_get(NameOfField));
  }
  else
  {
    DIAG(DIAG_HIGH, "iGet(): %-30s = <Does not exist>", NameOfField);
  }
  return i_get(NameOfField);
}

/* -----------------------------------------------------------------------------
* Function  : initialize_input_output_fields()
* Purpose   : Initializes all the input fields
* Arguments : None
* Return    : None
* --------------------------------------------------------------------------- */
void initialize_input_output_fields()
{
	DIAG(DIAG_HIGH, "initialize_input_output_fields(): Initializes all the input fields...");

	memset(record_Type, '\0', sizeof(record_Type));
	memset(I_charging_start_time, '\0', sizeof(I_charging_start_time));
	memset(I_charging_end_time, '\0', sizeof(I_charging_end_time));
	memset(I_call_reference_time, '\0', sizeof(I_call_reference_time));	
	memset(I_in_channel_allocated_time, '\0', sizeof(I_in_channel_allocated_time));	
	memset(I_calling_subs_first_mcc, '\0', sizeof(I_calling_subs_first_mcc));
	memset(I_calling_subs_first_mnc, '\0', sizeof(I_calling_subs_first_mnc));
	memset(I_called_subs_first_mcc, '\0', sizeof(I_called_subs_first_mcc));
	memset(I_called_subs_first_mnc, '\0', sizeof(I_called_subs_first_mnc));
	memset(I_served_subs_mcc, '\0', sizeof(I_served_subs_mcc));
	memset(I_served_subs_mnc, '\0', sizeof(I_served_subs_mnc));
	memset(I_orig_calling_number, '\0', sizeof(I_orig_calling_number));
	memset(I_called_number, '\0', sizeof(I_called_number));
	memset(I_calling_number, '\0', sizeof(I_calling_number));
	memset(I_forwarding_number, '\0', sizeof(I_forwarding_number));	  
	memset(I_forwarded_to_number, '\0', sizeof(I_forwarded_to_number));
	memset(el_subscriber_type, '\0', sizeof(el_subscriber_type));
	memset(el_cdr_type, '\0', sizeof(el_cdr_type));
	memset(el_service_id, '\0', sizeof(el_service_id));
	memset(el_default_duration, '\0', sizeof(el_default_duration));
	memset(el_hni, '\0', sizeof(el_hni));
	memset(el_cell_id, '\0', sizeof(el_cell_id));
	memset(el_call_date, '\0', sizeof(el_call_date));
	memset(el_call_date_t, '\0', sizeof(el_call_date_t));
	memset(el_call_duration, '\0', sizeof(el_call_duration));
	memset(el_camel_flag, '\0', sizeof(el_camel_flag));
	memset(el_network_time, '\0', sizeof(el_network_time));
	memset(el_imei_number, '\0', sizeof(el_imei_number));
	memset(el_imsi, '\0', sizeof(el_imsi));
	memset(el_record_type, '\0', sizeof(el_record_type));	
	memset(el_trimmed_IMSI, '\0', sizeof(el_trimmed_IMSI));	
	memset(el_a_number, '\0', sizeof(el_a_number));
	memset(el_b_number, '\0', sizeof(el_b_number));
	memset(el_originator, '\0', sizeof(el_originator));
	memset(el_destination, '\0', sizeof(el_destination));  
	memset(el_utc_offset, '\0', sizeof(el_utc_offset));	 
	memset(el_updated_location_info, '\0', sizeof(el_updated_location_info)); 
	memset(el_destination_updated, '\0', sizeof(el_destination_updated));  
	memset(el_partial_indicator, '\0', sizeof(el_partial_indicator));  
	memset(el_b_number_updated, '\0', sizeof(el_b_number_updated)); 
	memset(el_HNI_IMSI, '\0', sizeof(el_HNI_IMSI));   
	memset(el_originator_updated, '\0', sizeof(el_originator_updated));  
	memset(el_a_number_updated, '\0', sizeof(el_a_number_updated));  
	memset(el_PRM_ANumber_Series, '\0', sizeof(el_PRM_ANumber_Series));   
	memset(el_PRM_BNumber_Series, '\0', sizeof(el_PRM_BNumber_Series));   
	memset(el_Tele_Service, '\0', sizeof(el_Tele_Service));    
	memset(el_Destination_pre, '\0', sizeof(el_Destination_pre));  
	memset(el_BNumber_pre, '\0', sizeof(el_BNumber_pre));  
	memset(el_facility_usage, '\0', sizeof(el_facility_usage)); 
	memset(el_Camel_Call_REF, '\0', sizeof(el_Camel_Call_REF));  
	memset(el_Camel_Call_FLAG, '\0', sizeof(el_Camel_Call_FLAG));
	memset(el_Camel_Call_SUP, '\0', sizeof(el_Camel_Call_SUP)); 
	memset(el_short_code, '\0', sizeof(el_short_code));
	
	memset(VAS_Flag, '\0', sizeof(VAS_Flag));
	memset(VAS_Type, '\0', sizeof(VAS_Type));
	memset(EL_VAS_IN_TRUNKID, '\0', sizeof(EL_VAS_IN_TRUNKID));
	memset(EL_VAS_OUT_TRUNKID, '\0', sizeof(EL_VAS_OUT_TRUNKID));
	
	memset(EL_VERT_Dial_Case, '\0', sizeof(EL_VERT_Dial_Case));
	 memset(VAS_Subscriber_Type, '\0', sizeof(VAS_Subscriber_Type));  
   memset(SP_VAS_Flag, '\0', sizeof(SP_VAS_Flag)); 
   memset(USSD_PARAM, '\0', sizeof(USSD_PARAM)); 
   memset(EL_USSD_PARAMETER, '\0', sizeof(EL_USSD_PARAMETER));
	
	DIAG(DIAG_HIGH, "initialize_input_output_fields(): Done.");
	return;
}
/* -----------------------------------------------------------------------------
* Function  : ReadInputRecord()
* Purpose   : Read Input Record Variables
* Arguments : None
* Return    : None
* ---------------------------------------------------------------------------- */
void read_input_output_fields()  
{
	DIAG(DIAG_LOW, "read_input_output_fields(): Entered the function");
	
	strcpy(record_Type, i_get_input_type()); 
	DIAG(DIAG_HIGH, "read_input_output_fields(): RecordType is '%s'",record_Type);	
  
	strcpy(I_calling_subs_first_mcc, i_get("I_CALLING_SUBS_FIRST_MCC"));
	DIAG(DIAG_HIGH, "read_input_output_fields(): Field I_CALLING_SUBS_FIRST_MCC is '%s'", I_calling_subs_first_mcc);
	strcpy(I_calling_subs_first_mnc, i_get("I_CALLING_SUBS_FIRST_MNC"));
	DIAG(DIAG_HIGH, "read_input_output_fields(): Field I_CALLING_SUBS_FIRST_MNC is '%s'", I_calling_subs_first_mnc);
	
	strcpy(I_called_subs_first_mcc, i_get("I_CALLED_SUBS_FIRST_MCC"));
	DIAG(DIAG_HIGH, "read_input_output_fields(): Field I_CALLED_SUBS_FIRST_MCC is '%s'", I_called_subs_first_mcc);
	strcpy(I_called_subs_first_mnc, i_get("I_CALLED_SUBS_FIRST_MNC"));
	DIAG(DIAG_HIGH, "read_input_output_fields(): Field I_CALLED_SUBS_FIRST_MNC is '%s'", I_called_subs_first_mnc);
	
	strcpy(I_served_subs_mcc, i_get("I_SERVED_SUBS_MCC"));
	DIAG(DIAG_HIGH, "read_input_output_fields(): Field I_SERVED_SUBS_MCC is '%s'", I_served_subs_mcc);
	strcpy(I_served_subs_mnc, i_get("I_SERVED_SUBS_MNC"));
	DIAG(DIAG_HIGH, "read_input_output_fields(): Field I_SERVED_SUBS_MNC is '%s'", I_served_subs_mnc);
	
	DIAG(DIAG_LOW, "read_input_output_fields(): Leaving the function");
}

/* -----------------------------------------------------------------------------
* Function  : app_validate()
* Purpose   : Implements Record Validation Rules
* Arguments : None
* Return    : PASS or FAIL
* --------------------------------------------------------------------------- */
int app_validate(void)
{
	
  char rejString [256] = {'\0'};  
  /*I_charging_start_time = "";
  I_call_reference_time = "";*/

	
  DIAG(DIAG_HIGH, "app_validate(): Applying validation rules");
  
  strcpy(I_charging_start_time, i_get("I_CHARGING_START_TIME"));
  DIAG(DIAG_HIGH, "node_process(): Field I_charging_start_time is '%s'", I_charging_start_time);
  
  strcpy(I_call_reference_time, i_get("I_CALL_REFERENCE_TIME"));
  DIAG(DIAG_HIGH, "node_process(): Field I_call_reference_time is '%s'", I_call_reference_time);
  
  // charging_start_time
	if((strcmp(record_Type, "moc") == 0) || (strcmp(record_Type, "mtc") == 0) || (strcmp(record_Type, "poc") == 0) || 
	(strcmp(record_Type, "ptc") == 0) || (strcmp(record_Type, "cf") == 0))
	{
		if (i_field_exists("I_CHARGING_START_TIME"))
		{
			
			if ((CheckDate(I_charging_start_time) != FALSE) && (CheckTime(I_charging_start_time) != FALSE))
			{	
			DIAG(DIAG_HIGH, "app_validate(): Passed validation INVALID_TIMESTAMP.");
			}
		
		//return PASS;
				
			else
			{	
				DIAG(DIAG_MEDIUM, "app_validate(): Invalid CHARGING_START_TIME Timestamp ['%s'] for the record type '%s'",i_get("I_CHARGING_START_TIME"),record_Type);
				memset(rejString, '\0', sizeof(rejString));
				sprintf(rejString, "Reject Record '%s'. Invalid CHARGING_START_TIME Timestamp  [%s]",record_Type, i_get("I_CHARGING_START_TIME"));
				i_reject("INVALID_TIMESTAMP", rejString );
				return FAIL;
			}
		}
		else 
			DIAG(DIAG_HIGH, "app_validate(): I_CHARGING_START_TIME does not contain a value.");
					
	}	 

	//call_reference_time
	if((strcmp(record_Type, "smmo") == 0) || (strcmp(record_Type, "smmt") == 0)) 
	{
		if(i_field_exists("I_CALL_REFERENCE_TIME"))
		{
			if ((CheckDate(I_call_reference_time) != FALSE) && (CheckTime(I_call_reference_time) != FALSE))
			{
				DIAG(DIAG_HIGH, "app_validate(): Passed validation INVALID_TIMESTAMP.");	
			}
		
		//return PASS;	
			
			else 
			{
				DIAG(DIAG_MEDIUM, "app_validate(): Invalid CALL_REFERENCE_TIME  Timestamp ['%s']",i_get("I_CALL_REFERENCE_TIME"));
				memset(rejString, '\0', sizeof(rejString));
				sprintf(rejString, "Reject Record '%s'. Invalid CALL_REFERENCE_TIME Timestamp [%s]",record_Type, i_get("I_CALL_REFERENCE_TIME"));
				i_reject("INVALID_TIMESTAMP", rejString);
				return FAIL;
			}
		}
		else 
			DIAG(DIAG_HIGH, "app_validate(): I_CALL_REFERENCE_TIME does not contain a value.");
	}
	
	
	// calling_number
	if((strcmp(record_Type, "moc") == 0) || (strcmp(record_Type, "mtc") == 0) || (strcmp(record_Type, "poc") == 0) || 
	(strcmp(record_Type, "ptc") == 0) || (strcmp(record_Type, "smmo") == 0) || (strcmp(record_Type, "smmt") == 0))
	{
		if((i_field_exists("I_CALLING_NUMBER")) && (strlen(iGet("I_CALLING_NUMBER")) != 0))
		{
			DIAG(DIAG_HIGH, "app_validate(): Passed validation MISSING_FIELDS.");	
		}
		//return PASS;
			
	
		else
		{
			DIAG(DIAG_MEDIUM, "app_validate(): Invalid Value ['%s']",i_get("I_CALLING_NUMBER"));
			memset(rejString, '\0', sizeof(rejString));
			sprintf(rejString, "Reject Record '%s'. Missing calling number value [%s]",record_Type,i_get("I_CALLING_NUMBER"));
			i_reject("MISSING_FIELDS", rejString);
			return FAIL;
		}
	}	
		
	// called_number
	if((strcmp(record_Type, "moc") == 0) || (strcmp(record_Type, "mtc") == 0) || (strcmp(record_Type, "poc") == 0) || 
	(strcmp(record_Type, "ptc") == 0) || (strcmp(record_Type, "smmo") == 0) || (strcmp(record_Type, "smmt") == 0)) 
	{
		if((i_field_exists("I_CALLED_NUMBER")) && (strlen(iGet("I_CALLED_NUMBER")) != 0))
		{
			DIAG(DIAG_HIGH, "app_validate(): Passed validation MISSING_FIELDS.");
		}

		//return PASS;
		
		else 
		{
			DIAG(DIAG_MEDIUM, "app_validate(): Invalid Value ['%s']",i_get("I_CALLED_NUMBER"));
			memset(rejString, '\0', sizeof(rejString));
			sprintf(rejString, "Reject Record '%s'. Missing called number value [%s]",record_Type, i_get("I_CALLED_NUMBER"));
			i_reject("MISSING_FIELDS", rejString);
			return FAIL;
		}
	}	
			
	//forwarding_number
	if((strcmp(record_Type, "cf") == 0) ) 
	{
		if((i_field_exists("I_FORWARDING_NUMBER")) && (strlen(iGet("I_FORWARDING_NUMBER")) != 0))
		{
			DIAG(DIAG_HIGH, "app_validate(): Passed validation MISSING_FIELDS.");	
		}

		//return PASS;
	
		else 
		{
			DIAG(DIAG_MEDIUM, "app_validate(): Invalid Value ['%s']",i_get("I_FORWARDING_NUMBER"));
			memset(rejString, '\0', sizeof(rejString));
			sprintf(rejString, "Reject Record '%s'. Missing forwarding number value [%s]",record_Type, i_get("I_FORWARDING_NUMBER"));
			i_reject("MISSING_FIELDS", rejString);
			return FAIL;		
		}
	}			
		
	// forwarded_to_number
	if((strcmp(record_Type, "cf") == 0) ) 
	{
		if((i_field_exists("I_FORWARDED_TO_NUMBER")) && (strlen(iGet("I_FORWARDED_TO_NUMBER")) != 0))
		{
			DIAG(DIAG_HIGH, "app_validate(): Passed validation MISSING_FIELDS.");
		}
			
		//return PASS;
		
		else 
		{
			DIAG(DIAG_MEDIUM, "app_validate(): Invalid Value ['%s']",i_get("I_FORWARDED_TO_NUMBER"));
			memset(rejString, '\0', sizeof(rejString));
			sprintf(rejString, "Reject Record '%s'. Missing forwarded to number value [%s]",record_Type, i_get("I_FORWARDED_TO_NUMBER"));
			i_reject("MISSING_FIELDS", rejString);
			return FAIL;		
		}
	}	
	// orig_calling_number
	if((strcmp(record_Type, "cf") == 0)) 
	{
		if ((i_field_exists("I_ORIG_CALLING_NUMBER")) && (strlen(iGet("I_ORIG_CALLING_NUMBER")) != 0))
		{
			DIAG(DIAG_HIGH, "app_validate(): Passed validation MISSING_FIELDS.");
		}

		//return PASS;
	
		else 
		{
			DIAG(DIAG_MEDIUM, "app_validate(): Invalid Value ['%s']",i_get("I_ORIG_CALLING_NUMBER"));
			memset(rejString, '\0', sizeof(rejString));
			sprintf(rejString, "Reject Record '%s'. Missing orig calling number value [%s]",record_Type, i_get("I_ORIG_CALLING_NUMBER"));
			i_reject("MISSING_FIELDS", rejString);
			return FAIL;		
		}
	}
   
   
   
  DIAG(DIAG_HIGH, "app_validate(): Returning PASS");
  return PASS;
}

int Postpaid_Validate(void)
{
	  int status = FAIL;
		if((strcmp(record_Type, "moc") == 0) || (strcmp(record_Type, "mtc") == 0) || (strcmp(record_Type, "poc") == 0) || 
	(strcmp(record_Type, "ptc") == 0)  || (strcmp(record_Type, "smmo") == 0) || (strcmp(record_Type, "smmt") == 0))
	{
		if (i_field_exists("I_CAMEL_CALL_REFERENCE"))  
		{
			
			strncpy(el_Camel_Call_REF,i_get("I_CAMEL_CALL_REFERENCE"), 4);
			el_Camel_Call_REF[4] = 0;
			DIAG(DIAG_HIGH, "Postpaid_Validate(): Camel Call Refernce first 4 digits = '%s'." , el_Camel_Call_REF);
			if((strcmp(el_Camel_Call_REF, "FFFF") == 0))
			{	
			DIAG(DIAG_HIGH, "Postpaid_Validate(): CDR is postpaid.");
			 status =  PASS;
			}
		
		  
				
			else
			{	
				DIAG(DIAG_MEDIUM, "Postpaid_Validate() CDR is prepaid" );
				status =  FAIL;
			}
		}
		else 
			DIAG(DIAG_HIGH, "Postpaid_Validate(): I_CAMEL_CALL_REFERENCE does not exist.");
					
	}
	else if (strcmp(record_Type, "cf") == 0)
	{
		if (i_field_exists("I_SCP_CONNECTION"))  
		{	
			if((strcmp(iGet("I_SCP_CONNECTION"), "") == 0))  
			{	
				DIAG(DIAG_HIGH, "Postpaid_Validate(): CDR is CF postpaid.");
				status =  PASS;
			}
			else if ((strcmp(iGet("I_SCP_CONNECTION"), "10") == 0))
			{
				DIAG(DIAG_MEDIUM, "Postpaid_Validate() CDR is CF prepaid" );
				status =  FAIL;
				
			}
			
		}
		
	}
	return status;
}
int Short_code_validate()
{
	
	char Dialed [12] = {'\0'};
	int status = FAIL; 
	strcpy(el_short_code,Short_code);
	DIAG(DIAG_MEDIUM, " Short_code_validate() : short codes = %s",Short_code );
	
	if((strcmp(record_Type, "moc") == 0)  ||  (strcmp(record_Type, "smmo") == 0) )
	{
		if((i_field_exists("I_CALLED_NUMBER")))
	    {
			
			strcpy(Dialed, i_get("I_CALLED_NUMBER"));
			
		}
	}
	else if ((strcmp(record_Type, "cf") == 0))  
	{
		if((i_field_exists("I_FORWARDED_TO_NUMBER")))
	    {
			strcpy(Dialed, i_get("I_FORWARDED_TO_NUMBER"));
			
		}
		
	}
	 DIAG(DIAG_MEDIUM, " Short_code_validate() : Dialed   = %s",Dialed );
	  char * Numbers = strtok(el_short_code, "," );
	  while (Numbers != NULL)
        {
            DIAG(DIAG_MEDIUM, " Short_code_validate() : Number  = %s",Numbers );
                if (strcmp (Numbers,Dialed) == 0) {
                       return   PASS;
                }
                Numbers = strtok(NULL, ",");
        }
	
	return status;
		
		   	
}


int get_LookupTable_Result()
{
	DIAG(DIAG_MEDIUM, "get_LookupTable_Result():Entering Function");
	int result;
	char I_Calling [20] = {'\0'};
	
	if((strcmp(record_Type, "moc") == 0)  ||  (strcmp(record_Type, "smmo") == 0) )
	{
		if((i_field_exists("I_CALLING_NUMBER")))
	    {
			
			strcpy(I_Calling, i_get("I_CALLING_NUMBER"));
			 result = get_Special_Numbers_Status(LT_Special_Numbers,I_Calling);	
		}
	}
	else if ((strcmp(record_Type, "cf") == 0))  
	{
		if((i_field_exists("I_FORWARDING_NUMBER")))
	    {
			strcpy(I_Calling, i_get("I_FORWARDING_NUMBER"));
			result = get_Special_Numbers_Status(LT_Special_Numbers,I_Calling);
		}
		
	}

	 DIAG(DIAG_MEDIUM, "get_LookupTable_Result(): result = %d",result);
	return result;
}


int get_VAS_VERT_Number_Result()
{
	DIAG(DIAG_MEDIUM, "get_VAS_VERT_Number_Result():Entering Function");
	int VERT_result;
	int LT_VAS_Vert_Numbers=ls_lookup_get_table(Lookup_Server_Name,VAS_VERT_Lookup_Table_Name);
	
	DIAG(DIAG_MEDIUM, "get_VAS_VERT_Number_Result() : Lookup table Name %s , Table Index %d",VAS_VERT_Lookup_Table_Name,LT_VAS_Vert_Numbers);
	if ((strcmp(record_Type, "moc") == 0) )
	{
		 VERT_result = get_VAS_LKP_Number_Details(LT_VAS_Vert_Numbers,iGet("I_DIALLED_DIGITS"));	
	}
	else
	{
		VERT_result = 0;
		
	}
	 DIAG(DIAG_MEDIUM, "get_LookupTable_Result(): VERT_result = %d",VERT_result);
	return VERT_result;
}

int get_VAS_Special_Number_Result()
{
	DIAG(DIAG_MEDIUM, "get_VAS_Special_Number_Result():Entering Function");
	int VSpecial_result;
	int LT_VAS_Special_Numbers=ls_lookup_get_table(Lookup_Server_Name,VAS_Special_Numbers_LookupTableName);
	DIAG(DIAG_MEDIUM, "get_VAS_Special_Number_Result() : Lookup table Name %s , Table Index %d",VAS_Special_Numbers_LookupTableName,LT_VAS_Special_Numbers);
	
	if ((strcmp(record_Type, "moc") == 0))
	{
		 VSpecial_result = get_VAS_LKP_Number_Details(LT_VAS_Special_Numbers,iGet("I_DIALLED_DIGITS"));	
	}
	else
	{
		VSpecial_result = 0;
		
	}
	 DIAG(DIAG_MEDIUM, "get_LookupTable_Result(): VSpecial_result = %d",VSpecial_result);
	return VSpecial_result;
	
}

int get_VAS_SMS_Special_Number_Result()
{
	
   DIAG(DIAG_MEDIUM, "get_VAS_SMS_Special_Number_Result():Entering Function");
	int SMS_result;
	int LT_VAS_SMS_Numbers= ls_lookup_get_table(Lookup_Server_Name,VAS_SMS_LookupTableName);
	DIAG(DIAG_MEDIUM, "get_VAS_SMS_Special_Number_Result() : Lookup table Name %s , Table Index %d",VAS_SMS_LookupTableName,LT_VAS_SMS_Numbers);
	if ((strcmp(record_Type, "smmo") == 0))
	{
		 SMS_result = get_VAS_LKP_Number_Details(LT_VAS_SMS_Numbers,iGet("I_DIALLED_DIGITS"));	
	}  
	else
	{
		SMS_result = 0;
		
	}
	 DIAG(DIAG_MEDIUM, "get_VAS_SMS_Special_Number_Result(): SMS_result = %d",SMS_result);
	return SMS_result; 	
	
	
	
}


int get_VAS_USSD_Special_Number_Result()
{
	char* temp = {'\0'};
	char temp2 [40];
	char *e;
    int index;
	char USSD_Service [20];
	int LT_VAS_USSD_Numbers= ls_lookup_get_table(Lookup_Server_Name,VAS_USSD_LookupTableName);
	DIAG(DIAG_MEDIUM, "get_VAS_USSD_Special_Number_Result() : Lookup table Name %s , Table Index %d",VAS_USSD_LookupTableName,LT_VAS_USSD_Numbers);
   DIAG(DIAG_MEDIUM, "get_VAS_USSD_Special_Number_Result():Entering Function");
	int USSD_result;
	if ((strcmp(record_Type, "Sup") == 0) && (strcmp(iGet("I_SUPPLEMENTARY_SERVICE_CODE"), "F2") == 0) )
	{
		 DIAG(DIAG_MEDIUM, "get_VAS_USSD_Special_Number_Result(): I_SUPPLEMENTARY_SERVICE_CODE = %s",iGet("I_SUPPLEMENTARY_SERVICE_CODE"));
		 strcpy(USSD_PARAM,iGet("I_PARAMETERS"));
		  DIAG(DIAG_MEDIUM, "get_VAS_USSD_Special_Number_Result(): USSD_PARAM = %s",USSD_PARAM);
		 temp =  Remove_Filler(USSD_PARAM);
		  DIAG(DIAG_MEDIUM, "get_VAS_USSD_Special_Number_Result(): temp = %s",temp);
		   DIAG(DIAG_MEDIUM, "get_VAS_USSD_Special_Number_Result(): length of  temp = %d",strlen(temp));
		  if (strlen(temp) > 18)
		  {
			  substring(temp,20,(strlen(temp)-20),temp2);
			  DIAG(DIAG_MEDIUM, "get_VAS_USSD_Special_Number_Result(): temp2 = %s",temp2);
		      convert_HEX_ASCII(temp2,EL_USSD_PARAMETER);
		      DIAG(DIAG_MEDIUM, "get_VAS_USSD_Special_Number_Result(): EL_USSD_PARAMETER = %s",EL_USSD_PARAMETER);
              		
			 e = strchr(EL_USSD_PARAMETER, '*');
			 
			 if (e != '\0'){
				 
				index = (int)(e - EL_USSD_PARAMETER);

			 }
			 else{
				  e = strchr(EL_USSD_PARAMETER, '#');
				  index = (int)(e - EL_USSD_PARAMETER);
				   
			 }
			  substring(EL_USSD_PARAMETER,0,index,USSD_Service);
			   DIAG(DIAG_MEDIUM, "get_VAS_USSD_Special_Number_Result(): USSD_Service = %s",USSD_Service);
			 
			  
		       USSD_result = get_VAS_LKP_Number_Details(LT_VAS_USSD_Numbers,USSD_Service);
		  }
		  else
		  {
			  USSD_result = 0;
			  
		  }	  	
	}  
	else
	{
		USSD_result = 0;
		
	}
	 DIAG(DIAG_MEDIUM, "get_VAS_USSD_Special_Number_Result(): USSD_result = %d",USSD_result);
	return USSD_result; 	
	
	
	
}


int  get_Special_Numbers_Status(int lookupTableID, char* MSISDN)
{
	
	int Result;
	char temp[DATA_LEN]={'\0'};
	sprintf(temp, "%s %s %s" ,"0","=",MSISDN);
    DIAG(DIAG_LOW, "get_Special_Numbers_Status(): entered function");
	
    Result =  ls_ct_lookup(lookupTableID, temp);
	DIAG(DIAG_LOW, "get_Special_Numbers_Status(): MSISDN  = %s ",temp);
    DIAG(DIAG_LOW, "get_Special_Numbers_Status(): Resut  = %d ",Result);

    

    return Result;
}
// -- TGC to TGC Validate---// 


int TGCTGC_Validate(void)
{
	 int status = FAIL;
	 if((strcmp(record_Type, "poc") == 0) || (strcmp(record_Type, "ptc") == 0))
	{
		if (((i_field_exists("I_CALLING_NUMBER")) && (i_field_exists("I_CALLED_NUMBER"))) || ((i_field_exists("I_ORIG_CALLING_NUMBER")) && (i_field_exists("I_FORWARDING_NUMBER"))))
		{
			strncpy(el_PRM_ANumber_Series,el_a_number_updated, 5);
			el_PRM_ANumber_Series[5] = 0;
			strncpy(el_PRM_BNumber_Series,el_b_number_updated, 5);
			el_PRM_BNumber_Series[5] = 0;
			DIAG(DIAG_HIGH, "PRM A Number First five Digits  = '%s'" ,el_PRM_ANumber_Series );
			DIAG(DIAG_HIGH, "PRM B Number First five Digits = '%s'" ,el_PRM_BNumber_Series );
			if(((strcmp(el_PRM_ANumber_Series, "22870") != 0) && (strcmp(el_PRM_ANumber_Series, "22890") != 0) &&(strcmp(el_PRM_ANumber_Series, "22891") != 0) &&(strcmp(el_PRM_ANumber_Series, "22892") != 0) && (strcmp(el_PRM_ANumber_Series, "22893") != 0)) || ((strcmp(el_PRM_BNumber_Series, "22870") != 0)  && (strcmp(el_PRM_BNumber_Series, "22890") != 0)  && (strcmp(el_PRM_BNumber_Series, "22891") != 0)  && (strcmp(el_PRM_BNumber_Series, "22892") != 0)  && (strcmp(el_PRM_BNumber_Series, "22893") != 0) ))
			{
				
				DIAG(DIAG_HIGH, "TGCTGC_Validate(): CDR iS  NOT TGC TGC   with Calling Number '%s' and Called Number '%s'" ,el_a_number_updated , el_b_number_updated );
					
				status =  PASS;
			}
			else
			{
				DIAG(DIAG_HIGH, "TGCTGC_Validate(): CDR iS   TGC TGC   with Calling Number '%s' and Called Number '%s'" ,el_a_number_updated , el_b_number_updated );
					
				status =  FAIL;
				
			}
			
			
		}
		
	}
	 
	return status;
}
// PRM_Validation ---------//

int ROAM_Validate(void)
{
	  int status = FAIL;
		if ((i_field_exists("I_CALLING_IMSI")) || (i_field_exists("I_CALLED_IMSI")) || (i_field_exists("I_FORWARDING_IMSI"))) 
		{
			if((strcmp(record_Type, "moc") == 0) || (strcmp(record_Type, "smmo") == 0))
			{
				strncpy(el_trimmed_IMSI, i_get("I_CALLING_IMSI"), 5);
				el_trimmed_IMSI[5] = 0;
				DIAG(DIAG_HIGH, "First Five Digits in IMSI = '%s'" ,el_trimmed_IMSI );
				if((strcmp(el_trimmed_IMSI, "61501") != 0))    
				{	
					DIAG(DIAG_HIGH, "ROAM_Validate(): CDR iS Roaming Oringinating with IMSI '%s'" ,i_get("I_CALLING_IMSI") );
					
					status =  PASS;
				}
				
				else
				{	
					DIAG(DIAG_MEDIUM, "ROAM_Validate(): CDR is Local with Oringinating IMSI '%s'" ,i_get("I_CALLING_IMSI"));
					status =  FAIL;
				}	
			}
			else if ((strcmp(record_Type, "mtc") == 0) || (strcmp(record_Type, "smmt") == 0))
			{
				strncpy(el_trimmed_IMSI, i_get("I_CALLED_IMSI"), 5);
				el_trimmed_IMSI[5] = 0;
				DIAG(DIAG_HIGH, "First Five Digits in IMSI = '%s'" ,el_trimmed_IMSI );
				if((strcmp(el_trimmed_IMSI, "61501") != 0))  
				{	
					DIAG(DIAG_HIGH, "ROAM_Validate(): CDR iS Roaming Terminating with IMSI '%s'" ,i_get("I_CALLED_IMSI") );
					
					status =  PASS;
				}
				
				else
				{	
					DIAG(DIAG_MEDIUM, "ROAM_Validate() CDR is Local with Terminating IMSI '%s'" ,i_get("I_CALLED_IMSI"));
					
					status =  FAIL;
				}	
			}
			else if ((strcmp(record_Type, "cf") == 0))
			{
				strncpy(el_trimmed_IMSI, i_get("I_FORWARDING_IMSI"), 5);
				el_trimmed_IMSI[5] = 0;
				DIAG(DIAG_HIGH, "First Five Digits in IMSI =  '%s'" ,el_trimmed_IMSI );
				if((strcmp(el_trimmed_IMSI, "61501") != 0))  
				{	
					DIAG(DIAG_HIGH, "ROAM_Validate(): CDR iS Roaming CF with IMSI '%s'" ,i_get("I_FORWARDING_IMSI") );
					status =  PASS;
				}
				
				else
				{	
					DIAG(DIAG_MEDIUM, "ROAM_Validate() CDR is Local  with CF IMSI '%s'" ,i_get("I_FORWARDING_IMSI"));
					status =  FAIL;
				}	
				
			}
			
		}
		else 
		{
			DIAG(DIAG_HIGH, "ROAM_Validate(): I_CALLING_IMSI OR I_CALLED_IMSI OR I_FORWARDING_IMSI does not exist.");
		}		
	return status;
}


int  get_VAS_LKP_Number_Details(int lookupTableID, const char* MSISDN)
{
	
	int VERTResult; 
	char temp[DATA_LEN]={'\0'};
	sprintf(temp, "%s %s %s" ,"0","=",MSISDN);
    DIAG(DIAG_LOW, "get_VAS_LKP_Number_Details(): entered function");
	
    VERTResult =  ls_ct_lookup(lookupTableID, temp);
	DIAG(DIAG_LOW, "get_VAS_LKP_Number_Details(): KEY  = %s ",temp);
    DIAG(DIAG_LOW, "get_VAS_LKP_Number_Details(): Resut  = %d ",VERTResult);

    return VERTResult;
}




/* ------------------------------------------------------------------------------
* Function: handle_conversion_rules()
* Purpose : Generates all the output fields
* Arguments : None
* Return: None
* --------------------------------------------------------------------------- */
void handle_conversion_rules()
{
	DIAG(DIAG_LOW, "handle_conversion_rules(): Entered the function");
	
	time_t charging_start_time = 0;
	time_t charging_end_time = 0;
	time_t in_channel_allocated_time = 0;
	int duration=0 ,hni=0, network_time=0;

	
	strcpy(I_charging_start_time, i_get("I_CHARGING_START_TIME"));
	strcpy(I_charging_end_time, i_get("I_CHARGING_END_TIME"));
	strcpy(I_in_channel_allocated_time, i_get("I_IN_CHANNEL_ALLOCATED_TIME"));
		
	//EL_CDR_TYPE	
	if(i_field_exists("I_RECORD_TYPE"))
	{
		DIAG(DIAG_HIGH, "handle_conversion_rules(): EL_CDR_TYPE conversion rules.");
	
		if((strcmp(record_Type, "mtc") == 0) || (strcmp(record_Type, "smmt") == 0) || (strcmp(record_Type, "poc") == 0))
			strcpy(el_cdr_type,"IC"); 
		
		if((strcmp(record_Type, "moc") == 0) || (strcmp(record_Type, "smmo") == 0) || (strcmp(record_Type, "ptc") == 0))
			strcpy(el_cdr_type,"OG"); 
		
		if((strcmp(record_Type, "cf") == 0)) 
			strcpy(el_cdr_type,"CF"); 
		
		if((strcmp(record_Type, "Sup") == 0)) 
			strcpy(el_cdr_type,"USSD"); 
		
	
	//EL_SERVICE_ID
		if((strcmp(record_Type, "moc") == 0) || (strcmp(record_Type, "mtc") == 0) || (strcmp(record_Type, "poc") == 0) || (strcmp(record_Type, "ptc") == 0)|| (strcmp(record_Type, "cf") == 0))
			strcpy(el_service_id,"111"); 
		
		if((strcmp(record_Type, "smmo") == 0) || (strcmp(record_Type, "smmt") == 0) || (strcmp(record_Type, "Sup") == 0))
			strcpy(el_service_id,"444"); 
		
		
		
		DIAG(DIAG_HIGH, "handle_conversion_rules(): Field EL_CDR_TYPE is '%s'", el_cdr_type);
		DIAG(DIAG_HIGH, "handle_conversion_rules(): Field EL_SERVICE_ID is '%s'", el_service_id);
	}
	
	//EL_HNI
		
	if((i_field_exists("I_CALLING_SUBS_FIRST_MCC")) || (i_field_exists("I_CALLING_SUBS_FIRST_MNC")) || (i_field_exists("I_CALLED_SUBS_FIRST_MCC")) || (i_field_exists("I_CALLED_SUBS_FIRST_MNC")) || (i_field_exists("I_SERVED_SUBS_MCC")) || (i_field_exists("I_SERVED_SUBS_MNC")))
	{
		DIAG(DIAG_HIGH, "handle_conversion_rules(): EL_HNI conversion rules.");
		if(strcmp(record_Type, "moc") == 0)
			hni=atoi(I_calling_subs_first_mcc)+atoi(I_calling_subs_first_mnc); 
			  //sprintf(el_hni,"%s%s",I_calling_subs_first_mcc,I_calling_subs_first_mnc);
	
		if(strcmp(record_Type, "mtc") == 0)
			hni=atoi(I_called_subs_first_mcc)+atoi(I_called_subs_first_mnc);
			//sprintf(el_hni,"%s%s",I_called_subs_first_mcc,I_called_subs_first_mnc);
	
		if ((strcmp(record_Type, "smmo") == 0) || (strcmp(record_Type, "smmt") == 0))
			hni=atoi(I_served_subs_mcc)+atoi(I_served_subs_mnc);
			//sprintf(el_hni,"%s%s",I_served_subs_mcc,I_served_subs_mnc);

		sprintf(el_hni,"%d",hni);
		DIAG(DIAG_HIGH, "handle_conversion_rules(): Field EL_HNI is '%s'", el_hni);

	}	
	

	
	//EL_HNI_Updated
	
	if ((i_field_exists("I_CALLING_IMSI")) || (i_field_exists("I_CALLED_IMSI")) || (i_field_exists("I_FORWARDING_IMSI")) || (i_field_exists("I_SERVED_IMSI"))  )  
		{
			if((strcmp(record_Type, "moc") == 0) || (strcmp(record_Type, "smmo") == 0) || (strcmp(record_Type, "poc") == 0))
			{
				strcpy(el_HNI_IMSI, iGet("I_CALLING_IMSI"));
					
			}
			else if ((strcmp(record_Type, "mtc") == 0) || (strcmp(record_Type, "smmt") == 0) || (strcmp(record_Type, "ptc") == 0))
			{
				strcpy(el_HNI_IMSI, iGet("I_CALLED_IMSI"));
				
			}
			else if ((strcmp(record_Type, "cf") == 0))
			{
				strcpy(el_HNI_IMSI, iGet("I_FORWARDING_IMSI"));
				
				
			}
			else if ((strcmp(record_Type, "Sup") == 0))
			{
				strcpy(el_HNI_IMSI, iGet("I_SERVED_IMSI"));
				
			}
			
			DIAG(DIAG_HIGH, "EL_HNI_Updated Value =  '%s'" ,el_HNI_IMSI );
		}
		else 
		{
			DIAG(DIAG_HIGH, "handle_conversion_rules ():  EL_HNI_updated I_CALLING_IMSI OR I_CALLED_IMSI OR I_FORWARDING_IMSI does not exist.");
		}	
	
	

	
	// el_Tele_Service
	      if((strcmp(record_Type, "moc") == 0) || (strcmp(record_Type, "mtc") == 0) || (strcmp(record_Type, "poc") == 0) || (strcmp(record_Type, "ptc") == 0))
			{
				strcpy(el_Tele_Service, "11" );
			}
			else if((strcmp(record_Type, "smmo") == 0))
			{
				strcpy(el_Tele_Service, "22");
			}
			else if((strcmp(record_Type, "smmt") == 0))
			{
				strcpy(el_Tele_Service, "21");
			}
			else
			{
				strcpy(el_Tele_Service, "00");
			}
	
	
	///
	// EL_Facilty_usage
			if((strcmp(record_Type, "moc") == 0) || (strcmp(record_Type, "mtc") == 0) || (strcmp(record_Type, "poc") == 0) || (strcmp(record_Type, "ptc") == 0) || (strcmp(record_Type, "smmo") == 0) || (strcmp(record_Type, "smmt") == 0))
			{
				strcpy(el_facility_usage,iGet("I_FACILITY_USAGE"));
			}
			else if ((strcmp(record_Type, "cf") == 0))
			{
				
				strcpy(el_facility_usage,iGet("I_CAUSE_FOR_FORWARDING"));
			}
	
	

		//el_subscriber_type
		if ((strcmp(record_Type, "cf") == 0))
		{
			if((strcmp(iGet("I_SCP_CONNECTION"), "") == 0)) 
				strcpy(el_subscriber_type,"S");
			else
				strcpy(el_subscriber_type,"P");
			
		}
		else
		{
			strncpy(el_Camel_Call_SUP,i_get("I_CAMEL_CALL_REFERENCE"), 4);
			el_Camel_Call_SUP[4] = 0;
			if((strcmp(el_Camel_Call_SUP, "FFFF") == 0))
				strcpy(el_subscriber_type,"S");
			else
				strcpy(el_subscriber_type,"P");
		}
	//el_cell_id
	if((i_field_exists("I_CALLING_SUBS_LAST_CI")) || (i_field_exists("I_CALLED_SUBS_LAST_CI")) || (i_field_exists("I_SERVED_SUBS_CI")))
	{
		DIAG(DIAG_HIGH, "node_process(): el_cell_id conversion rules.");
		if(strcmp(record_Type, "moc") == 0) 
			strcpy(el_cell_id,iGet("I_CALLING_SUBS_LAST_CI"));
		
		if(strcmp(record_Type, "mtc") == 0) 
			strcpy(el_cell_id,iGet("I_CALLED_SUBS_LAST_CI"));
		
		if((strcmp(record_Type, "smmo") == 0) || (strcmp(record_Type, "smmt") == 0))
			strcpy(el_cell_id,iGet("I_SERVED_SUBS_CI"));
	}
	
	//el_default_duration
	if((i_field_exists("I_ORIG_MCZ_DURATION")) || (i_field_exists("I_TERM_MCZ_DURATION")) || (i_field_exists("I_FORW_MCZ_DURATION")))
	{
		DIAG(DIAG_HIGH, "node_process(): el_default_duration conversion rules.");
		if(strcmp(record_Type, "moc") == 0) 
			strcpy(el_default_duration,iGet("I_ORIG_MCZ_DURATION"));
		
		else if(strcmp(record_Type, "mtc") == 0) 
			strcpy(el_default_duration,iGet("I_TERM_MCZ_DURATION"));
		
		else if(strcmp(record_Type, "cf") == 0) 
			strcpy(el_default_duration,iGet("I_FORW_MCZ_DURATION"));
		
	DIAG(DIAG_HIGH, "handle_conversion_rules(): el_default_duration = '%s'",el_default_duration);	
	}
		if((strcmp(record_Type, "smmo") == 0) || (strcmp(record_Type, "smmt") == 0))
		{
			strcpy(el_default_duration,"0");
			DIAG(DIAG_HIGH, "handle_conversion_rules(): el_default_duration for SMS = '%s'",el_default_duration);
		}
		
	
	
	
	//EL_A_NUMBER
	if((i_field_exists("I_ORIG_CALLING_NUMBER") || i_field_exists("I_CALLING_NUMBER") || i_field_exists("I_SERVED_NUMBER")))  //I_SERVED_NUMBER
	{
		char el_Local_Prefix[3] = "228";
		DIAG(DIAG_HIGH, "handle_conversion_rules(): EL_A_NUMBER conversion rules.");
		if((strcmp(record_Type, "moc") == 0) || (strcmp(record_Type, "mtc") == 0) || (strcmp(record_Type, "poc") == 0) || (strcmp(record_Type, "ptc") == 0) || (strcmp(record_Type, "smmo") == 0) || (strcmp(record_Type, "smmt") == 0))
		{
			trimstring(iGet("I_CALLING_NUMBER"),'0',TRIM_BEGIN,el_a_number);
			trimstring(el_a_number,'+',TRIM_BEGIN,el_a_number);			
		}
		else if(strcmp(record_Type, "cf") == 0)
		{
			trimstring(iGet("I_ORIG_CALLING_NUMBER"),'0',TRIM_BEGIN,el_a_number);
			trimstring(el_a_number,'+',TRIM_BEGIN,el_a_number);
		}
		if(strcmp(record_Type, "moc") == 0 || strcmp(record_Type, "smmo") == 0 || strcmp(record_Type, "mtc") == 0 || strcmp(record_Type, "smmt") == 0 || (strcmp(record_Type, "poc") == 0) || (strcmp(record_Type, "ptc") == 0) )	
		{
			if((i_field_exists("I_CALLING_NUMBER_TON")))
			{
				
				if((strcmp(i_get("I_CALLING_NUMBER_TON"),"6") == 0))
				{
					
				 strcpy(el_a_number_updated,el_Local_Prefix);
				 strcat(el_a_number_updated,el_a_number);
				}
				else
				{
					strcpy(el_a_number_updated,el_a_number);
					
				}
				
			}
		}
		else if(strcmp(record_Type, "cf") == 0 )	
		{
			if((i_field_exists("I_ORIG_CALLING_NUMBER_TON")))
			{
				
				if((strcmp(i_get("I_ORIG_CALLING_NUMBER_TON"),"6") == 0))
				{
				 strcpy(el_a_number_updated,el_Local_Prefix);
				 strcat(el_a_number_updated,el_a_number);
				}
			    else
				{
					strcpy(el_a_number_updated,el_a_number);
					
				}
				
			}
		}
		else if (strcmp(record_Type, "Sup") == 0)
		{
			strcpy(el_a_number_updated,iGet("I_SERVED_NUMBER"));
			
		}

		DIAG(DIAG_HIGH, "handle_conversion_rules(): Field EL_A_NUMBER is '%s'", el_a_number);
	}	
	//EL_B_NUMBER		
	if((i_field_exists("I_CALLED_NUMBER") || i_field_exists("I_FORWARDING_NUMBER")))
	{	

	   char el_Local_Prefix[3] = "228";
		DIAG(DIAG_HIGH, "handle_conversion_rules(): EL_B_NUMBER conversion rules.");
		if((strcmp(record_Type, "moc") == 0) || (strcmp(record_Type, "mtc") == 0) || (strcmp(record_Type, "poc") == 0) || (strcmp(record_Type, "ptc") == 0) || (strcmp(record_Type, "smmo") == 0) || (strcmp(record_Type, "smmt") == 0))
		{
			trimstring(iGet("I_CALLED_NUMBER"),'0',TRIM_BEGIN,el_b_number);
			trimstring(el_b_number,'+',TRIM_BEGIN,el_b_number);
		}
		else if(strcmp(record_Type, "cf") == 0)
		{
			trimstring(iGet("I_FORWARDING_NUMBER"),'0',TRIM_BEGIN,el_b_number);
			trimstring(el_b_number,'+',TRIM_BEGIN,el_b_number);
		}
		DIAG(DIAG_HIGH, "handle_conversion_rules(): Field EL_B_NUMBER is '%s'", el_b_number);
		
		if((strcmp(record_Type, "moc") == 0) || (strcmp(record_Type, "smmo") == 0) || (strcmp(record_Type, "mtc") == 0 )|| (strcmp(record_Type, "smmt") == 0) || (strcmp(record_Type, "poc") == 0) || (strcmp(record_Type, "ptc") == 0))	
		{
			if((i_field_exists("I_CALLED_NUMBER_TON")))
			{
				strncpy(el_BNumber_pre, el_b_number, 3);
				el_BNumber_pre[3] = 0;
				if((strcmp(i_get("I_CALLED_NUMBER_TON"),"6") == 0) || (strcmp(i_get("I_CALLED_NUMBER_TON"),"2") == 0))
				{
					if((strcmp(el_BNumber_pre,"228") != 0) &&  ( (strlen(el_b_number) < 10) && (strlen(el_b_number) > 4 ) ) )
					{
						strcpy(el_b_number_updated,el_Local_Prefix);
				        strcat(el_b_number_updated,el_b_number);
						
					}
					else{
						strcpy(el_b_number_updated,el_b_number);
					}
				
				}
				else
				{
					strcpy(el_b_number_updated,el_b_number);
					
				}
				
			}
		}
		
		else if(strcmp(record_Type, "cf") == 0 )	
		{
			if((i_field_exists("I_FORWARDING_NUMBER_TON")))
			{

				if((strcmp(i_get("I_FORWARDING_NUMBER_TON"),"6") == 0)) 
				{
				  
				  strcpy(el_b_number_updated,el_Local_Prefix);
				  strcat(el_b_number_updated,el_b_number);
				}
			    else
				{
					strcpy(el_b_number_updated,el_b_number);
					
				}
				
			}
		}
		DIAG(DIAG_HIGH, "handle_conversion_rules(): Field el_b_number_updated is '%s'", el_b_number_updated);
	}
	
	//EL_CALL_DATE
	if((i_field_exists("I_CHARGING_START_TIME"))||(i_field_exists("I_CALL_REFERENCE_TIME")))
	{	
		DIAG(DIAG_HIGH, "handle_conversion_rules(): EL_CALL_DATE conversion rules.");
		if((strcmp(record_Type, "moc") == 0) || (strcmp(record_Type, "mtc") == 0) || (strcmp(record_Type, "poc") == 0) || (strcmp(record_Type, "ptc") == 0)|| (strcmp(record_Type, "cf") == 0))
			strcpy(el_call_date, iGet("I_CHARGING_START_TIME"));
		
		if((strcmp(record_Type, "smmo") == 0) || (strcmp(record_Type, "smmt") == 0))
			strcpy(el_call_date, iGet("I_CALL_REFERENCE_TIME"));
		
		sprintf(el_call_date_t, "%s", convert_date_time(el_call_date));
		DIAG(DIAG_HIGH, "el_call_date_t '%s'",el_call_date_t);
		
		DIAG(DIAG_HIGH, "handle_conversion_rules(): Field EL_CALL_DATE is '%s'", el_call_date);
	}
	
	//EL_CALL_DURATION
	
	if((i_field_exists("I_CHARGING_START_TIME"))||(i_field_exists("I_CHARGING_END_TIME")))
	{
		DIAG(DIAG_HIGH, "handle_conversion_rules(): EL_CALL_DURATION conversion rules.");			
		
		charging_start_time = convert_to_epoch(I_charging_start_time);			 
		charging_end_time = convert_to_epoch(I_charging_end_time);	
				
		if((strcmp(record_Type, "moc") == 0) || (strcmp(record_Type, "mtc") == 0) || (strcmp(record_Type, "poc") == 0) || (strcmp(record_Type, "ptc") == 0)|| (strcmp(record_Type, "cf") == 0))
		{
			duration = charging_end_time - charging_start_time;			
			sprintf(el_call_duration, "%d",duration);
			//sprintf(el_call_duration,"%s%s",I_charging_end_time,I_charging_start_time);
		}
		DIAG(DIAG_HIGH, "handle_conversion_rules(): Field EL_CALL_DURATION is '%s'", el_call_duration);
	}		
	 if((strcmp(record_Type, "smmo") == 0) || (strcmp(record_Type, "smmt") == 0) ||  (strcmp(record_Type, "Sup") == 0))
			strcpy(el_call_duration, "0");
		
		DIAG(DIAG_HIGH, "handle_conversion_rules(): Field EL_CALL_DURATION is '%s'", el_call_duration);

	
	
	//EL_NETWORK_TIME
	
	if((i_field_exists("I_CHARGING_END_TIME"))||(i_field_exists("I_IN_CHANNEL_ALLOCATED_TIME")))
	{			
		DIAG(DIAG_HIGH, "handle_conversion_rules(): EL_NETWORK_TIME conversion rules.");
		
		charging_end_time = convert_to_epoch(I_charging_end_time);					
		in_channel_allocated_time = convert_to_epoch(I_in_channel_allocated_time);	
		
		if((strcmp(i_get("I_IN_CHANNEL_ALLOCATED_TIME"), "") != 0))
		{
			network_time = charging_end_time - in_channel_allocated_time;
			sprintf(el_network_time, "%d" , network_time);
		}			
		else
			strcpy(el_network_time, "0");	
		
		DIAG(DIAG_HIGH, "handle_conversion_rules(): network_time is '%d'", network_time);
			//sprintf(el_network_time,"%s%s",I_charging_end_time,I_in_channel_allocated_time);
		
		DIAG(DIAG_HIGH, "handle_conversion_rules(): Field EL_NETWORK_TIME is '%s'", el_network_time);
	}
	
	//EL_CAMEL_FLAG
	DIAG(DIAG_HIGH, "handle_conversion_rules(): EL_CAMEL_FLAG conversion rules.");
	
	  
	if (strcmp(record_Type, "cf") == 0)
	{
		if((strcmp(iGet("I_SCP_CONNECTION"), "") == 0))
		{
			strcpy(el_camel_flag,"N");
		}
		else
		{
			strcpy(el_camel_flag,"Y");
		}
	}
	else
	{
		strncpy(el_Camel_Call_FLAG,i_get("I_CAMEL_CALL_REFERENCE"), 4);
		el_Camel_Call_FLAG[4] = 0;
		if((strcmp(el_Camel_Call_FLAG, "FFFF") == 0))
		{
			strcpy(el_camel_flag,"N");
		}
		else
		{
			strcpy(el_camel_flag,"Y");
		}
	}
	DIAG(DIAG_HIGH, "handle_conversion_rules(): Field EL_CAMEL_FLAG is '%s'", el_camel_flag);
	
	
	//EL_PARTIAL_INDICATOR
	DIAG(DIAG_HIGH, "handle_conversion_rules(): EL_PARTIAL_INDICATOR conversion rules.");
	
	if((strcmp(iGet("I_INTERMEDIATE_CHARGING_IND"), "0") == 0))
		strcpy(el_partial_indicator,"\0");
	else
		strcpy(el_partial_indicator,iGet("I_INTERMEDIATE_CHARGING_IND"));
	
	DIAG(DIAG_HIGH, "handle_conversion_rules(): Field EL_PARTIAL_INDICATOR is '%s'", el_partial_indicator);
	
	//EL_IMEI_NUMBER
	if((i_field_exists("I_CALLING_IMEI"))||(i_field_exists("I_CALLED_IMEI"))||(i_field_exists("I_FORWARDING_IMEI")))
	{
		DIAG(DIAG_HIGH, "handle_conversion_rules(): EL_IMEI_NUMBER conversion rules.");
		if((strcmp(record_Type, "moc") == 0)||(strcmp(record_Type, "smmo") == 0))
			strcpy(el_imei_number,iGet("I_CALLING_IMEI"));
	
		else if((strcmp(record_Type, "mtc") == 0)||(strcmp(record_Type, "smmt") == 0))
			strcpy(el_imei_number,iGet("I_CALLED_IMEI"));
	
		else if ((strcmp(record_Type, "cf") == 0))
		strcpy(el_imei_number,iGet("I_FORWARDING_IMEI"));
	
		DIAG(DIAG_HIGH, "handle_conversion_rules(): Field EL_IMEI_NUMBER is '%s'", el_imei_number);
	}
	
	
	//EL_IMSI
	if((i_field_exists("I_CALLING_IMSI"))||(i_field_exists("I_CALLED_IMSI"))||(i_field_exists("I_FORWARDING_IMSI")))
	{
		DIAG(DIAG_HIGH, "handle_conversion_rules(): EL_IMSI conversion rules.");
		if((strcmp(record_Type, "moc") == 0) || (strcmp(record_Type, "smmo") == 0))
			strcpy(el_imsi,iGet("I_CALLING_IMSI"));
		
		if((strcmp(record_Type, "mtc") == 0) || (strcmp(record_Type, "smmt") == 0))
			strcpy(el_imsi,iGet("I_CALLED_IMSI"));
		
		if(strcmp(record_Type, "cf") == 0)
			strcpy(el_imsi,iGet("I_FORWARDING_IMSI"));
		DIAG(DIAG_HIGH, "handle_conversion_rules(): Field EL_IMSI is '%s'", el_imsi);
	}
	//EL_Surpay_updated_Location_info  
	
	if((i_field_exists("I_CALLING_SUBS_FIRST_MCC")) || (i_field_exists("I_CALLING_SUBS_FIRST_MNC")) || (i_field_exists("I_CALLED_SUBS_FIRST_MCC")) || (i_field_exists("I_CALLED_SUBS_FIRST_MNC")) || (i_field_exists("I_SERVED_SUBS_MCC")) || (i_field_exists("I_SERVED_SUBS_MNC")) || (i_field_exists("I_CALLING_SUBS_FIRST_LAC")) || (i_field_exists("I_CALLING_SUBS_FIRST_CI")) || (i_field_exists("I_SERVED_SUBS_LAC")) || (i_field_exists("I_SERVED_SUBS_CI"))|| (i_field_exists("I_CALLED_SUBS_FIRST_LAC")) || (i_field_exists("I_CALLED_SUBS_FIRST_CI")) || (i_field_exists("I_FORWARDING_FIRST_MCC")) || (i_field_exists("I_FORWARDING_FIRST_MNC")) )
	{
		
		DIAG(DIAG_HIGH, "handle_conversion_rules(): EL_Surpay_updated_Location_info conversion rules.");
		
		 char  F1 [2] = {'\0','F'};
	     char  *FILLER = F1  ;
		
		if ((strlen(iGet("I_CALLING_SUBS_FIRST_MNC")) == 2) || (strlen(iGet("I_CALLED_SUBS_FIRST_MNC"))) == 2 || (strlen(iGet("I_SERVED_SUBS_MNC")) == 2) ||(strlen(iGet("I_FORWARDING_FIRST_MNC")) == 2) )
		{ 
		   FILLER = &F1[1];
		}
		else{
		  FILLER = &F1[0];
		}
		
		if(strcmp(record_Type, "moc") == 0)
		strcpy(el_updated_location_info,concatinate(iGet("I_CALLING_SUBS_FIRST_MCC"),iGet("I_CALLING_SUBS_FIRST_MNC"),FILLER,iGet("I_CALLING_SUBS_FIRST_LAC"),iGet("I_CALLING_SUBS_FIRST_CI")));

		else if(strcmp(record_Type, "mtc") == 0)
			strcpy(el_updated_location_info,concatinate(iGet("I_CALLED_SUBS_FIRST_MCC"),iGet("I_CALLED_SUBS_FIRST_MNC"),FILLER,iGet("I_CALLED_SUBS_FIRST_LAC"),iGet("I_CALLED_SUBS_FIRST_CI")));

		else if ((strcmp(record_Type, "smmo") == 0) || (strcmp(record_Type, "smmt") == 0))
			strcpy(el_updated_location_info,concatinate(iGet("I_SERVED_SUBS_MCC"),iGet("I_SERVED_SUBS_MNC"),FILLER,iGet("I_SERVED_SUBS_LAC"),iGet("I_SERVED_SUBS_CI")));
		
		else if ((strcmp(record_Type, "cf") == 0))
		{
			strcpy(el_updated_location_info,concatinate(iGet("I_FORWARDING_FIRST_MCC"),iGet("I_FORWARDING_FIRST_MNC"),FILLER,"00AA","BBCC"));
		}

		DIAG(DIAG_HIGH, "handle_Surpay_updated_Location_info: Field EL_Surpay_updated_Location_info is '%s'", el_updated_location_info);

	}	
	
	//EL_RECORD_TYPE
	if(strcmp(record_Type, "") != 0)
	{
		DIAG(DIAG_HIGH, "handle_conversion_rules(): EL_RECORD_TYPE conversion rules.");
		if(strcmp(record_Type, "moc") == 0)
			strcpy(el_record_type,"MOC");
		if(strcmp(record_Type, "mtc") == 0)
			strcpy(el_record_type,"MTC");
		if(strcmp(record_Type, "smmo") == 0)
			strcpy(el_record_type,"SMSMO");
		if(strcmp(record_Type, "smmt") == 0)
			strcpy(el_record_type,"SMSMT");
		if(strcmp(record_Type, "cf") == 0)
			strcpy(el_record_type,"CF");
		
		DIAG(DIAG_HIGH, "handle_conversion_rules(): Field EL_RECORD_TYPE is '%s'", el_record_type);
		
	} 
	//EL_ORIGINATOR
	if((i_field_exists("I_CALLING_NUMBER"))||(i_field_exists("I_ORIG_CALLING_NUMBER")))
	{
		
		char el_Local_Prefix[3] = "228";
		strcpy(I_orig_calling_number, i_get("I_ORIG_CALLING_NUMBER"));
		strcpy(I_calling_number, i_get("I_CALLING_NUMBER"));
				
		if(strcmp(record_Type, "cf") == 0)	
			strcpy(el_originator,replace(I_orig_calling_number, "+" ,"00"));
		else
			strcpy(el_originator,replace(I_calling_number,"+" ,"00"));
		
	
		if(strcmp(record_Type, "moc") == 0 || strcmp(record_Type, "smmo") == 0 || strcmp(record_Type, "mtc") == 0 || strcmp(record_Type, "smmt") == 0)	
		{
			if((i_field_exists("I_CALLING_NUMBER_TON")))
			{
				
				if((strcmp(i_get("I_CALLING_NUMBER_TON"),"6") == 0))
				{
					
				 strcpy(el_originator_updated,el_Local_Prefix);
				 strcat(el_originator_updated,el_originator);
				}
				else
				{
					strcpy(el_originator_updated,el_originator);
					
				}
				
			}
		}
		else if(strcmp(record_Type, "cf") == 0 )	
		{
			if((i_field_exists("I_ORIG_CALLING_NUMBER_TON")))
			{
				
				if((strcmp(i_get("I_ORIG_CALLING_NUMBER_TON"),"6") == 0))
				{
				 strcpy(el_originator_updated,el_Local_Prefix);
				 strcat(el_originator_updated,el_originator);
				}
			    else
				{
					strcpy(el_originator_updated,el_originator);
					
				}
				
			}
		}
			DIAG(DIAG_HIGH, "handle_conversion_rules(): Field EL_ORIGINATOR is '%s'", el_originator);
				
	}
	//EL_DESTINATION
	if((i_field_exists("I_CALLED_NUMBER"))||(i_field_exists("I_FORWARDED_TO_NUMBER")))
	{
		char el_destination_2[3] = "228";
		strcpy(I_called_number, i_get("I_CALLED_NUMBER"));
		strcpy(I_forwarded_to_number, i_get("I_FORWARDED_TO_NUMBER"));
		
		if(strcmp(record_Type, "cf") == 0)	
		{
			strcpy(el_destination,replace(I_forwarded_to_number, "+" ,"00"));
			//printf("%s\n", replace(I_forwarded_to_number, "+" ,"00"));
		}
		else{
			strcpy(el_destination,replace(I_called_number, "+" ,"00"));
		}
		
		if(strcmp(record_Type, "moc") == 0 || strcmp(record_Type, "smmo") == 0 || strcmp(record_Type, "mtc") == 0 || strcmp(record_Type, "smmt") == 0)	
		{
			if((i_field_exists("I_CALLED_NUMBER_TON")))
			{
				strncpy(el_Destination_pre, el_destination, 3);
				el_Destination_pre[3] = 0;
				if((strcmp(i_get("I_CALLED_NUMBER_TON"),"6") == 0) || (strcmp(i_get("I_CALLED_NUMBER_TON"),"2") == 0))
				{
					if((strcmp(el_Destination_pre,"228") != 0) && (strlen(el_destination) < 10))
					{
						strcpy(el_destination_updated,el_destination_2);
				        strcat(el_destination_updated,el_destination);
						
					}
					else{
						strcpy(el_destination_updated,el_destination);
					}
				}
				else
				{
					strcpy(el_destination_updated,el_destination);
					
				}
				
			}
		}
		
		else if(strcmp(record_Type, "cf") == 0 )	
		{
			if((i_field_exists("I_FORWARDED_TO_NUMBER_TON")))
			{
				
				if((strcmp(i_get("I_FORWARDED_TO_NUMBER_TON"),"6") == 0))
				{
				  
				  strcpy(el_destination_updated,el_destination_2);
				  strcat(el_destination_updated,el_destination);
				}
			    else
				{
					strcpy(el_destination_updated,el_destination);
					
				}
				
			}
		}
	
		
		DIAG(DIAG_HIGH, "handle_conversion_rules(): Field EL_DESTINATION is '%s'", el_destination);
		DIAG(DIAG_HIGH, "handle_conversion_rules(): Field EL_DESTINATION_2 is '%s'", el_destination_2);
	}
	
	//////////////VAS_Rules///////////////
	
	if((strcmp(record_Type, "moc") == 0) &&  (get_VAS_VERT_Number_Result() == 1))  
	{
		strcpy(VAS_Type,"VERT");
		strcpy(VAS_Flag,"1");
        DIAG(DIAG_HIGH, "VAS_Type = [%s]",VAS_Type);
         DIAG(DIAG_HIGH, "VAS_Flag = [%s]",VAS_Flag);		
		DIAG(DIAG_MEDIUM, "VAS_Validate(): Records written to VAS_VERT_PRM");
		
	}
    else if ((strcmp(record_Type, "moc") == 0) && (get_VAS_Special_Number_Result() == 1 ))
	{
		
		strcpy(VAS_Type,"Special");
		strcpy(VAS_Flag,"1");
        DIAG(DIAG_HIGH, "VAS_Type = [%s]",VAS_Type);
        DIAG(DIAG_HIGH, "VAS_Flag = [%s]",VAS_Flag);			
		DIAG(DIAG_MEDIUM, "VAS_Validate(): Records written to VAS_SPECIAL_PRM");
		
	}
	else if ((strcmp(record_Type, "smmo") == 0) && (get_VAS_SMS_Special_Number_Result() == 1 ))
	{
		strcpy(VAS_Type,"SMS");
		strcpy(VAS_Flag,"1");
        DIAG(DIAG_HIGH, "VAS_Type = [%s]",VAS_Type);
        DIAG(DIAG_HIGH, "VAS_Flag = [%s]",VAS_Flag);			
		DIAG(DIAG_MEDIUM, "VAS_Validate(): Records written to VAS_SMS_PRM");
		
	}  
	else if ((strcmp(record_Type, "Sup") == 0) && (get_VAS_USSD_Special_Number_Result() == 1 ))
	{
		strcpy(VAS_Type,"USSD");
		strcpy(VAS_Flag,"1");
        DIAG(DIAG_HIGH, "VAS_Type = [%s]",VAS_Type);
        DIAG(DIAG_HIGH, "VAS_Flag = [%s]",VAS_Flag);			
		DIAG(DIAG_MEDIUM, "VAS_Validate(): Records written to VAS_USSD_PRM");
		
	}

	DIAG(DIAG_LOW, "EL_VAS_OUT_TRUNKID(): Entering the function");
	if ((strcmp(VAS_Type,"VERT") == 0))
	{
		
		strcpy(EL_VAS_OUT_TRUNKID,ls_ct_value(1));
		
	}
	else if ((strcmp(VAS_Type,"Special") == 0)){
		strcpy(EL_VAS_OUT_TRUNKID,ls_ct_value(1));	
	}
	else if ((strcmp(VAS_Type,"SMS") == 0)){
		strcpy(EL_VAS_OUT_TRUNKID,ls_ct_value(1));	
	}
	else if ((strcmp(VAS_Type,"USSD") == 0)){
		strcpy(EL_VAS_OUT_TRUNKID,ls_ct_value(1));	
	}
	else{
		strcpy(EL_VAS_OUT_TRUNKID,"");
	}
	
	DIAG(DIAG_HIGH, "EL_VAS_OUT_TRUNKID(): applying convertion rules,EL_VAS_OUT_TRUNKID=[%s]",EL_VAS_OUT_TRUNKID); 
	
	
	
	////////////////////////////////////
	DIAG(DIAG_LOW, "handle_conversion_rules(): Leaving the function");
}

/*----------------------------- output functions
------------------------------------*/
void write_output_fields()
{	
	DIAG(DIAG_LOW, "write_output_fields(): Entered the function");
	int MResult ; 
	MResult = get_LookupTable_Result();	
	

	
	if(((strcmp(record_Type, "moc") == 0) ||  (strcmp(record_Type, "cf") == 0)) && (strcmp(VAS_Flag,"1") != 0 ))
	{
		
		if(Postpaid_Validate() == PASS ) {
			
			DIAG(DIAG_HIGH, "write_output_fields  CDR is  Postpaid checking if it is roam or not ... ");
			if(ROAM_Validate() == FAIL)
			{
				    
					DIAG(DIAG_HIGH, "write_output_fields :CDR is  Local  and postpaid ");
					if (Short_code_validate() == FAIL)
					{
						DIAG(DIAG_HIGH, "write_output_fields :NoT short code case  ");
						if (MResult == 0)
						{
							DIAG(DIAG_HIGH, "write_output_fields :NoT Special Number , will be sent to SP  ");
							write_surepay_voice();
						}
						else 
						{
							
							DIAG(DIAG_HIGH, "write_output_fields :Dial is  Special Number , will NOT be sent to SP  ");
							write_special_number_CDR();
						}
					}
					else 
						{
							
							DIAG(DIAG_HIGH, "write_output_fields :Dial is calling short code  , will NOT be sent to SP  ");
						}
			}
			else {
				
				DIAG(DIAG_HIGH, "write_output_fields : CDR is  Roam   will not be sent to SurePay ,Filtered_out");
				//return;
			}
		}
		else
		{
			DIAG(DIAG_HIGH, "write_output_fields : CDR is Prepaid   and will not be sent to SurePay,  Filtered_out ");
			// return;
		}
	}
	
	if((strcmp(record_Type, "smmo") == 0) && (strcmp(VAS_Flag,"1") != 0 ))
	{
		if(Postpaid_Validate() == PASS ) {
			
			DIAG(DIAG_HIGH, " CDR is  Postpaid checking if it is roam or not ... ");
			if(ROAM_Validate() == FAIL)
			{
				DIAG(DIAG_HIGH, "write_output_fields :CDR is  Local  and postpaid ");
			 	if (Short_code_validate() == FAIL)
					{
						DIAG(DIAG_HIGH, "write_output_fields :NoT short code case  ");
						if (MResult == 0)
						{
							DIAG(DIAG_HIGH, "write_output_fields :NoT Special Number , will be sent to SP  ");
							write_surepay_sms();
						}
						else 
						{
							
							DIAG(DIAG_HIGH, "write_output_fields :Dial is  Special Number , will NOT be sent to SP  ");
						}
					}
					else 
						{
							
							DIAG(DIAG_HIGH, "write_output_fields :Dial is calling short code  , will NOT be sent to SP  ");
						}
					
			}
			else {
				
				DIAG(DIAG_HIGH, "write_output_fields :CDR is  Roam   will not be sent to SurePay ,Filtered_out");
				//return;
			}
		}
		else
		{
			DIAG(DIAG_HIGH, "write_output_fields :CDR is Prepaid   and will not be sent to SurePay,  Filtered_out ");
			// return;
		}
	}	
	
	if ((strcmp(record_Type, "poc") == 0) ||  (strcmp(record_Type, "ptc") == 0))   
	{
			if ((TGCTGC_Validate() == PASS ) && (Short_code_validate() == FAIL) )
			{
				DIAG(DIAG_HIGH, "PRM is Passed Validation , Interconnect CDR NON TGC to TGC");
		        write_prm_interco();
			}
	}
	if((strcmp(record_Type, "moc") == 0) ||  (strcmp(record_Type, "mtc") == 0)  || (strcmp(record_Type, "cf") == 0))
	{
		if(( ROAM_Validate() == PASS ) && (Short_code_validate() == FAIL ))
		{
			if(Postpaid_Validate() == FAIL )
			{
				DIAG(DIAG_HIGH, "PRM is Passed Correlation Validation , Roaming Prepaid  voice CDR ");
				if (strcmp(el_call_duration, "0") != 0 )
				{
		    		write_prm_roam_corr();
					DIAG(DIAG_HIGH, "PRM is  Passed  Validation , NOT zero duration CDR");
				}
				else
				{
					DIAG(DIAG_HIGH, "PRM is not Passed  Validation , zero duration CDR");
				}
			}
			else if (Postpaid_Validate() == PASS )
			{
				DIAG(DIAG_HIGH, "PRM is Passed  Validation , Roaming Postpaid  voice CDR ");
				
				if (strcmp(el_call_duration, "0") != 0)
				{
					write_prm_roam();
					DIAG(DIAG_HIGH, "PRM is  Passed  Validation , NOT zero duration CDR");
				}
				else
				{
					DIAG(DIAG_HIGH, "PRM is not Passed  Validation , zero duration CDR");
				}
			}
		}
		
	}
	if ((strcmp(record_Type, "COC") == 0) ||  (strcmp(record_Type, "CTC") == 0))
	{
		DIAG(DIAG_HIGH, "PRM is Passed Validation , COC , or CTC  CDR ");
		write_prm_roam_corr();
		
	}
	if ((strcmp(record_Type, "smmo") == 0) ||  (strcmp(record_Type, "smmt") == 0))
	{
		if(( ROAM_Validate() == PASS ) && (Short_code_validate() == FAIL))
		{
			DIAG(DIAG_HIGH, "PRM is Passed Validation , Roaming SMS  CDR ");
			write_prm_roam();
		}
	}
	
	if (strcmp(VAS_Flag, "1") == 0){
		
		 DIAG(DIAG_HIGH, "VAS  is Passed Validation , VAS  CDR ");
		 write_VAS();
	}
	
	

	DIAG(DIAG_LOW, "write_output_fields(): Leaving the function");
}		
void write_surepay_voice()
{
	DIAG(DIAG_LOW, "write_surepay_voice(): Entered the function");
	
	nb_new_record();
	audit_out_type("Surepay_voice");
	o_add_field("TYPE",el_record_type);
	o_add_field("IMSI",el_imsi);
	o_add_field("Originator",el_originator_updated);
	o_add_field("Destination",el_destination_updated);
	o_add_field("date_time",el_call_date);
	o_add_field("location_info",iGet("I_EXCHANGE_ID"));
	//o_add_field("operator_id","");
	o_add_field("Volume",el_call_duration);
	o_add_field("orig_cdr_id",iGet("I_CALL_REFERENCE"));
	o_add_field("service_id","101");
	o_add_field("Info_transfer_cap","0");
	/* o_add_field("user_rate","");
	o_add_field("syn_asyn","");
	o_add_field("user_info_layer1",""); */  
	o_add_field("updated_location_info",el_updated_location_info);
	nb_write_record("SUREPAY");
	
	DIAG(DIAG_LOW, "write_surepay_voice(): Leaving the function");
}

void write_special_number_CDR()
{
	DIAG(DIAG_LOW, "write_special_number_CDR(): Entered the function");
	
	nb_new_record();
	audit_out_type("Public_number");
	o_add_field("TYPE",el_record_type);
	o_add_field("IMSI",el_imsi);
	o_add_field("Originator",el_originator_updated);
	o_add_field("Destination",el_destination_updated);
	o_add_field("date_time",el_call_date);
	o_add_field("location_info",iGet("I_EXCHANGE_ID"));
	//o_add_field("operator_id","");
	o_add_field("Volume",el_call_duration);
	o_add_field("orig_cdr_id",iGet("I_CALL_REFERENCE"));
	o_add_field("service_id","101");
	o_add_field("Info_transfer_cap","0");
	/* o_add_field("user_rate","");
	o_add_field("syn_asyn","");
	o_add_field("user_info_layer1",""); */  
	o_add_field("updated_location_info",el_updated_location_info);
	nb_write_record("PUBLIC_FMS");
	
	DIAG(DIAG_LOW, "write_special_number_CDR(): Leaving the function");
}
void write_surepay_sms()
{
	DIAG(DIAG_LOW, "write_surepay_sms(): Entered the function");
	
	nb_new_record();
	audit_out_type("Surepay_sms");
	o_add_field("TYPE",el_record_type);
	o_add_field("IMSI",el_imsi);
	o_add_field("Originator",el_originator_updated);
	o_add_field("Destination",el_destination_updated);
	o_add_field("date_time",el_call_date);
	o_add_field("location_info",iGet("I_EXCHANGE_ID"));
	//o_add_field("operator_id","");
	o_add_field("Volume","1");
	o_add_field("orig_cdr_id",iGet("I_CALL_REFERENCE"));
	o_add_field("updated_location_info",el_updated_location_info);
	nb_write_record("SUREPAY");
	
	DIAG(DIAG_LOW, "write_surepay_sms(): Leaving the function");
}

void write_prm_roam_corr()
{		
	DIAG(DIAG_LOW, "write_prm_roam_corr(): Entered the function");
	
	nb_new_record();
	audit_out_type(CDR_Type_cat("PRM",record_Type));
	o_add_field("record_type",iGet("I_RECORD_TYPE"));
	o_add_field("intermediate_record_number",iGet("I_INTERMEDIATE_RECORD_NUMBER"));
	o_add_field("NE_CODE",iGet("I_EXCHANGE_ID"));
	o_add_field("CDR_TYPE",el_cdr_type);
	o_add_field("SERVICE_ID",el_service_id);
	o_add_field("IN_TRUNK","");
	o_add_field("OUT_TRUNK","");
	o_add_field("HNI",el_HNI_IMSI);
	o_add_field("A_NUMBER",el_a_number_updated);
	o_add_field("B_NUMBER",el_b_number_updated);
	o_add_field("C_NUMBER",iGet("I_FORWARDED_TO_NUMBER"));
	/* o_add_field("SGSN_ADDRESS","");
	o_add_field("APN_ADDRESS","");
	o_add_field("PDP_ADDRESS",""); */	
	o_add_field("CALL_DATE",el_call_date);
	o_add_field("CALL_DURATION",el_call_duration);
	/* o_add_field("INCOMING_GPRS_VOLUME","");
	o_add_field("OUTGOING_GPRS_VOLUME",""); */
	o_add_field("TELESERVICE",el_Tele_Service);  //iGet("I_BASIC_SERVICE_TYPE")
	o_add_field("BEARERSERVICE","00");	
	o_add_field("CAMEL_FLAG",el_camel_flag);
	//o_add_field("CAMEL SERVICE LEVEL","");
	o_add_field("CAMEL_SERVICE_KEY",iGet("I_CAMEL_SERVICE_KEY"));		
	o_add_field("CAMEL_DEFAULT_CALL_HANDLING","");
	o_add_field("CAMEL_SERVER_ADDRESS",iGet("I_SCF_ADDRESS"));  //I_SCF_ADDRESS
	o_add_field("CAMEL_MSC_ADDRESS",iGet("I_CAMEL_EXCHANGE_ID"));
	o_add_field("CAMEL_CALL_REF_NUM",iGet("I_CAMEL_CALL_REFERENCE"));
	/* o_add_field("CAMEL_INIT_CF_INDICATOR","");
	o_add_field("CAMEL_DESTINATION_NUM",""); */		
	o_add_field("CAMEL_MODIFICATION",iGet("I_CAMEL_SMS_MODIFICATION"));
	//o_add_field("SUPPLIMENTARY_NUM","");
	o_add_field("NETWORK_TIME",el_network_time);
	o_add_field("REASON_FOR_CLEARDOWN",iGet("I_CAUSE_FOR_TERMINATION"));
	 o_add_field("PARTIAL_INDICATOR",el_partial_indicator);
	o_add_field("PARTIAL_CDR_SEQ_NUM",iGet("I_INTERMEDIATE_RECORD_NUMBER"));
	o_add_field("CHARGING_ID",""); 		
	o_add_field("IMEI_NUMBER",el_imei_number);
	// o_add_field("CDR_CHRONO_NUM","");
	o_add_field("SUBSCRIBER_TYPE",el_subscriber_type); 
	o_add_field("DIALLED_DIGITS",iGet("I_DIALLED_DIGITS"));
	 o_add_field("SUPPLEMENTARY_ACTION_CODE",el_facility_usage);  
	o_add_field("CELL_ID",el_cell_id); 
	o_add_field("UTC_OFFSET",(char*)nb_get_parameter_string("UTC_Offset"));		//iGet("I_UE_TIME_ZONE") //set as +0000 as temp solution till rule is clear
	 o_add_field("MNP_IDENTIFIER",iGet("I_NPDB_QUERY_STATUS"));
	/*o_add_field("P_CSCF_ADDRESS","");
	o_add_field("A_USERID","");
	o_add_field("B_USERID","");
	o_add_field("EVENT_REFERENCE","");
	o_add_field("APN_OPERATOR_ID","");	
	o_add_field("QCI","");
	o_add_field("MAXIMUM_BIT_RATE","");
	o_add_field("GUARANTEED_BIT_RATE","");
	o_add_field("ALLOCATION_AND_RETENTION_PRIORITY",""); */
	o_add_field("NETWORK_CALL_REFERENCE_NUMBER",Call_Reference_concatinate(iGet("I_CALL_REFERENCE"),iGet("I_CALL_REFERENCE_TIME"))); 
	o_add_field("ROAMING_DURATION", el_default_duration);
	// o_add_field("ANSWER_TIME","");
	o_add_field("RELEASE_TIME",iGet("I_CHARGING_END_TIME"));
	o_add_field("DISCONNECT_TIME",iGet("I_CHARGING_END_TIME"));
	o_add_field("DEFAULT_DURATION",el_call_duration); 				
	nb_write_record("PRM_roam_corr");	
	
	DIAG(DIAG_LOW, "write_prm_roam_corr(): Leaving the function");
}

void write_prm_roam()
{		
	DIAG(DIAG_LOW, "write_prm_roam(): Entered the function");
	
	nb_new_record();
	audit_out_type(CDR_Type_cat("PRM",record_Type));
	o_add_field("record_type",iGet("I_RECORD_TYPE"));
	o_add_field("intermediate_record_number",iGet("I_INTERMEDIATE_RECORD_NUMBER"));
	o_add_field("NE_CODE",iGet("I_EXCHANGE_ID"));
	o_add_field("CDR_TYPE",el_cdr_type);
	o_add_field("SERVICE_ID",el_service_id);
	o_add_field("IN_TRUNK","");
	o_add_field("OUT_TRUNK","");
	o_add_field("HNI",el_HNI_IMSI);
	o_add_field("A_NUMBER",el_a_number_updated);
	o_add_field("B_NUMBER",el_b_number_updated);
	o_add_field("C_NUMBER",iGet("I_FORWARDED_TO_NUMBER"));
	/* o_add_field("SGSN_ADDRESS","");
	o_add_field("APN_ADDRESS","");
	o_add_field("PDP_ADDRESS",""); */	
	o_add_field("CALL_DATE",el_call_date);
	o_add_field("CALL_DURATION",el_call_duration);
	/* o_add_field("INCOMING_GPRS_VOLUME","");
	o_add_field("OUTGOING_GPRS_VOLUME",""); */
	o_add_field("TELESERVICE",el_Tele_Service);  //iGet("I_BASIC_SERVICE_TYPE")
	o_add_field("BEARERSERVICE","00");	
	o_add_field("CAMEL_FLAG",el_camel_flag);
	//o_add_field("CAMEL SERVICE LEVEL","");
	o_add_field("CAMEL_SERVICE_KEY",iGet("I_CAMEL_SERVICE_KEY"));		
	o_add_field("CAMEL_DEFAULT_CALL_HANDLING","");
	o_add_field("CAMEL_SERVER_ADDRESS",iGet("I_SCF_ADDRESS"));  //I_SCF_ADDRESS
	o_add_field("CAMEL_MSC_ADDRESS",iGet("I_CAMEL_EXCHANGE_ID"));
	o_add_field("CAMEL_CALL_REF_NUM",iGet("I_CAMEL_CALL_REFERENCE"));
	/* o_add_field("CAMEL_INIT_CF_INDICATOR","");
	o_add_field("CAMEL_DESTINATION_NUM",""); */		
	o_add_field("CAMEL_MODIFICATION",iGet("I_CAMEL_SMS_MODIFICATION"));
	//o_add_field("SUPPLIMENTARY_NUM","");
	o_add_field("NETWORK_TIME",el_network_time);
	o_add_field("REASON_FOR_CLEARDOWN",iGet("I_CAUSE_FOR_TERMINATION"));
	 o_add_field("PARTIAL_INDICATOR",el_partial_indicator);
	o_add_field("PARTIAL_CDR_SEQ_NUM",iGet("I_INTERMEDIATE_RECORD_NUMBER"));
	o_add_field("CHARGING_ID",""); 		
	o_add_field("IMEI_NUMBER",el_imei_number);
	// o_add_field("CDR_CHRONO_NUM","");
	o_add_field("SUBSCRIBER_TYPE",el_subscriber_type); 
	o_add_field("DIALLED_DIGITS",iGet("I_DIALLED_DIGITS"));
	 o_add_field("SUPPLEMENTARY_ACTION_CODE",el_facility_usage);  
	o_add_field("CELL_ID",el_cell_id); 
	o_add_field("UTC_OFFSET",(char*)nb_get_parameter_string("UTC_Offset"));		//iGet("I_UE_TIME_ZONE") //set as +0000 as temp solution till rule is clear
	 o_add_field("MNP_IDENTIFIER",iGet("I_NPDB_QUERY_STATUS"));
	/*o_add_field("P_CSCF_ADDRESS","");
	o_add_field("A_USERID","");
	o_add_field("B_USERID","");
	o_add_field("EVENT_REFERENCE","");
	o_add_field("APN_OPERATOR_ID","");	
	o_add_field("QCI","");
	o_add_field("MAXIMUM_BIT_RATE","");
	o_add_field("GUARANTEED_BIT_RATE","");
	o_add_field("ALLOCATION_AND_RETENTION_PRIORITY",""); */
	o_add_field("NETWORK_CALL_REFERENCE_NUMBER",Call_Reference_concatinate(iGet("I_CALL_REFERENCE"),iGet("I_CALL_REFERENCE_TIME"))); 
	o_add_field("ROAMING_DURATION", el_default_duration);
	// o_add_field("ANSWER_TIME","");
	o_add_field("RELEASE_TIME",iGet("I_CHARGING_END_TIME"));
	o_add_field("DISCONNECT_TIME",iGet("I_CHARGING_END_TIME"));
	o_add_field("DEFAULT_DURATION",el_call_duration); 				
	nb_write_record("PRM_roam");	
	
	DIAG(DIAG_LOW, "write_prm_roam(): Leaving the function");
}

void write_prm_interco()
{		
	DIAG(DIAG_LOW, "write_prm_interco(): Entered the function");
	
	nb_new_record();
	audit_out_type("PRM");
	o_add_field("NE_CODE",iGet("I_EXCHANGE_ID"));
	o_add_field("CDR_TYPE",el_cdr_type);
	o_add_field("SERVICE_ID",el_service_id);
	o_add_field("IN_TRUNK",iGet("I_IN_CIRCUIT_GROUP"));
	o_add_field("OUT_TRUNK",iGet("I_OUT_CIRCUIT_GROUP"));
	o_add_field("HNI",el_HNI_IMSI);
	o_add_field("A_NUMBER",el_a_number_updated);
	o_add_field("B_NUMBER",el_b_number_updated);
	o_add_field("C_NUMBER",iGet("I_FORWARDED_TO_NUMBER"));
	/* o_add_field("SGSN_ADDRESS","");
	o_add_field("APN_ADDRESS","");
	o_add_field("PDP_ADDRESS",""); */	
	o_add_field("CALL_DATE",el_call_date);
	o_add_field("CALL_DURATION",el_call_duration);
	/* o_add_field("INCOMING_GPRS_VOLUME","");
	o_add_field("OUTGOING_GPRS_VOLUME",""); */
	o_add_field("TELESERVICE",el_Tele_Service);  //iGet("I_BASIC_SERVICE_TYPE")
	o_add_field("BEARERSERVICE","00");	
	o_add_field("CAMEL_FLAG",el_camel_flag);
	//o_add_field("CAMEL SERVICE LEVEL","");
	o_add_field("CAMEL_SERVICE_KEY",iGet("I_CAMEL_SERVICE_KEY"));		
	o_add_field("CAMEL_DEFAULT_CALL_HANDLING","");
	//o_add_field("CAMEL_SERVER_ADDRESS","");
	o_add_field("CAMEL_MSC_ADDRESS",iGet("I_CAMEL_EXCHANGE_ID"));
	o_add_field("CAMEL_CALL_REF_NUM",iGet("I_CAMEL_CALL_REFERENCE"));
	/* o_add_field("CAMEL_INIT_CF_INDICATOR","");
	o_add_field("CAMEL_DESTINATION_NUM",""); */		
	o_add_field("CAMEL_MODIFICATION",iGet("I_CAMEL_SMS_MODIFICATION"));
	//o_add_field("SUPPLIMENTARY_NUM","");
	o_add_field("NETWORK_TIME",el_network_time);
	o_add_field("REASON_FOR_CLEARDOWN",iGet("I_CAUSE_FOR_TERMINATION"));
	 o_add_field("PARTIAL_INDICATOR",el_partial_indicator);
	o_add_field("PARTIAL_CDR_SEQ_NUM",iGet("I_INTERMEDIATE_RECORD_NUMBER"));
	o_add_field("CHARGING_ID",""); 		
	o_add_field("IMEI_NUMBER",el_imei_number);
	// o_add_field("CDR_CHRONO_NUM","");
	o_add_field("SUBSCRIBER_TYPE",el_subscriber_type); 
	o_add_field("DIALLED_DIGITS",iGet("I_DIALLED_DIGITS"));
	 o_add_field("SUPPLEMENTARY_ACTION_CODE",el_facility_usage);  
	o_add_field("CELL_ID",el_cell_id); 
	o_add_field("UTC_OFFSET",(char*)nb_get_parameter_string("UTC_Offset"));		//iGet("I_UE_TIME_ZONE") //set as +0000 as temp solution till rule is clear
	 o_add_field("MNP_IDENTIFIER",iGet("I_NPDB_QUERY_STATUS"));
	/*o_add_field("P_CSCF_ADDRESS","");
	o_add_field("A_USERID","");
	o_add_field("B_USERID","");
	o_add_field("EVENT_REFERENCE","");
	o_add_field("APN_OPERATOR_ID","");	
	o_add_field("QCI","");
	o_add_field("MAXIMUM_BIT_RATE","");
	o_add_field("GUARANTEED_BIT_RATE","");
	o_add_field("ALLOCATION_AND_RETENTION_PRIORITY",""); */
	o_add_field("NETWORK_CALL_REFERENCE_NUMBER",Call_Reference_concatinate(iGet("I_CALL_REFERENCE"),iGet("I_CALL_REFERENCE_TIME"))); 
	o_add_field("ROAMING_DURATION", el_default_duration);
	// o_add_field("ANSWER_TIME","");
	o_add_field("RELEASE_TIME",iGet("I_CHARGING_END_TIME"));
	o_add_field("DISCONNECT_TIME",iGet("I_CHARGING_END_TIME"));
	o_add_field("DEFAULT_DURATION",el_call_duration); 				
	nb_write_record("PRM_interco");	
	
	DIAG(DIAG_LOW, "write_prm_interco(): Leaving the function");
}

void write_VAS()
{
		DIAG(DIAG_LOW, "write_VAS(): Entering the function");
		nb_new_record();
		audit_out_type("VAS");
		o_add_field("NE_CODE",iGet("I_EXCHANGE_ID"));
		o_add_field("CDR_TYPE",el_cdr_type);
		o_add_field("SERVICE_ID",el_service_id);
		o_add_field("IN_TRUNK",EL_VAS_IN_TRUNKID);
		o_add_field("OUT_TRUNK",EL_VAS_OUT_TRUNKID);
		o_add_field("HNI",el_HNI_IMSI);
		o_add_field("A_NUMBER",el_a_number_updated);
		o_add_field("B_NUMBER",el_b_number_updated);
		o_add_field("C_NUMBER",iGet("I_FORWARDED_TO_NUMBER"));
		/* o_add_field("SGSN_ADDRESS","");
		o_add_field("APN_ADDRESS","");
		o_add_field("PDP_ADDRESS",""); */	
		o_add_field("CALL_DATE",el_call_date);
		o_add_field("CALL_DURATION",el_call_duration);
		/* o_add_field("INCOMING_GPRS_VOLUME","");
		o_add_field("OUTGOING_GPRS_VOLUME",""); */
		o_add_field("TELESERVICE",el_Tele_Service);  //iGet("I_BASIC_SERVICE_TYPE")
		o_add_field("BEARERSERVICE","00");	
		o_add_field("CAMEL_FLAG",el_camel_flag);
		//o_add_field("CAMEL SERVICE LEVEL","");
		o_add_field("CAMEL_SERVICE_KEY",iGet("I_CAMEL_SERVICE_KEY"));		
		o_add_field("CAMEL_DEFAULT_CALL_HANDLING","");
		//o_add_field("CAMEL_SERVER_ADDRESS","");
		o_add_field("CAMEL_MSC_ADDRESS",iGet("I_CAMEL_EXCHANGE_ID"));
		o_add_field("CAMEL_CALL_REF_NUM",iGet("I_CAMEL_CALL_REFERENCE"));
		/* o_add_field("CAMEL_INIT_CF_INDICATOR","");
		o_add_field("CAMEL_DESTINATION_NUM",""); */		
		o_add_field("CAMEL_MODIFICATION",iGet("I_CAMEL_SMS_MODIFICATION"));
		//o_add_field("SUPPLIMENTARY_NUM","");
		o_add_field("NETWORK_TIME",el_network_time);
		o_add_field("REASON_FOR_CLEARDOWN",iGet("I_CAUSE_FOR_TERMINATION"));
		o_add_field("PARTIAL_INDICATOR",el_partial_indicator);
		o_add_field("PARTIAL_CDR_SEQ_NUM",iGet("I_INTERMEDIATE_RECORD_NUMBER"));
		o_add_field("CHARGING_ID",""); 		
		o_add_field("IMEI_NUMBER",el_imei_number);
		// o_add_field("CDR_CHRONO_NUM","");
		o_add_field("SUBSCRIBER_TYPE",el_subscriber_type); 
		o_add_field("DIALLED_DIGITS",iGet("I_DIALLED_DIGITS"));
		o_add_field("SUPPLEMENTARY_ACTION_CODE",el_facility_usage);  
		o_add_field("CELL_ID",el_cell_id); 
		o_add_field("UTC_OFFSET",(char*)nb_get_parameter_string("UTC_Offset"));		//iGet("I_UE_TIME_ZONE") //set as +0000 as temp solution till rule is clear
		o_add_field("MNP_IDENTIFIER",iGet("I_NPDB_QUERY_STATUS"));
		/*o_add_field("P_CSCF_ADDRESS","");
		o_add_field("A_USERID","");
		o_add_field("B_USERID","");
		o_add_field("EVENT_REFERENCE","");
		o_add_field("APN_OPERATOR_ID","");	
		o_add_field("QCI","");
		o_add_field("MAXIMUM_BIT_RATE","");
		o_add_field("GUARANTEED_BIT_RATE","");
		o_add_field("ALLOCATION_AND_RETENTION_PRIORITY",""); */
		o_add_field("NETWORK_CALL_REFERENCE_NUMBER",Call_Reference_concatinate(iGet("I_CALL_REFERENCE"),iGet("I_CALL_REFERENCE_TIME"))); 
		o_add_field("ROAMING_DURATION","");
		// o_add_field("ANSWER_TIME","");
		o_add_field("RELEASE_TIME",iGet("I_CHARGING_END_TIME"));
		o_add_field("DISCONNECT_TIME",iGet("I_CHARGING_END_TIME"));
		o_add_field("DEFAULT_DURATION",el_call_duration); 				
		nb_write_record("BLN_VAS_OUT");
	
	DIAG(DIAG_LOW, "write_VAS(): Leaving the function");
}

/* -----------------------------------------------------------------------------
* Function  : convert_date_time()
* Purpose   : Convert datetime in yyyy-MM-dd HH:mm:ss format
* Arguments : None
* Return    : None
* --------------------------------------------------------------------------- */	

char * convert_date_time(char * i_date_time)
{
	DIAG(DIAG_HIGH, "Entering convert_date_time() function"); 
	char year[DATA_LEN]={'\0'};
	char month[DATA_LEN]={'\0'};
	char date[DATA_LEN]={'\0'};
	char hours[DATA_LEN]={'\0'};
	char mins[DATA_LEN]={'\0'};
	char seconds[DATA_LEN]={'\0'};
	char el_date_time[DATA_LEN]={'\0'};
	DIAG(DIAG_HIGH, "Entering convert_date_time() function : Initialized the variables");
	
	strncpy(year,i_date_time,4);
	DIAG(DIAG_HIGH, "convert_date_time(): year=[%s]", year);
	strncpy(month,i_date_time+4,2);
	DIAG(DIAG_HIGH, "convert_date_time(): month=[%s]", month);
	strncpy(date,i_date_time+6,2);
	DIAG(DIAG_HIGH, "convert_date_time(): date=[%s]", date);
	strncpy(hours,i_date_time+8,2);  
	DIAG(DIAG_HIGH, "convert_date_time(): hours=[%s]", hours);
	strncpy(mins,i_date_time+10,2);
	DIAG(DIAG_HIGH, "convert_date_time(): mins=[%s]", mins);
	strncpy(seconds,i_date_time+12,2);
	DIAG(DIAG_HIGH, "convert_date_time(): seconds=[%s]", seconds);
	
	sprintf(el_date_time,"%s-%s-%s %s:%s:%s",year,month,date,hours,mins,seconds);	
	DIAG(DIAG_HIGH, "convert_date_time(): applying convertion rules,el_date_time=[%s]", el_date_time);
	return el_date_time;
} 
/* ----------------------------------------------------------------------------- 
 * Function: trimstring()
 * Purpose  - used to trim a specific character from the string
 * PARAMETERS:
 *    input        - input string which need to be padded
 *    trimmed_char - the character that is used trimmed
 *    pos          - the position at which padding is done TRIM_BEGIN for begining
 *                   of string and TRIM_END for end of string and TRIM_BOTH for
 *                   both ends
 *    output       - the output string to which the result of trimming is written into
 *  EXAMPLE: trimstring(called_number,'0',TRIM_BEGIN,o_called_number);
 * -----------------------------------------------------------------------------*/
void trimstring(const char *input,char trimmed_char,int pos,char *output)
{
  int i;
  int j;
  int tmp;
  int input_len = 0;

  input_len = strlen(input);

  i = 0;
  j = input_len;
  
  DIAG(DIAG_LOW, "trimstring(): entered the function");
  DIAG(DIAG_HIGH, "trimstring(): Input Value is '%s'",input);

  if(input_len > 0)
  {
    if(pos == TRIM_BEGIN || pos == TRIM_BOTH)
    {
      for(i = 0; i < input_len && input[i] == trimmed_char; i++);
    }
    if(pos == TRIM_END || pos == TRIM_BOTH)
    {
      for(j = input_len; j > 0 && input[j-1] == trimmed_char; j--);
    }

    tmp = j - i;
    if(tmp > 0)
    {
      strncpy(output,&input[i],tmp);
      output[tmp] = '\0';
    }
    else
    {
      strcpy(output,"");
    }
  }
  else
  {
    strcpy(output,"");
  }
  
  DIAG(DIAG_HIGH, "trimstring(): Output Value is '%s'",output);
  DIAG(DIAG_LOW, "trimstring(): Leaving the function");
}


/*********************************************
 * To check for valid date
 *********************************************/
int CheckDate(char *datestr)
{
	int DaysPerMonth[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
	int len, i;
	int year,month,day;
	char datetemp[12];
	char temp[13];
	char ttime[10];

	DIAG(DIAG_LOW, "CheckDate(): entered the function");
	
	DIAG(DIAG_HIGH, "CheckDate(): Input Value is '%s'",datestr);
	
	substring(datestr,0,8,datetemp);
	len = strlen(datetemp);
	DIAG(DIAG_HIGH, "CheckDate(): Substring Value is '%s'",datetemp);
	
	if (len != 6 && len != 8 && len != 10 && len != 12) {
		DIAG(DIAG_LOW, "CheckDate(): Leaving the function and length '%d'",len);
		return FALSE;
		
	}

	strcpy(temp,datetemp);

	for (i=0;i<len;i++) {
		if (!isdigit(temp[i])) {
		DIAG(DIAG_LOW, "CheckDate(): Leaving the function not valid number");
		return FALSE;
		
		}
	}

	if (len == 12) {
		strcpy(ttime, temp+8); strcat(ttime,"00");
		if (CheckTime(ttime)!=0) return -1;
		temp[8] = 0; year = atoi(temp+4);
		temp[4] = 0; day = atoi(temp+2);
		temp[2] = 0; month = atoi(temp);
		if (year < 1970 || year > 2100) {
		DIAG(DIAG_LOW, "CheckDate(): Leaving the function3");
		return FALSE;
		
		}
	}
	else if (len == 10) {
		strcpy(ttime, temp+6); strcat(ttime,"00");
		if (CheckTime(ttime) !=0) {
		return FALSE;
		}
		temp[6] = 0; year = atoi(temp+4);
		temp[4] = 0; day = atoi(temp+2);
		temp[2] = 0; month = atoi(temp);

		if (year < 0 || year > 99) {
		//DIAG(DIAG_LOW, "CheckDate(): Leaving the function4");
		return FALSE;
		
		}
	}
	else if (len == 8) {
		DIAG(DIAG_LOW, "CheckDate(): Entreing the function for len=8");
		day = atoi(temp+6);
		temp[6] = 0; month = atoi(temp+4);
		temp[4] = 0; year  = atoi(temp);
		if (year < 1970 || year > 2100) {		
		DIAG(DIAG_LOW, "CheckTime(): Leaving the function since year '%d'",year);
		return FALSE;
		}
	}
	else {
		day = atoi(temp+4);
		temp[4] = 0; month = atoi(temp+2);
		temp[2] = 0; year  = atoi(temp);
		if (year < 0 || year > 99) {
		DIAG(DIAG_LOW, "CheckTime(): Leaving the function since year '%d'",year);
		return FALSE;
		}
	}

	if (month < 1 || month > 12) {
		DIAG(DIAG_LOW, "CheckTime(): Leaving the function since month '%d'",month);
		return FALSE;
		
	}

	if (((year % 4) == 0 && (year % 100) != 0) || (year % 400) == 0)
		DaysPerMonth[1] = 29; else DaysPerMonth[1] = 28;

	if (day < 1 || day > DaysPerMonth[month-1]) {
		DIAG(DIAG_LOW, "CheckTime(): Leaving the function since day '%d'",day);
		return FALSE;
		
	}
	DIAG(DIAG_LOW, "CheckDate(): Leaving the function");
	return TRUE;	
}

/*********************************************
 * To check for valid time
 *********************************************/
int CheckTime(char *timestr)
{
	int len,hour,minutes,seconds,i;
	char temp[10];
	char tzone_hh[10];
	
	DIAG(DIAG_LOW, "CheckTime(): entered the function");
	DIAG(DIAG_HIGH, "CheckTime(): Input Value is '%s'",timestr);
	
	substring(timestr,8,6,tzone_hh);	
	len = strlen(tzone_hh);
	
	DIAG(DIAG_HIGH, "CheckTime(): Substring Value is '%s'",tzone_hh);
  
	if(len!=6) {
		DIAG(DIAG_LOW, "CheckTime(): Leaving the function since length '%d'",len);
		return FALSE;
	}
	strcpy(temp,tzone_hh);
	
	for (i=0;i<len;i++) {
		if (!isdigit(temp[i])) {
			DIAG(DIAG_LOW, "CheckTime(): Leaving the function not valid input");
		return FALSE;
		}
	}
	
	seconds = atoi(temp + 4);
	temp[4] = 0;
	minutes = atoi(temp + 2);
	temp[2] = 0;
	hour = atoi(temp);
	
	if (hour < 0 || hour > 23) {
		DIAG(DIAG_LOW, "CheckTime(): Leaving the function since hour '%d'",hour);
		return FALSE;

	}
	if (minutes < 0 || minutes > 59) {
		DIAG(DIAG_LOW, "CheckTime(): Leaving the function since mins '%d'",minutes);
		return FALSE;

	}
	if (seconds < 0 || seconds > 59) {	
		DIAG(DIAG_LOW, "CheckTime(): Leaving the function since secs '%d'",seconds);
		return FALSE;
	}
	DIAG(DIAG_LOW, "CheckTime(): Leaving the function");
	return TRUE;	
}

/* FUNCTION:  substring - Function to obtain substring from a string
 * PARAMETERS:
 *    input   - input string from which substring need to be extracted
 *    offset  - offset location from which the substring is to be extracted
 *    length  - length of the substring
 *    output  - the output string to which the substring is extracted           
 *  EXAMPLE: substring(number,2,10,output);  
 */
void substring(const char *input,int offset,int length,char *output)
{
    int i;
    int string_length;
    
    string_length = strlen(input);
    
    if(offset >= 0 && offset < string_length)
    {
        for(i=0; i < length && input[offset + i] != '\0'; i++)
        {
            output[i] = input[offset+i];
        }
        output[i] = '\0';    
    }
    else
    {
        strcpy(output,"");
    }
}
/* FUNCTION:  convert_to_epoch - Function to convert a time in YYYYMMDDHHMISS
 *            to its epoch equivalent integer
 * PARAMETERS:
 *    date   - input string which contains the time in YYYYMMDDHHMISS
 * RETURNS:
 *           - Returns time in epoch seconds             
 */
time_t convert_to_epoch(const char *date)
{
  time_t epoch;
  struct tm time_struct;
  
  DIAG(DIAG_LOW, "convert_to_epoch(): Entering the function");
  DIAG(DIAG_LOW, "convert_to_epoch() - input date = [%s] ", date);

  sscanf(date,"%4d%2d%2d%2d%2d%2d",&time_struct.tm_year,&time_struct.tm_mon,&time_struct.tm_mday,&time_struct.tm_hour,&time_struct.tm_min,&time_struct.tm_sec);

  time_struct.tm_mon  -= 1;
  time_struct.tm_year -= 1900;
  time_struct.tm_isdst = -1;

  epoch = mktime(&time_struct);
  
  DIAG(DIAG_LOW, "convert_to_epoch() - output epoch date = [%ld] ", epoch);
  DIAG(DIAG_LOW, "convert_to_epoch(): Leaving the function");
  
  return epoch;
}
char * replace(char* str, char* a, char* b)
{
    int len  = strlen(str);
    int lena = strlen(a); 
	int lenb = strlen(b);
	char* p;
    for (p = str; p = (strstr(p, a)); ++p) 
	{
        if (lena != lenb) 
		{
            memmove(p+lenb, p+lena,len - (p - str) + lenb);
		}
        memcpy(p, b, lenb);
		
	}
    return str;
} 
/*-----------------new method added by amr.saeed------------*/
char * concatinate(const char *s1,const char *s2,const char *s3,const char *s4,const char *s5)
{
	
    char *result = malloc(strlen(s1) + strlen(s2) +strlen(s3) +strlen(s4)+strlen(s5)+ 1); // +1 for the null-terminator
    strcpy(result, s1);
    strcat(result, s2);
	strcat(result, s3);
	strcat(result, s4);
	strcat(result, s5);
    return result;
}
char * Call_Reference_concatinate(const char *s1,const char *s2)
{
	 char *result = malloc(strlen(s1) + strlen(s2) + 1);
	 strcpy(result, s1);
     strcat(result, s2);
	 return result;
}

char * CDR_Type_cat(const char *s1,const char *s2)
{
	 char *result = malloc(strlen(s1) + strlen(s2) + 1);
	 strcpy(result, s1);
     strcat(result, s2);
	 return result;
	
}

void convert_HEX_ASCII(const char *input,char *output)
{   
	int c;
    int x = 0;
	int counter;
    char tmp[100];
	char word[100];
	lowercase(input,tmp);
	for (counter = 0 ; counter < strlen(tmp) ; counter+=2)
	{
		substring(tmp,counter,strlen(tmp),word);
		sscanf(word,"%2x",&c);		
		output[x] = (char)c;
		if(c == 0) break;
		word[0]='\0';
		 x++;
	}
    	
}
void lowercase(const char *input,char *output)
{
    int i;
    
    for(i = 0; i < strlen(input); i++)
    {
        if( input[i] >='A' && input[i] <='Z')
        {
            output[i] = input[i] + 32;
        }
        else
        {
            output[i] = input[i];
        } 
    }
    output[i] = '\0';
} 
 
char* Remove_Filler(char *num)
{
     DIAG(DIAG_HIGH, "Remove_Filler():entered Fubction");
     int i,len1;
     len1 = strlen(num);
     for ( i = len1; i >= 0 ; i--)
     {
        if (num[i-1] == 'F'){
        num[i-1] = 0;
     }
     else{
        num[len1] = 0;
        break;
     }
 }
 
 return num;
} 
 