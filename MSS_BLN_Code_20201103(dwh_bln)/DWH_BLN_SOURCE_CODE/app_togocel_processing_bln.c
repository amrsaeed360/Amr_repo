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

void node_init(void)
{
	

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
    memset(EL_FIRST_LAC, '\0', sizeof(EL_FIRST_LAC));
    memset(EL_LAST_LAC, '\0', sizeof(EL_LAST_LAC));
	
	memset(EL_IN_CIRCUIT_GROUP_NAME, '\0', sizeof(EL_IN_CIRCUIT_GROUP_NAME));
    memset(EL_OUT_CIRCUIT_GROUP_NAME, '\0', sizeof(EL_OUT_CIRCUIT_GROUP_NAME));
	memset(EL_SMMT_CALLING_NUMBER, '\0', sizeof(EL_SMMT_CALLING_NUMBER)); 
	memset(SMMT_CAL_NUM, '\0', sizeof(SMMT_CAL_NUM));  
	memset(USSD_PARAM, '\0', sizeof(USSD_PARAM));
	memset(EL_SUP_PARAM, '\0', sizeof(EL_SUP_PARAM));
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









/* ------------------------------------------------------------------------------
* Function: handle_conversion_rules()
* Purpose : Generates all the output fields
* Arguments : None
* Return: None
* --------------------------------------------------------------------------- */
void handle_conversion_rules()
{
	DIAG(DIAG_LOW, "handle_conversion_rules(): Entered the function");
		
	strcpy(I_charging_start_time, i_get("I_CHARGING_START_TIME"));
	strcpy(I_charging_end_time, i_get("I_CHARGING_END_TIME"));
	strcpy(I_in_channel_allocated_time, i_get("I_IN_CHANNEL_ALLOCATED_TIME"));


	set_FIRST_LAC_DEC();
	set_LAST_LAC_DEC();
	set_IN_CIRCUIT_GROUP_NAME();
	set_OUT_CIRCUIT_GROUP_NAME();
	set_SMMT_CAlling_NUMBER();
	set_SUP_Parameters();
	DIAG(DIAG_LOW, "handle_conversion_rules(): Leaving the function");
}

/*----------------------------- output functions
------------------------------------*/
void write_output_fields()
{	
	DIAG(DIAG_LOW, "write_output_fields(): Entered the function");
	if((strcmp(record_Type, "moc") == 0))
	{
			 write_MOC();		 
	}
	else if ((strcmp(record_Type, "mtc") == 0))
	{
		   write_MTC();
	}
	else if ((strcmp(record_Type, "cf") == 0))
	{
			write_CF();
	}
	else if((strcmp(record_Type, "smmo") == 0))
	{
		  write_SMMO();
	}	
	else if ((strcmp(record_Type, "smmt") == 0))
	{
		   write_SMMT();	
	}
	else if (strcmp(record_Type, "poc") == 0)   
	{
			write_POC();
	}
	else if(strcmp(record_Type, "ptc") == 0)
	{
		   write_PTC();
	}
	else if ((strcmp(record_Type, "COC") == 0))
	{	
		  write_COC();
	}
	else if ((strcmp(record_Type, "CTC") == 0))  
	{
		  write_CTC();
	}
	else if ((strcmp(record_Type, "Sup") == 0))  
	{
		   write_SUP();
	}
	else if ((strcmp(record_Type, "SOC") == 0))  
	{
		   write_SOC();
	}	
	else if ((strcmp(record_Type, "STC") == 0))  
	{
		   write_STC();
	}
	DIAG(DIAG_LOW, "write_output_fields(): Leaving the function");
}		


void write_MOC()
{		
	DIAG(DIAG_LOW, "write_MOC(): Entered the function");
	
	  nb_new_record();
	     audit_out_type("moc");
			o_add_field("I_RECORD_LENGTH",iGet("I_RECORD_LENGTH"));
			o_add_field("I_RECORD_TYPE",iGet("I_RECORD_TYPE"));
			o_add_field("I_RECORD_NUMBER",iGet("I_RECORD_NUMBER"));
			o_add_field("I_RECORD_STATUS",iGet("I_RECORD_STATUS"));
			o_add_field("I_CHECK_SUM",iGet("I_CHECK_SUM"));
			o_add_field("I_CALL_REFERENCE",iGet("I_CALL_REFERENCE"));
			o_add_field("I_EXCHANGE_ID",iGet("I_EXCHANGE_ID"));
			o_add_field("I_INTERMEDIATE_RECORD_NUMBER",iGet("I_INTERMEDIATE_RECORD_NUMBER"));
			o_add_field("I_INTERMEDIATE_CHARGING_IND",iGet("I_INTERMEDIATE_CHARGING_IND"));
			o_add_field("I_NUMBER_OF_SS_RECORDS",iGet("I_NUMBER_OF_SS_RECORDS"));
			o_add_field("I_CALLING_IMSI",iGet("I_CALLING_IMSI"));
			o_add_field("I_CALLING_IMEI",iGet("I_CALLING_IMEI"));
			o_add_field("I_CALLING_NUMBER",iGet("I_CALLING_NUMBER"));
			o_add_field("I_CALLING_CATEGORY",iGet("I_CALLING_CATEGORY"));
			o_add_field("I_CALLING_MS_CLASSMARK",iGet("I_CALLING_MS_CLASSMARK"));
			o_add_field("I_CALLED_IMSI",iGet("I_CALLED_IMSI"));
			o_add_field("I_CALLED_IMEI",iGet("I_CALLED_IMEI"));
			o_add_field("I_CALLED_NUMBER_TON",iGet("I_CALLED_NUMBER_TON"));
			o_add_field("I_CALLED_NUMBER",iGet("I_CALLED_NUMBER"));
			o_add_field("I_CALLED_MS_CLASSMARK",iGet("I_CALLED_MS_CLASSMARK"));
			o_add_field("I_DIALLED_DIGITS_TON",iGet("I_DIALLED_DIGITS_TON"));
			o_add_field("I_DIALLED_DIGITS",iGet("I_DIALLED_DIGITS"));
			o_add_field("I_CALLING_SUBS_FIRST_LAC",EL_FIRST_LAC);
				o_add_field("I_CALLING_SUBS_FIRST_CI",iGet("I_CALLING_SUBS_FIRST_CI"));
			o_add_field("I_CALLING_SUBS_LAST_EX_ID",iGet("I_CALLING_SUBS_LAST_EX_ID"));
			o_add_field("I_CALLING_SUBS_LAST_LAC",EL_LAST_LAC);
			o_add_field("I_CALLING_SUBS_LAST_CI",iGet("I_CALLING_SUBS_LAST_CI"));
			o_add_field("I_OUT_CIRCUIT_GROUP",iGet("I_OUT_CIRCUIT_GROUP"));
			o_add_field("I_OUT_CIRCUIT",iGet("I_OUT_CIRCUIT"));
			o_add_field("I_BASIC_SERVICE_TYPE",iGet("I_BASIC_SERVICE_TYPE"));
			o_add_field("I_BASIC_SERVICE_CODE",iGet("I_BASIC_SERVICE_CODE"));
			o_add_field("I_FACILITY_USAGE",iGet("I_FACILITY_USAGE"));
			o_add_field("I_NON_TRANSPARENCY_INDICATOR",iGet("I_NON_TRANSPARENCY_INDICATOR"));
			o_add_field("I_CHANNEL_RATE_INDICATOR",iGet("I_CHANNEL_RATE_INDICATOR"));
			o_add_field("I_IN_CHANNEL_ALLOCATED_TIME",iGet("I_IN_CHANNEL_ALLOCATED_TIME"));
			o_add_field("I_CHARGING_START_TIME",iGet("I_CHARGING_START_TIME"));
			o_add_field("I_CHARGING_END_TIME",iGet("I_CHARGING_END_TIME"));
			o_add_field("I_CAUSE_FOR_TERMINATION",iGet("I_CAUSE_FOR_TERMINATION"));
			o_add_field("I_CALL_TYPE",iGet("I_CALL_TYPE"));
			o_add_field("I_ORIG_MCZ_CHRG_TYPE",iGet("I_ORIG_MCZ_CHRG_TYPE"));
			o_add_field("I_ORIG_MCZ_DURATION",iGet("I_ORIG_MCZ_DURATION"));
			o_add_field("I_ORIG_MCZ_TARIFF_CLASS",iGet("I_ORIG_MCZ_TARIFF_CLASS"));
			o_add_field("I_ORIG_MCZ_PULSES",iGet("I_ORIG_MCZ_PULSES"));
			o_add_field("I_CALLED_MSRN_TON",iGet("I_CALLED_MSRN_TON"));
			o_add_field("I_CALLED_MSRN",iGet("I_CALLED_MSRN"));
			o_add_field("I_CALLING_NUMBER_TON",iGet("I_CALLING_NUMBER_TON"));
			o_add_field("I_INTERMEDIATE_CHRG_CAUSE",iGet("I_INTERMEDIATE_CHRG_CAUSE"));
			o_add_field("I_CALLING_MODIFY_PARAMETERS",iGet("I_CALLING_MODIFY_PARAMETERS"));
			o_add_field("I_ORIG_MCZ_MODIFY_PERCENT",iGet("I_ORIG_MCZ_MODIFY_PERCENT"));
			o_add_field("I_ORIG_MCZ_MODIFY_DIRECTION",iGet("I_ORIG_MCZ_MODIFY_DIRECTION"));
			o_add_field("I_ORIG_DIALLING_CLASS",iGet("I_ORIG_DIALLING_CLASS"));
			o_add_field("I_LEG_CALL_REFERENCE",iGet("I_LEG_CALL_REFERENCE"));
			o_add_field("I_CALL_REFERENCE_TIME",iGet("I_CALL_REFERENCE_TIME"));
			o_add_field("I_SPEECH_VERSION",iGet("I_SPEECH_VERSION"));
			o_add_field("I_CAMEL_CALL_REFERENCE",iGet("I_CAMEL_CALL_REFERENCE"));
			o_add_field("I_CAMEL_EXCHANGE_ID",iGet("I_CAMEL_EXCHANGE_ID"));
			o_add_field("I_CAMEL_EXCHANGE_ID_TON",iGet("I_CAMEL_EXCHANGE_ID_TON"));
			o_add_field("I_NUMBER_OF_ALL_IN_RECORDS",iGet("I_NUMBER_OF_ALL_IN_RECORDS"));
			o_add_field("I_NUMBER_OF_IN_RECORDS",iGet("I_NUMBER_OF_IN_RECORDS"));
			o_add_field("I_ORIG_MCZ_CHANGE_DIRECTION",iGet("I_ORIG_MCZ_CHANGE_DIRECTION"));
			o_add_field("I_ORIG_MCZ_CHANGE_PERCENT",iGet("I_ORIG_MCZ_CHANGE_PERCENT"));
			o_add_field("I_SCP_CONNECTION",iGet("I_SCP_CONNECTION"));
			o_add_field("I_LOC_ROUTING_NUMBER",iGet("I_LOC_ROUTING_NUMBER"));
			o_add_field("I_LOC_ROUTING_NUMBER_TON",iGet("I_LOC_ROUTING_NUMBER_TON"));
			o_add_field("I_NPDB_QUERY_STATUS",iGet("I_NPDB_QUERY_STATUS"));
			o_add_field("I_CDB_INDICATOR",iGet("I_CDB_INDICATOR"));
			o_add_field("I_CUG_INFORMATION",iGet("I_CUG_INFORMATION"));
			o_add_field("I_CUG_INTERLOCK",iGet("I_CUG_INTERLOCK"));
			o_add_field("I_CUG_OUTGOING_ACCESS",iGet("I_CUG_OUTGOING_ACCESS"));
			o_add_field("I_CALLED_SUBS_FIRST_MCC",iGet("I_CALLED_SUBS_FIRST_MCC"));
			o_add_field("I_CALLED_SUBS_FIRST_MNC",iGet("I_CALLED_SUBS_FIRST_MNC"));
			o_add_field("I_CALLED_SUBS_LAST_EX_ID_TON",iGet("I_CALLED_SUBS_LAST_EX_ID_TON"));
			o_add_field("I_CALLED_SUBS_LAST_MCC",iGet("I_CALLED_SUBS_LAST_MCC"));
			o_add_field("I_CALLED_SUBS_LAST_MNC",iGet("I_CALLED_SUBS_LAST_MNC"));
			o_add_field("I_CALLING_SUBS_FIRST_MCC",iGet("I_CALLING_SUBS_FIRST_MCC"));
			o_add_field("I_CALLING_SUBS_FIRST_MNC",iGet("I_CALLING_SUBS_FIRST_MNC"));
			o_add_field("I_CALLING_SUBS_LAST_EX_ID_TON",iGet("I_CALLING_SUBS_LAST_EX_ID_TON"));
			o_add_field("I_CALLING_SUBS_LAST_MCC",iGet("I_CALLING_SUBS_LAST_MCC"));
			o_add_field("I_CALLING_SUBS_LAST_MNC",iGet("I_CALLING_SUBS_LAST_MNC"));
			o_add_field("I_CALLING_SUBS_SECOND_MCC",iGet("I_CALLING_SUBS_SECOND_MCC"));
			o_add_field("I_CALLING_SUBS_SECOND_MNC",iGet("I_CALLING_SUBS_SECOND_MNC"));
			o_add_field("I_RADIO_NETWORK_TYPE",iGet("I_RADIO_NETWORK_TYPE"));
			o_add_field("I_REQ_AIR_INTERFACE_USER_RATE",iGet("I_REQ_AIR_INTERFACE_USER_RATE"));
			o_add_field("I_REQ_FIXED_NW_USER_RATE",iGet("I_REQ_FIXED_NW_USER_RATE"));
			o_add_field("I_USED_AIR_INTERFACE_USER_RATE",iGet("I_USED_AIR_INTERFACE_USER_RATE"));
			o_add_field("I_USED_FIXED_NW_USER_RATE",iGet("I_USED_FIXED_NW_USER_RATE"));
			o_add_field("I_SELECTED_CODEC",iGet("I_SELECTED_CODEC"));
			o_add_field("I_EMERGENCY_CALL_CATEGORY",iGet("I_EMERGENCY_CALL_CATEGORY"));
			o_add_field("I_PNI",iGet("I_PNI"));
			o_add_field("I_OUT_BNC_CONNECTION_TYPE",iGet("I_OUT_BNC_CONNECTION_TYPE"));
			o_add_field("I_OUTSIDE_CONTROL_PLANE_INDEX",iGet("I_OUTSIDE_CONTROL_PLANE_INDEX"));
			o_add_field("I_OUTSIDE_USER_PLANE_INDEX",iGet("I_OUTSIDE_USER_PLANE_INDEX"));
			o_add_field("I_GLOBAL_CALL_REFERENCE",iGet("I_GLOBAL_CALL_REFERENCE"));
			o_add_field("I_OPTIMAL_ROUTING_INDICATOR",iGet("I_OPTIMAL_ROUTING_INDICATOR"));
			o_add_field("I_HOT_BILLING_RECORD_NUMBER",iGet("I_HOT_BILLING_RECORD_NUMBER"));
			o_add_field("I_CALLED_IMEISV",iGet("I_CALLED_IMEISV"));
			o_add_field("I_CALLING_IMEISV",iGet("I_CALLING_IMEISV"));
			o_add_field("I_VIRTUAL_MSC_ID",iGet("I_VIRTUAL_MSC_ID"));
			o_add_field("I_VIRTUAL_MSC_ID_TON",iGet("I_VIRTUAL_MSC_ID_TON"));
			o_add_field("I_NBR_OF_ORIG_CAP_IN_RECS",iGet("I_NBR_OF_ORIG_CAP_IN_RECS"));
			o_add_field("I_NUMBER_OF_IN_ANNOUNCEMENTS",iGet("I_NUMBER_OF_IN_ANNOUNCEMENTS"));
			o_add_field("I_REDIRECTED_INDICATOR",iGet("I_REDIRECTED_INDICATOR"));
			o_add_field("I_CONNECTED_TO_NUMBER",iGet("I_CONNECTED_TO_NUMBER"));
			o_add_field("I_CONNECTED_TO_NUMBER_TON",iGet("I_CONNECTED_TO_NUMBER_TON"));
			o_add_field("I_IN_CATEGORY_KEY",iGet("I_IN_CATEGORY_KEY"));
			o_add_field("I_ADD_ROUTING_CATEGORY",iGet("I_ADD_ROUTING_CATEGORY"));
			o_add_field("I_ROUTING_CATEGORY",iGet("I_ROUTING_CATEGORY"));
			o_add_field("I_CALLING_CHARGING_AREA",iGet("I_CALLING_CHARGING_AREA"));
			o_add_field("I_REGIONAL_SUBS_INDICATOR",iGet("I_REGIONAL_SUBS_INDICATOR"));
			o_add_field("I_REGIONAL_SUBS_LOCATION_TYPE",iGet("I_REGIONAL_SUBS_LOCATION_TYPE"));
			o_add_field("I_ACCEPTABLE_CHANNEL_CODINGS",iGet("I_ACCEPTABLE_CHANNEL_CODINGS"));
			o_add_field("I_REQ_NUMBER_OF_CHANNELS",iGet("I_REQ_NUMBER_OF_CHANNELS"));
			o_add_field("I_REQ_OTHER_MODEM_TYPE",iGet("I_REQ_OTHER_MODEM_TYPE"));
			o_add_field("I_REQ_USER_INITIATED_MOD_IND",iGet("I_REQ_USER_INITIATED_MOD_IND"));
			o_add_field("I_USED_CHANNEL_CODING",iGet("I_USED_CHANNEL_CODING"));
			o_add_field("I_USED_NUMBER_OF_CHANNELS",iGet("I_USED_NUMBER_OF_CHANNELS"));
			o_add_field("I_USED_OTHER_MODEM_TYPE",iGet("I_USED_OTHER_MODEM_TYPE"));
			o_add_field("I_CALLING_CELL_BAND",iGet("I_CALLING_CELL_BAND"));
			o_add_field("I_MS_CLASSMARK3",iGet("I_MS_CLASSMARK3"));
			o_add_field("I_ORIG_MCZ_DURATION_TEN_MS",iGet("I_ORIG_MCZ_DURATION_TEN_MS"));
			o_add_field("I_IN_CIRCUIT",iGet("I_IN_CIRCUIT"));
			o_add_field("I_IN_CIRCUIT_GROUP",iGet("I_IN_CIRCUIT_GROUP"));
			o_add_field("I_IN_CIRCUIT_GROUP_NAME",EL_IN_CIRCUIT_GROUP_NAME);
			o_add_field("I_OUT_CIRCUIT_GROUP_NAME",EL_OUT_CIRCUIT_GROUP_NAME);
			o_add_field("I_OUTPULSED_NUMBER_TON",iGet("I_OUTPULSED_NUMBER_TON"));
			o_add_field("I_OUTPULSED_NUMBER",iGet("I_OUTPULSED_NUMBER"));
			o_add_field("I_CT_THIRD_PARTY_NUMBER_TON",iGet("I_CT_THIRD_PARTY_NUMBER_TON"));
			o_add_field("I_CT_THIRD_PARTY_NUMBER",iGet("I_CT_THIRD_PARTY_NUMBER"));
			o_add_field("I_EMLPP_LEVEL",iGet("I_EMLPP_LEVEL"));
	
				nb_write_record("DWH");	
	
	DIAG(DIAG_LOW, "write_MOC(): Leaving the function");
}

void write_MTC()
{		
	DIAG(DIAG_LOW, "write_MTC(): Entered the function");
	
	nb_new_record();
	audit_out_type("mtc");
		o_add_field("I_RECORD_LENGTH",iGet("I_RECORD_LENGTH"));
		o_add_field("I_RECORD_TYPE",iGet("I_RECORD_TYPE"));
		o_add_field("I_RECORD_NUMBER",iGet("I_RECORD_NUMBER"));
		o_add_field("I_RECORD_STATUS",iGet("I_RECORD_STATUS"));
		o_add_field("I_CHECK_SUM",iGet("I_CHECK_SUM"));
		o_add_field("I_CALL_REFERENCE",iGet("I_CALL_REFERENCE"));
		o_add_field("I_EXCHANGE_ID",iGet("I_EXCHANGE_ID"));
		o_add_field("I_INTERMEDIATE_RECORD_NUMBER",iGet("I_INTERMEDIATE_RECORD_NUMBER"));
		o_add_field("I_INTERMEDIATE_CHARGING_IND",iGet("I_INTERMEDIATE_CHARGING_IND"));
		o_add_field("I_NUMBER_OF_SS_RECORDS",iGet("I_NUMBER_OF_SS_RECORDS"));
		o_add_field("I_CALLING_NUMBER",iGet("I_CALLING_NUMBER"));
		o_add_field("I_CALLED_IMSI",iGet("I_CALLED_IMSI"));
		o_add_field("I_CALLED_IMEI",iGet("I_CALLED_IMEI"));
		o_add_field("I_CALLED_NUMBER",iGet("I_CALLED_NUMBER"));
		o_add_field("I_CALLED_CATEGORY",iGet("I_CALLED_CATEGORY"));
		o_add_field("I_CALLED_MS_CLASSMARK",iGet("I_CALLED_MS_CLASSMARK"));
		o_add_field("I_IN_CIRCUIT_GROUP",iGet("I_IN_CIRCUIT_GROUP"));
		o_add_field("I_IN_CIRCUIT",iGet("I_IN_CIRCUIT"));
		o_add_field("I_CALLED_SUBS_FIRST_LAC",EL_FIRST_LAC);
		o_add_field("I_CALLED_SUBS_FIRST_CI",iGet("I_CALLED_SUBS_FIRST_CI"));
		o_add_field("I_CALLED_SUBS_LAST_EX_ID",iGet("I_CALLED_SUBS_LAST_EX_ID"));
		o_add_field("I_CALLED_SUBS_LAST_LAC",EL_LAST_LAC);
		o_add_field("I_CALLED_SUBS_LAST_CI",iGet("I_CALLED_SUBS_LAST_CI"));
		o_add_field("I_BASIC_SERVICE_TYPE",iGet("I_BASIC_SERVICE_TYPE"));
		o_add_field("I_BASIC_SERVICE_CODE",iGet("I_BASIC_SERVICE_CODE"));
		o_add_field("I_FACILITY_USAGE",iGet("I_FACILITY_USAGE"));
		o_add_field("I_NON_TRANSPARENCY_INDICATOR",iGet("I_NON_TRANSPARENCY_INDICATOR"));
		o_add_field("I_CHANNEL_RATE_INDICATOR",iGet("I_CHANNEL_RATE_INDICATOR"));
		o_add_field("I_IN_CHANNEL_ALLOCATED_TIME",iGet("I_IN_CHANNEL_ALLOCATED_TIME"));
		o_add_field("I_CHARGING_START_TIME",iGet("I_CHARGING_START_TIME"));
		o_add_field("I_CHARGING_END_TIME",iGet("I_CHARGING_END_TIME"));
		o_add_field("I_CAUSE_FOR_TERMINATION",iGet("I_CAUSE_FOR_TERMINATION"));
		o_add_field("I_CALL_TYPE",iGet("I_CALL_TYPE"));
		o_add_field("I_TERM_MCZ_CHRG_TYPE",iGet("I_TERM_MCZ_CHRG_TYPE"));
		o_add_field("I_TERM_MCZ_DURATION",iGet("I_TERM_MCZ_DURATION"));
		o_add_field("I_TERM_MCZ_TARIFF_CLASS",iGet("I_TERM_MCZ_TARIFF_CLASS"));
		o_add_field("I_TERM_MCZ_PULSES",iGet("I_TERM_MCZ_PULSES"));
		o_add_field("I_CALLING_NUMBER_TON",iGet("I_CALLING_NUMBER_TON"));
		o_add_field("I_CALLED_NUMBER_TON",iGet("I_CALLED_NUMBER_TON"));
		o_add_field("I_INTERMEDIATE_CHRG_CAUSE",iGet("I_INTERMEDIATE_CHRG_CAUSE"));
		o_add_field("I_CALLED_MODIFY_PARAMETERS",iGet("I_CALLED_MODIFY_PARAMETERS"));
		o_add_field("I_TERM_MCZ_MODIFY_PERCENT",iGet("I_TERM_MCZ_MODIFY_PERCENT"));
		o_add_field("I_TERM_MCZ_MODIFY_DIRECTION",iGet("I_TERM_MCZ_MODIFY_DIRECTION"));
		o_add_field("I_LEG_CALL_REFERENCE",iGet("I_LEG_CALL_REFERENCE"));
		o_add_field("I_OUT_CHANNEL_ALLOCATED_TIME",iGet("I_OUT_CHANNEL_ALLOCATED_TIME"));
		o_add_field("I_CALL_REFERENCE_TIME",iGet("I_CALL_REFERENCE_TIME"));
		o_add_field("I_SPEECH_VERSION",iGet("I_SPEECH_VERSION"));
		o_add_field("I_CAMEL_CALL_REFERENCE",iGet("I_CAMEL_CALL_REFERENCE"));
		o_add_field("I_CAMEL_EXCHANGE_ID",iGet("I_CAMEL_EXCHANGE_ID"));
		o_add_field("I_CAMEL_EXCHANGE_ID_TON",iGet("I_CAMEL_EXCHANGE_ID_TON"));
		o_add_field("I_NUMBER_OF_ALL_IN_RECORDS",iGet("I_NUMBER_OF_ALL_IN_RECORDS"));
		o_add_field("I_NUMBER_OF_IN_RECORDS",iGet("I_NUMBER_OF_IN_RECORDS"));
		o_add_field("I_SCP_CONNECTION",iGet("I_SCP_CONNECTION"));
		o_add_field("I_TERM_MCZ_CHANGE_PERCENT",iGet("I_TERM_MCZ_CHANGE_PERCENT"));
		o_add_field("I_TERM_MCZ_CHANGE_DIRECTION",iGet("I_TERM_MCZ_CHANGE_DIRECTION"));
		o_add_field("I_LOC_ROUTING_NUMBER",iGet("I_LOC_ROUTING_NUMBER"));
		o_add_field("I_LOC_ROUTING_NUMBER_TON",iGet("I_LOC_ROUTING_NUMBER_TON"));
		o_add_field("I_CDB_INDICATOR",iGet("I_CDB_INDICATOR"));
		o_add_field("I_CUG_INFORMATION",iGet("I_CUG_INFORMATION"));
		o_add_field("I_CUG_INTERLOCK",iGet("I_CUG_INTERLOCK"));
		o_add_field("I_CUG_OUTGOING_ACCESS",iGet("I_CUG_OUTGOING_ACCESS"));
		o_add_field("I_CALLED_SUBS_FIRST_MCC",iGet("I_CALLED_SUBS_FIRST_MCC"));
		o_add_field("I_CALLED_SUBS_FIRST_MNC",iGet("I_CALLED_SUBS_FIRST_MNC"));
		o_add_field("I_CALLED_SUBS_LAST_EX_ID_TON",iGet("I_CALLED_SUBS_LAST_EX_ID_TON"));
		o_add_field("I_CALLED_SUBS_LAST_MCC",iGet("I_CALLED_SUBS_LAST_MCC"));
		o_add_field("I_CALLED_SUBS_LAST_MNC",iGet("I_CALLED_SUBS_LAST_MNC"));
		o_add_field("I_CALLED_SUBS_SECOND_MCC",iGet("I_CALLED_SUBS_SECOND_MCC"));
		o_add_field("I_CALLED_SUBS_SECOND_MNC",iGet("I_CALLED_SUBS_SECOND_MNC"));
		o_add_field("I_RADIO_NETWORK_TYPE",iGet("I_RADIO_NETWORK_TYPE"));
		o_add_field("I_REQ_AIR_INTERFACE_USER_RATE",iGet("I_REQ_AIR_INTERFACE_USER_RATE"));
		o_add_field("I_REQ_FIXED_NW_USER_RATE",iGet("I_REQ_FIXED_NW_USER_RATE"));
		o_add_field("I_USED_AIR_INTERFACE_USER_RATE",iGet("I_USED_AIR_INTERFACE_USER_RATE"));
		o_add_field("I_USED_FIXED_NW_USER_RATE",iGet("I_USED_FIXED_NW_USER_RATE"));
		o_add_field("I_SELECTED_CODEC",iGet("I_SELECTED_CODEC"));
		o_add_field("I_PNI",iGet("I_PNI"));
		o_add_field("I_IN_BNC_CONNECTION_TYPE",iGet("I_IN_BNC_CONNECTION_TYPE"));
		o_add_field("I_INSIDE_CONTROL_PLANE_INDEX",iGet("I_INSIDE_CONTROL_PLANE_INDEX"));
		o_add_field("I_INSIDE_USER_PLANE_INDEX",iGet("I_INSIDE_USER_PLANE_INDEX"));
		o_add_field("I_GLOBAL_CALL_REFERENCE",iGet("I_GLOBAL_CALL_REFERENCE"));
		o_add_field("I_CALLING_PSTN_CATEGORY",iGet("I_CALLING_PSTN_CATEGORY"));
		o_add_field("I_OPTIMAL_ROUTING_INDICATOR",iGet("I_OPTIMAL_ROUTING_INDICATOR"));
		o_add_field("I_HOT_BILLING_RECORD_NUMBER",iGet("I_HOT_BILLING_RECORD_NUMBER"));
		o_add_field("I_CALLED_IMEISV",iGet("I_CALLED_IMEISV"));
		o_add_field("I_VIRTUAL_MSC_ID",iGet("I_VIRTUAL_MSC_ID"));
		o_add_field("I_VIRTUAL_MSC_ID_TON",iGet("I_VIRTUAL_MSC_ID_TON"));
		o_add_field("I_NBR_OF_TERM_CAP_IN_RECS",iGet("I_NBR_OF_TERM_CAP_IN_RECS"));
		o_add_field("I_NUMBER_OF_IN_ANNOUNCEMENTS",iGet("I_NUMBER_OF_IN_ANNOUNCEMENTS"));
		o_add_field("I_ADD_ROUTING_CATEGORY",iGet("I_ADD_ROUTING_CATEGORY"));
		o_add_field("I_ROUTING_CATEGORY",iGet("I_ROUTING_CATEGORY"));
		o_add_field("I_ACCEPTABLE_CHANNEL_CODINGS",iGet("I_ACCEPTABLE_CHANNEL_CODINGS"));
		o_add_field("I_REQ_NUMBER_OF_CHANNELS",iGet("I_REQ_NUMBER_OF_CHANNELS"));
		o_add_field("I_REQ_OTHER_MODEM_TYPE",iGet("I_REQ_OTHER_MODEM_TYPE"));
		o_add_field("I_REQ_USER_INITIATED_MOD_IND",iGet("I_REQ_USER_INITIATED_MOD_IND"));
		o_add_field("I_USED_CHANNEL_CODING",iGet("I_USED_CHANNEL_CODING"));
		o_add_field("I_USED_NUMBER_OF_CHANNELS",iGet("I_USED_NUMBER_OF_CHANNELS"));
		o_add_field("I_USED_OTHER_MODEM_TYPE",iGet("I_USED_OTHER_MODEM_TYPE"));
		o_add_field("I_CALLED_CELL_BAND",iGet("I_CALLED_CELL_BAND"));
		o_add_field("I_MS_CLASSMARK3",iGet("I_MS_CLASSMARK3"));
		o_add_field("I_TERM_MCZ_DURATION_TEN_MS",iGet("I_TERM_MCZ_DURATION_TEN_MS"));
		o_add_field("I_DIALLED_DIGITS_TON",iGet("I_DIALLED_DIGITS_TON"));
		o_add_field("I_DIALLED_DIGITS",iGet("I_DIALLED_DIGITS"));
		o_add_field("I_PIC",iGet("I_PIC"));
		o_add_field("I_IN_CIRCUIT_GROUP_NAME",EL_IN_CIRCUIT_GROUP_NAME);
		o_add_field("I_OUT_CIRCUIT",iGet("I_OUT_CIRCUIT"));
		o_add_field("I_OUT_CIRCUIT_GROUP",iGet("I_OUT_CIRCUIT_GROUP"));
		o_add_field("I_OUT_CIRCUIT_GROUP_NAME",EL_OUT_CIRCUIT_GROUP_NAME);
		o_add_field("I_CT_THIRD_PARTY_NUMBER_",iGet("I_CT_THIRD_PARTY_NUMBER_"));
		o_add_field("I_CT_THIRD_PARTY_NUMBER",iGet("I_CT_THIRD_PARTY_NUMBER"));
		o_add_field("I_EMLPP_LEVEL",iGet("I_EMLPP_LEVEL"));
	nb_write_record("DWH");	
	
	DIAG(DIAG_LOW, "write_MTC(): Leaving the function");
}
void write_CF()
{		
	DIAG(DIAG_LOW, "write_CF(): Entered the function");
	
	nb_new_record();
	audit_out_type("cf");
	o_add_field("I_RECORD_LENGTH",iGet("I_RECORD_LENGTH"));
	o_add_field("I_RECORD_TYPE",iGet("I_RECORD_TYPE"));
	o_add_field("I_RECORD_NUMBER",iGet("I_RECORD_NUMBER"));
	o_add_field("I_RECORD_STATUS",iGet("I_RECORD_STATUS"));
	o_add_field("I_CHECK_SUM",iGet("I_CHECK_SUM"));
	o_add_field("I_CALL_REFERENCE",iGet("I_CALL_REFERENCE"));
	o_add_field("I_EXCHANGE_ID",iGet("I_EXCHANGE_ID"));
	o_add_field("I_INTERMEDIATE_RECORD_NUMBER",iGet("I_INTERMEDIATE_RECORD_NUMBER"));
	o_add_field("I_INTERMEDIATE_CHARGING_IND",iGet("I_INTERMEDIATE_CHARGING_IND"));
	o_add_field("I_NUMBER_OF_SS_RECORDS",iGet("I_NUMBER_OF_SS_RECORDS"));
	o_add_field("I_CAUSE_FOR_FORWARDING",iGet("I_CAUSE_FOR_FORWARDING"));
	o_add_field("I_FORWARDING_IMSI",iGet("I_FORWARDING_IMSI"));
	o_add_field("I_FORWARDING_NUMBER",iGet("I_FORWARDING_NUMBER"));
	o_add_field("I_FORWARDED_TO_IMSI",iGet("I_FORWARDED_TO_IMSI"));
	o_add_field("I_FORWARDED_TO_IMEI",iGet("I_FORWARDED_TO_IMEI"));
	o_add_field("I_FORWARDED_TO_NUMBER",iGet("I_FORWARDED_TO_NUMBER"));
	o_add_field("I_FORWARDED_TO_MS_CLASSMARK",iGet("I_FORWARDED_TO_MS_CLASSMARK"));
	o_add_field("I_ORIG_CALLING_NUMBER",iGet("I_ORIG_CALLING_NUMBER"));
	o_add_field("I_IN_CIRCUIT_GROUP",iGet("I_IN_CIRCUIT_GROUP"));
	o_add_field("I_IN_CIRCUIT",iGet("I_IN_CIRCUIT"));
	o_add_field("I_OUT_CIRCUIT_GROUP",iGet("I_OUT_CIRCUIT_GROUP"));
	o_add_field("I_OUT_CIRCUIT",iGet("I_OUT_CIRCUIT"));
	o_add_field("I_BASIC_SERVICE_TYPE",iGet("I_BASIC_SERVICE_TYPE"));
	o_add_field("I_BASIC_SERVICE_CODE",iGet("I_BASIC_SERVICE_CODE"));
	o_add_field("I_FACILITY_USAGE",iGet("I_FACILITY_USAGE"));
	o_add_field("I_NON_TRANSPARENCY_INDICATOR",iGet("I_NON_TRANSPARENCY_INDICATOR"));
	o_add_field("I_CHANNEL_RATE_INDICATOR",iGet("I_CHANNEL_RATE_INDICATOR"));
	o_add_field("I_IN_CHANNEL_ALLOCATED_TIME",iGet("I_IN_CHANNEL_ALLOCATED_TIME"));
	o_add_field("I_CHARGING_START_TIME",iGet("I_CHARGING_START_TIME"));
	o_add_field("I_CHARGING_END_TIME",iGet("I_CHARGING_END_TIME"));
	o_add_field("I_CAUSE_FOR_TERMINATION",iGet("I_CAUSE_FOR_TERMINATION"));
	o_add_field("I_CALL_TYPE",iGet("I_CALL_TYPE"));
	o_add_field("I_FORWARDED_TO_NUMBER_TON",iGet("I_FORWARDED_TO_NUMBER_TON"));
	o_add_field("I_FORW_MCZ_CHRG_TYPE",iGet("I_FORW_MCZ_CHRG_TYPE"));
	o_add_field("I_FORW_MCZ_DURATION",iGet("I_FORW_MCZ_DURATION"));
	o_add_field("I_FORW_MCZ_TARIFF_CLASS",iGet("I_FORW_MCZ_TARIFF_CLASS"));
	o_add_field("I_FORW_MCZ_PULSES",iGet("I_FORW_MCZ_PULSES"));
	o_add_field("I_FORWARDED_TO_MSRN_TON",iGet("I_FORWARDED_TO_MSRN_TON"));
	o_add_field("I_FORWARDED_TO_MSRN",iGet("I_FORWARDED_TO_MSRN"));
	o_add_field("I_FORWARDING_NUMBER_TON",iGet("I_FORWARDING_NUMBER_TON"));
	o_add_field("I_ORIG_CALLING_NUMBER_TON",iGet("I_ORIG_CALLING_NUMBER_TON"));
	o_add_field("I_INTERMEDIATE_CHRG_CAUSE",iGet("I_INTERMEDIATE_CHRG_CAUSE"));
	o_add_field("I_ORIG_DIALLING_CLASS",iGet("I_ORIG_DIALLING_CLASS"));
	o_add_field("I_LEG_CALL_REFERENCE",iGet("I_LEG_CALL_REFERENCE"));
	o_add_field("I_CALL_REFERENCE_TIME",iGet("I_CALL_REFERENCE_TIME"));
	o_add_field("I_SPEECH_VERSION",iGet("I_SPEECH_VERSION"));
	o_add_field("I_CAMEL_CALL_REFERENCE",iGet("I_CAMEL_CALL_REFERENCE"));
	o_add_field("I_CAMEL_EXCHANGE_ID",iGet("I_CAMEL_EXCHANGE_ID"));
	o_add_field("I_CAMEL_EXCHANGE_ID_TON",iGet("I_CAMEL_EXCHANGE_ID_TON"));
	o_add_field("I_NUMBER_OF_ALL_IN_RECORDS",iGet("I_NUMBER_OF_ALL_IN_RECORDS"));
	o_add_field("I_FORW_MCZ_CHANGE_DIRECTION",iGet("I_FORW_MCZ_CHANGE_DIRECTION"));
	o_add_field("I_FORW_MCZ_CHANGE_PERCENT",iGet("I_FORW_MCZ_CHANGE_PERCENT"));
	o_add_field("I_NUMBER_OF_IN_RECORDS",iGet("I_NUMBER_OF_IN_RECORDS"));
	o_add_field("I_SCP_CONNECTION",iGet("I_SCP_CONNECTION"));
	o_add_field("I_LOC_ROUTING_NUMBER",iGet("I_LOC_ROUTING_NUMBER"));
	o_add_field("I_LOC_ROUTING_NUMBER_TON",iGet("I_LOC_ROUTING_NUMBER_TON"));
	o_add_field("I_NPDB_QUERY_STATUS",iGet("I_NPDB_QUERY_STATUS"));
	o_add_field("I_CUG_INFORMATION",iGet("I_CUG_INFORMATION"));
	o_add_field("I_CUG_INTERLOCK",iGet("I_CUG_INTERLOCK"));
	o_add_field("I_CUG_OUTGOING_ACCESS",iGet("I_CUG_OUTGOING_ACCESS"));
	o_add_field("I_FORWARDED_TO_LAST_EX_ID_TON",iGet("I_FORWARDED_TO_LAST_EX_ID_TON"));
	o_add_field("I_FORWARDED_TO_LAST_MCC",iGet("I_FORWARDED_TO_LAST_MCC"));
	o_add_field("I_FORWARDED_TO_LAST_MNC",iGet("I_FORWARDED_TO_LAST_MNC"));
	o_add_field("I_FORWARDING_FIRST_MCC",iGet("I_FORWARDING_FIRST_MCC"));
	o_add_field("I_FORWARDING_FIRST_MNC",iGet("I_FORWARDING_FIRST_MNC"));
	o_add_field("I_FORWARDING_LAST_EX_ID_TON",iGet("I_FORWARDING_LAST_EX_ID_TON"));
	o_add_field("I_FORWARDING_LAST_MCC",iGet("I_FORWARDING_LAST_MCC"));
	o_add_field("I_FORWARDING_LAST_MNC",iGet("I_FORWARDING_LAST_MNC"));
	o_add_field("I_RADIO_NETWORK_TYPE",iGet("I_RADIO_NETWORK_TYPE"));
	o_add_field("I_PNI",iGet("I_PNI"));
	o_add_field("I_IN_BNC_CONNECTION_TYPE",iGet("I_IN_BNC_CONNECTION_TYPE"));
	o_add_field("I_INSIDE_CONTROL_PLANE_INDEX",iGet("I_INSIDE_CONTROL_PLANE_INDEX"));
	o_add_field("I_INSIDE_USER_PLANE_INDEX",iGet("I_INSIDE_USER_PLANE_INDEX"));
	o_add_field("I_OUT_BNC_CONNECTION_TYPE",iGet("I_OUT_BNC_CONNECTION_TYPE"));
	o_add_field("I_OUTSIDE_CONTROL_PLANE_INDEX",iGet("I_OUTSIDE_CONTROL_PLANE_INDEX"));
	o_add_field("I_OUTSIDE_USER_PLANE_INDEX",iGet("I_OUTSIDE_USER_PLANE_INDEX"));
	o_add_field("I_GLOBAL_CALL_REFERENCE",iGet("I_GLOBAL_CALL_REFERENCE"));
	o_add_field("I_HOT_BILLING_RECORD_NUMBER",iGet("I_HOT_BILLING_RECORD_NUMBER"));
	o_add_field("I_FORWARDED_TO_IMEISV",iGet("I_FORWARDED_TO_IMEISV"));
	o_add_field("I_FORWARDING_IMEISV",iGet("I_FORWARDING_IMEISV"));
	o_add_field("I_VIRTUAL_MSC_ID",iGet("I_VIRTUAL_MSC_ID"));
	o_add_field("I_VIRTUAL_MSC_ID_TON",iGet("I_VIRTUAL_MSC_ID_TON"));
	o_add_field("I_NBR_OF_ORIG_CAP_IN_RECS",iGet("I_NBR_OF_ORIG_CAP_IN_RECS"));
	o_add_field("I_NBR_OF_TERM_CAP_IN_RECS",iGet("I_NBR_OF_TERM_CAP_IN_RECS"));
	o_add_field("I_NUMBER_OF_IN_ANNOUNCEMENTS",iGet("I_NUMBER_OF_IN_ANNOUNCEMENTS"));
	o_add_field("I_CONNECTED_TO_NUMBER",iGet("I_CONNECTED_TO_NUMBER"));
	o_add_field("I_CONNECTED_TO_NUMBER_TON",iGet("I_CONNECTED_TO_NUMBER_TON"));
	o_add_field("I_ORIG_CALLED_NUMBER",iGet("I_ORIG_CALLED_NUMBER"));
	o_add_field("I_ORIG_CALLED_NUMBER_TON",iGet("I_ORIG_CALLED_NUMBER_TON"));
	o_add_field("I_ADD_ROUTING_CATEGORY",iGet("I_ADD_ROUTING_CATEGORY"));
	o_add_field("I_ROUTING_CATEGORY",iGet("I_ROUTING_CATEGORY"));
	o_add_field("I_FORWARDING_MSRN",iGet("I_FORWARDING_MSRN"));
	o_add_field("I_FORWARDING_MSRN_TON",iGet("I_FORWARDING_MSRN_TON"));
	o_add_field("I_OPTIMAL_ROUTING_INDICATOR",iGet("I_OPTIMAL_ROUTING_INDICATOR"));
	o_add_field("I_FORWARDING_CELL_BAND",iGet("I_FORWARDING_CELL_BAND"));
	o_add_field("I_MS_CLASSMARK3",iGet("I_MS_CLASSMARK3"));
	o_add_field("I_FORW_MCZ_DURATION_TEN_MS",iGet("I_FORW_MCZ_DURATION_TEN_MS"));
	o_add_field("I_FORWARDING_IMEI",iGet("I_FORWARDING_IMEI"));
	o_add_field("I_DIALLED_DIGITS_TON",iGet("I_DIALLED_DIGITS_TON"));
	o_add_field("I_DIALLED_DIGITS",iGet("I_DIALLED_DIGITS"));
	o_add_field("I_IN_CIRCUIT_GROUP_NAME",EL_IN_CIRCUIT_GROUP_NAME);
	o_add_field("I_OUT_CIRCUIT_GROUP_NAME",EL_OUT_CIRCUIT_GROUP_NAME);
	o_add_field("I_EMLPP_LEVEL",iGet("I_EMLPP_LEVEL"));
	o_add_field("I_DIRECTORY_NUMBER_TON",iGet("I_DIRECTORY_NUMBER_TON"));
	o_add_field("I_DIRECTORY_NUMBER",iGet("I_DIRECTORY_NUMBER"));
	o_add_field("I_SERVED_NUMBER",iGet("I_SERVED_NUMBER"));
	o_add_field("I_SERVED_NUMBER_TON",iGet("I_SERVED_NUMBER_TON"));
	nb_write_record("DWH");	
	
	DIAG(DIAG_LOW, "write_CF(): Leaving the function");
}
void write_SMMO()
{		
	DIAG(DIAG_LOW, "write_SMMO(): Entered the function");
	
	nb_new_record();
	audit_out_type("smmo");
	
	  o_add_field("I_RECORD_LENGTH",iGet("I_RECORD_LENGTH"));
		o_add_field("I_RECORD_TYPE",iGet("I_RECORD_TYPE"));
		o_add_field("I_RECORD_NUMBER",iGet("I_RECORD_NUMBER"));
		o_add_field("I_RECORD_STATUS",iGet("I_RECORD_STATUS"));
		o_add_field("I_CHECK_SUM",iGet("I_CHECK_SUM"));
		o_add_field("I_CALL_REFERENCE",iGet("I_CALL_REFERENCE"));
		o_add_field("I_EXCHANGE_ID",iGet("I_EXCHANGE_ID"));
		o_add_field("I_CALLING_IMSI",iGet("I_CALLING_IMSI"));
		o_add_field("I_CALLING_IMEI",iGet("I_CALLING_IMEI"));
		o_add_field("I_CALLING_NUMBER",iGet("I_CALLING_NUMBER"));
		o_add_field("I_CALLING_CATEGORY",iGet("I_CALLING_CATEGORY"));
		o_add_field("I_CALLING_MS_CLASSMARK",iGet("I_CALLING_MS_CLASSMARK"));
		o_add_field("I_SMS_CENTRE",iGet("I_SMS_CENTRE"));
		o_add_field("I_SERVED_SUBS_LAC",EL_FIRST_LAC);
		o_add_field("I_SERVED_SUBS_CI",iGet("I_SERVED_SUBS_CI"));
		o_add_field("I_INCOMING_TIME",iGet("I_INCOMING_TIME"));
		o_add_field("I_CAUSE_FOR_TERMINATION",iGet("I_CAUSE_FOR_TERMINATION"));
		o_add_field("I_MSC_TYPE",iGet("I_MSC_TYPE"));
		o_add_field("I_CALLED_NUMBER",iGet("I_CALLED_NUMBER"));
		o_add_field("I_CALLING_NUMBER_TON",iGet("I_CALLING_NUMBER_TON"));
		o_add_field("I_CALLED_NUMBER_TON",iGet("I_CALLED_NUMBER_TON"));
		o_add_field("I_CALLING_VMSC_NUMBER",iGet("I_CALLING_VMSC_NUMBER"));
		o_add_field("I_SMS_TYPE",iGet("I_SMS_TYPE"));
		o_add_field("I_DIALLED_DIGITS",iGet("I_DIALLED_DIGITS"));
		o_add_field("I_CALL_REFERENCE_TIME",iGet("I_CALL_REFERENCE_TIME"));
		o_add_field("I_NUMBER_OF_IN_RECORDS",iGet("I_NUMBER_OF_IN_RECORDS"));
		o_add_field("I_ADD_ROUTING_CATEGORY",iGet("I_ADD_ROUTING_CATEGORY"));
		o_add_field("I_APPLICATION_INFO",iGet("I_APPLICATION_INFO"));
		o_add_field("I_CONCATENATED_RECORD_NUMBER",iGet("I_CONCATENATED_RECORD_NUMBER"));
		o_add_field("I_CONCATENATED_SMS_REFERENCE",iGet("I_CONCATENATED_SMS_REFERENCE"));
		o_add_field("I_NUM_OF_CONCATENATED_SMS",iGet("I_NUM_OF_CONCATENATED_SMS"));
		o_add_field("I_ROUTING_CATEGORY",iGet("I_ROUTING_CATEGORY"));
		o_add_field("I_CAMEL_CALL_REFERENCE",iGet("I_CAMEL_CALL_REFERENCE"));
		o_add_field("I_CAMEL_EXCHANGE_ID",iGet("I_CAMEL_EXCHANGE_ID"));
		o_add_field("I_CAMEL_EXCHANGE_ID_TON",iGet("I_CAMEL_EXCHANGE_ID_TON"));
		o_add_field("I_CAMEL_SMS_MODIFICATION",iGet("I_CAMEL_SMS_MODIFICATION"));
		o_add_field("I_DEFAULT_SMS_HANDLING",iGet("I_DEFAULT_SMS_HANDLING"));
		o_add_field("I_CAMEL_SERVICE_KEY",iGet("I_CAMEL_SERVICE_KEY"));
		o_add_field("I_SCF_ADDRESS",iGet("I_SCF_ADDRESS"));
		o_add_field("I_DIALLED_DIGITS_TON",iGet("I_DIALLED_DIGITS_TON"));
		o_add_field("I_RADIO_NETWORK_TYPE",iGet("I_RADIO_NETWORK_TYPE"));
		o_add_field("I_SERVED_SUBS_MCC",iGet("I_SERVED_SUBS_MCC"));
		o_add_field("I_SERVED_SUBS_MNC",iGet("I_SERVED_SUBS_MNC"));
		o_add_field("I_HOT_BILLING_RECORD_NUMBER",iGet("I_HOT_BILLING_RECORD_NUMBER"));
		o_add_field("I_CALLED_IMSI",iGet("I_CALLED_IMSI"));
		o_add_field("I_CALLING_IMEISV",iGet("I_CALLING_IMEISV"));
		o_add_field("I_UE_TIME_ZONE",iGet("I_UE_TIME_ZONE"));
		o_add_field("I_PNI",iGet("I_PNI"));
		o_add_field("I_COMMAND_TYPE",iGet("I_COMMAND_TYPE"));
		o_add_field("I_MESSAGE_REFERENCE",iGet("I_MESSAGE_REFERENCE"));
		o_add_field("I_SMS_LENGTH",iGet("I_SMS_LENGTH"));
		o_add_field("I_TARIFF_CLASS",iGet("I_TARIFF_CLASS"));
	nb_write_record("DWH");	
	
	DIAG(DIAG_LOW, "write_SMMO(): Leaving the function");
}
void write_SMMT()
{		
	DIAG(DIAG_LOW, "write_SMMT(): Entered the function");
	
	nb_new_record();
	audit_out_type("smmt");
	o_add_field("I_RECORD_LENGTH",iGet("I_RECORD_LENGTH"));
	o_add_field("I_RECORD_TYPE",iGet("I_RECORD_TYPE"));
	o_add_field("I_RECORD_NUMBER",iGet("I_RECORD_NUMBER"));
	o_add_field("I_RECORD_STATUS",iGet("I_RECORD_STATUS"));
	o_add_field("I_CHECK_SUM",iGet("I_CHECK_SUM"));
	o_add_field("I_CALL_REFERENCE",iGet("I_CALL_REFERENCE"));
	o_add_field("I_EXCHANGE_ID",iGet("I_EXCHANGE_ID"));
	o_add_field("I_CALLED_IMSI",iGet("I_CALLED_IMSI"));
	o_add_field("I_CALLED_IMEI",iGet("I_CALLED_IMEI"));
	o_add_field("I_CALLED_NUMBER",iGet("I_CALLED_NUMBER"));
	o_add_field("I_CALLED_CATEGORY",iGet("I_CALLED_CATEGORY"));
	o_add_field("I_CALLED_MS_CLASSMARK",iGet("I_CALLED_MS_CLASSMARK"));
	o_add_field("I_SERVED_SUBS_LAC",EL_FIRST_LAC);
	o_add_field("I_SERVED_SUBS_CI",iGet("I_SERVED_SUBS_CI"));
	o_add_field("I_SMS_CENTRE",iGet("I_SMS_CENTRE"));
	o_add_field("I_INCOMING_TIME",iGet("I_INCOMING_TIME"));
	o_add_field("I_CAUSE_FOR_TERMINATION",iGet("I_CAUSE_FOR_TERMINATION"));
	o_add_field("I_MSC_TYPE",iGet("I_MSC_TYPE"));
	o_add_field("I_CALLING_NUMBER",EL_SMMT_CALLING_NUMBER);
	o_add_field("I_CALLED_VMSC_NUMBER",iGet("I_CALLED_VMSC_NUMBER"));
	o_add_field("I_CALLING_NUMBER_TON",iGet("I_CALLING_NUMBER_TON"));
	o_add_field("I_CALLED_NUMBER_TON",iGet("I_CALLED_NUMBER_TON"));
	o_add_field("I_SMS_TYPE",iGet("I_SMS_TYPE"));
	o_add_field("I_CALL_REFERENCE_TIME",iGet("I_CALL_REFERENCE_TIME"));
	o_add_field("I_NUMBER_OF_IN_RECORDS",iGet("I_NUMBER_OF_IN_RECORDS"));
	o_add_field("I_ADD_ROUTING_CATEGORY",iGet("I_ADD_ROUTING_CATEGORY"));
	o_add_field("I_APPLICATION_INFO",iGet("I_APPLICATION_INFO"));
	o_add_field("I_CONCATENATED_RECORD_NUMBER",iGet("I_CONCATENATED_RECORD_NUMBER"));
	o_add_field("I_CONCATENATED_SMS_REFERENCE",iGet("I_CONCATENATED_SMS_REFERENCE"));
	o_add_field("I_NUM_OF_CONCATENATED_SMS",iGet("I_NUM_OF_CONCATENATED_SMS"));
	o_add_field("I_ROUTING_CATEGORY",iGet("I_ROUTING_CATEGORY"));
	o_add_field("I_CAMEL_SERVICE_KEY",iGet("I_CAMEL_SERVICE_KEY"));
	o_add_field("I_SCF_ADDRESS",iGet("I_SCF_ADDRESS"));
	o_add_field("I_RADIO_NETWORK_TYPE",iGet("I_RADIO_NETWORK_TYPE"));
	o_add_field("I_SERVED_SUBS_MCC",iGet("I_SERVED_SUBS_MCC"));
	o_add_field("I_SERVED_SUBS_MNC",iGet("I_SERVED_SUBS_MNC"));
	o_add_field("I_CAMEL_CALL_REFERENCE",iGet("I_CAMEL_CALL_REFERENCE"));
	o_add_field("I_CAMEL_EXCHANGE_ID",iGet("I_CAMEL_EXCHANGE_ID"));
	o_add_field("I_CAMEL_EXCHANGE_ID_TON",iGet("I_CAMEL_EXCHANGE_ID_TON"));
	o_add_field("I_CAMEL_SMS_MODIFICATION",iGet("I_CAMEL_SMS_MODIFICATION"));
	o_add_field("I_DEFAULT_SMS_HANDLING",iGet("I_DEFAULT_SMS_HANDLING"));
	o_add_field("I_HOT_BILLING_RECORD_NUMBER",iGet("I_HOT_BILLING_RECORD_NUMBER"));
	o_add_field("I_CALLED_IMEISV",iGet("I_CALLED_IMEISV"));
	o_add_field("I_UE_TIME_ZONE",iGet("I_UE_TIME_ZONE"));
	o_add_field("I_CALLING_VMSC_NUMBER",iGet("I_CALLING_VMSC_NUMBER"));
	o_add_field("I_SMS_LENGTH",iGet("I_SMS_LENGTH"));
	o_add_field("I_TARIFF_CLASS",iGet("I_TARIFF_CLASS"));
	nb_write_record("DWH");	
	
	DIAG(DIAG_LOW, "write_SMMT(): Leaving the function");
}
void write_POC()
{		
	DIAG(DIAG_LOW, "write_POC(): Entered the function");
	
	nb_new_record();
	audit_out_type("poc");
	o_add_field("I_RECORD_LENGTH",iGet("I_RECORD_LENGTH"));
	o_add_field("I_RECORD_TYPE",iGet("I_RECORD_TYPE"));
	o_add_field("I_RECORD_NUMBER",iGet("I_RECORD_NUMBER"));
	o_add_field("I_RECORD_STATUS",iGet("I_RECORD_STATUS"));
	o_add_field("I_CHECK_SUM",iGet("I_CHECK_SUM"));
	o_add_field("I_CALL_REFERENCE",iGet("I_CALL_REFERENCE"));
	o_add_field("I_EXCHANGE_ID",iGet("I_EXCHANGE_ID"));
	o_add_field("I_INTERMEDIATE_RECORD_NUMBER",iGet("I_INTERMEDIATE_RECORD_NUMBER"));
	o_add_field("I_INTERMEDIATE_CHARGING_IND",iGet("I_INTERMEDIATE_CHARGING_IND"));
	o_add_field("I_NUMBER_OF_SS_RECORDS",iGet("I_NUMBER_OF_SS_RECORDS"));
	o_add_field("I_CALLING_NUMBER_TON",iGet("I_CALLING_NUMBER_TON"));
	o_add_field("I_CALLING_NUMBER",iGet("I_CALLING_NUMBER"));
	o_add_field("I_CALLED_NUMBER_TON",iGet("I_CALLED_NUMBER_TON"));
	o_add_field("I_CALLED_NUMBER",iGet("I_CALLED_NUMBER"));
	o_add_field("I_IN_CIRCUIT_GROUP",iGet("I_IN_CIRCUIT_GROUP"));
	o_add_field("I_IN_CIRCUIT",iGet("I_IN_CIRCUIT"));
	o_add_field("I_IN_CHANNEL_ALLOCATED_TIME",iGet("I_IN_CHANNEL_ALLOCATED_TIME"));
	o_add_field("I_CHARGING_START_TIME",iGet("I_CHARGING_START_TIME"));
	o_add_field("I_CHARGING_END_TIME",iGet("I_CHARGING_END_TIME"));
	o_add_field("I_CAUSE_FOR_TERMINATION",iGet("I_CAUSE_FOR_TERMINATION"));
	o_add_field("I_CALL_TYPE",iGet("I_CALL_TYPE"));
	o_add_field("I_TICKET_TYPE",iGet("I_TICKET_TYPE"));
	o_add_field("I_IAZ_CHRG_TYPE",iGet("I_IAZ_CHRG_TYPE"));
	o_add_field("I_IAZ_DURATION",iGet("I_IAZ_DURATION"));
	o_add_field("I_IAZ_TARIFF_CLASS",iGet("I_IAZ_TARIFF_CLASS"));
	o_add_field("I_IAZ_PULSES",iGet("I_IAZ_PULSES"));
	o_add_field("I_CALLED_MSRN_TON",iGet("I_CALLED_MSRN_TON"));
	o_add_field("I_CALLED_MSRN",iGet("I_CALLED_MSRN"));
	o_add_field("I_INTERMEDIATE_CHRG_CAUSE",iGet("I_INTERMEDIATE_CHRG_CAUSE"));
	o_add_field("I_ORIG_DIALLING_CLASS",iGet("I_ORIG_DIALLING_CLASS"));
	o_add_field("I_LEG_CALL_REFERENCE",iGet("I_LEG_CALL_REFERENCE"));
	o_add_field("I_BASIC_SERVICE_TYPE",iGet("I_BASIC_SERVICE_TYPE"));
	o_add_field("I_BASIC_SERVICE_CODE",iGet("I_BASIC_SERVICE_CODE"));
	o_add_field("I_CALL_REFERENCE_TIME",iGet("I_CALL_REFERENCE_TIME"));
	o_add_field("I_CAMEL_CALL_REFERENCE",iGet("I_CAMEL_CALL_REFERENCE"));
	o_add_field("I_CAMEL_EXCHANGE_ID",iGet("I_CAMEL_EXCHANGE_ID"));
	o_add_field("I_CAMEL_EXCHANGE_ID_TON",iGet("I_CAMEL_EXCHANGE_ID_TON"));
	o_add_field("I_NUMBER_OF_ALL_IN_RECORDS",iGet("I_NUMBER_OF_ALL_IN_RECORDS"));
	o_add_field("I_IAZ_CHANGE_DIRECTION",iGet("I_IAZ_CHANGE_DIRECTION"));
	o_add_field("I_IAZ_CHANGE_PERCENT",iGet("I_IAZ_CHANGE_PERCENT"));
	o_add_field("I_NUMBER_OF_IN_RECORDS",iGet("I_NUMBER_OF_IN_RECORDS"));
	o_add_field("I_ORIG_MCZ_CHANGE_DIRECTION",iGet("I_ORIG_MCZ_CHANGE_DIRECTION"));
	o_add_field("I_ORIG_MCZ_CHANGE_PERCENT",iGet("I_ORIG_MCZ_CHANGE_PERCENT"));
	o_add_field("I_SCP_CONNECTION",iGet("I_SCP_CONNECTION"));
	o_add_field("I_LOC_ROUTING_NUMBER",iGet("I_LOC_ROUTING_NUMBER"));
	o_add_field("I_LOC_ROUTING_NUMBER_TON",iGet("I_LOC_ROUTING_NUMBER_TON"));
	o_add_field("I_NPDB_QUERY_STATUS",iGet("I_NPDB_QUERY_STATUS"));
	o_add_field("I_CUG_INFORMATION",iGet("I_CUG_INFORMATION"));
	o_add_field("I_CUG_INTERLOCK",iGet("I_CUG_INTERLOCK"));
	o_add_field("I_CUG_OUTGOING_ACCESS",iGet("I_CUG_OUTGOING_ACCESS"));
	o_add_field("I_SELECTED_CODEC",iGet("I_SELECTED_CODEC"));
	o_add_field("I_IN_BNC_CONNECTION_TYPE",iGet("I_IN_BNC_CONNECTION_TYPE"));
	o_add_field("I_INSIDE_CONTROL_PLANE_INDEX",iGet("I_INSIDE_CONTROL_PLANE_INDEX"));
	o_add_field("I_INSIDE_USER_PLANE_INDEX",iGet("I_INSIDE_USER_PLANE_INDEX"));
	o_add_field("I_GLOBAL_CALL_REFERENCE",iGet("I_GLOBAL_CALL_REFERENCE"));
	o_add_field("I_NBR_OF_ORIG_CAP_IN_RECS",iGet("I_NBR_OF_ORIG_CAP_IN_RECS"));
	o_add_field("I_NUMBER_OF_IN_ANNOUNCEMENTS",iGet("I_NUMBER_OF_IN_ANNOUNCEMENTS"));
	o_add_field("I_REDIRECTED_INDICATOR",iGet("I_REDIRECTED_INDICATOR"));
	o_add_field("I_CALLED_IMSI",iGet("I_CALLED_IMSI"));
	o_add_field("I_ORIG_MCZ_DURATION",iGet("I_ORIG_MCZ_DURATION"));
	o_add_field("I_ORIG_MCZ_DURATION_TEN_MS",iGet("I_ORIG_MCZ_DURATION_TEN_MS"));
	o_add_field("I_IN_CIRCUIT_GROUP_NAME",EL_IN_CIRCUIT_GROUP_NAME);
	o_add_field("I_OUT_CIRCUIT",iGet("I_OUT_CIRCUIT"));
	o_add_field("I_OUT_CIRCUIT_GROUP",iGet("I_OUT_CIRCUIT_GROUP"));
	o_add_field("I_OUT_CIRCUIT_GROUP_NAME",EL_OUT_CIRCUIT_GROUP_NAME);
	o_add_field("I_ICID_LENGTH",iGet("I_ICID_LENGTH"));
	o_add_field("I_ICID_OVERFLOW",iGet("I_ICID_OVERFLOW"));
	o_add_field("I_ICID",iGet("I_ICID")); 
	nb_write_record("DWH");	
	
	DIAG(DIAG_LOW, "write_POC(): Leaving the function");
}
void write_PTC()
{		
	DIAG(DIAG_LOW, "write_PTC(): Entered the function");
	
	nb_new_record();
	audit_out_type("ptc");
	
	   o_add_field("I_RECORD_LENGTH",iGet("I_RECORD_LENGTH"));
		o_add_field("I_RECORD_TYPE",iGet("I_RECORD_TYPE"));
		o_add_field("I_RECORD_NUMBER",iGet("I_RECORD_NUMBER"));
		o_add_field("I_RECORD_STATUS",iGet("I_RECORD_STATUS"));
		o_add_field("I_CHECK_SUM",iGet("I_CHECK_SUM"));
		o_add_field("I_CALL_REFERENCE",iGet("I_CALL_REFERENCE"));
		o_add_field("I_EXCHANGE_ID",iGet("I_EXCHANGE_ID"));
		o_add_field("I_INTERMEDIATE_RECORD_NUMBER",iGet("I_INTERMEDIATE_RECORD_NUMBER"));
		o_add_field("I_INTERMEDIATE_CHARGING_IND",iGet("I_INTERMEDIATE_CHARGING_IND"));
		o_add_field("I_NUMBER_OF_SS_RECORDS",iGet("I_NUMBER_OF_SS_RECORDS"));
		o_add_field("I_CALLING_NUMBER_TON",iGet("I_CALLING_NUMBER_TON"));
		o_add_field("I_CALLING_NUMBER",iGet("I_CALLING_NUMBER"));
		o_add_field("I_CALLED_NUMBER_TON",iGet("I_CALLED_NUMBER_TON"));
		o_add_field("I_CALLED_NUMBER",iGet("I_CALLED_NUMBER"));
		o_add_field("I_OUT_CIRCUIT_GROUP",iGet("I_OUT_CIRCUIT_GROUP"));
		o_add_field("I_OUT_CIRCUIT",iGet("I_OUT_CIRCUIT"));
		o_add_field("I_IN_CHANNEL_ALLOCATED_TIME",iGet("I_IN_CHANNEL_ALLOCATED_TIME"));
		o_add_field("I_CHARGING_START_TIME",iGet("I_CHARGING_START_TIME"));
		o_add_field("I_CHARGING_END_TIME",iGet("I_CHARGING_END_TIME"));
		o_add_field("I_CAUSE_FOR_TERMINATION",iGet("I_CAUSE_FOR_TERMINATION"));
		o_add_field("I_CALL_TYPE",iGet("I_CALL_TYPE"));
		o_add_field("I_TICKET_TYPE",iGet("I_TICKET_TYPE"));
		o_add_field("I_OAZ_CHRG_TYPE",iGet("I_OAZ_CHRG_TYPE"));
		o_add_field("I_OAZ_DURATION",iGet("I_OAZ_DURATION"));
		o_add_field("I_OAZ_TARIFF_CLASS",iGet("I_OAZ_TARIFF_CLASS"));
		o_add_field("I_OAZ_PULSES",iGet("I_OAZ_PULSES"));
		o_add_field("I_CALLED_MSRN_TON",iGet("I_CALLED_MSRN_TON"));
		o_add_field("I_CALLED_MSRN",iGet("I_CALLED_MSRN"));
		o_add_field("I_INTERMEDIATE_CHRG_CAUSE",iGet("I_INTERMEDIATE_CHRG_CAUSE"));
		o_add_field("I_LEG_CALL_REFERENCE",iGet("I_LEG_CALL_REFERENCE"));
		o_add_field("I_OUT_CHANNEL_ALLOCATED_TIME",iGet("I_OUT_CHANNEL_ALLOCATED_TIME"));
		o_add_field("I_BASIC_SERVICE_TYPE",iGet("I_BASIC_SERVICE_TYPE"));
		o_add_field("I_BASIC_SERVICE_CODE",iGet("I_BASIC_SERVICE_CODE"));
		o_add_field("I_CALL_REFERENCE_TIME",iGet("I_CALL_REFERENCE_TIME"));
		o_add_field("I_NUMBER_OF_ALL_IN_RECORDS",iGet("I_NUMBER_OF_ALL_IN_RECORDS"));
		o_add_field("I_NUMBER_OF_IN_RECORDS",iGet("I_NUMBER_OF_IN_RECORDS"));
		o_add_field("I_SCP_CONNECTION",iGet("I_SCP_CONNECTION"));
		o_add_field("I_TERM_MCZ_CHANGE_DIRECTION",iGet("I_TERM_MCZ_CHANGE_DIRECTION"));
		o_add_field("I_TERM_MCZ_CHANGE_PERCENT",iGet("I_TERM_MCZ_CHANGE_PERCENT"));
		o_add_field("I_OAZ_CHANGE_PERCENT",iGet("I_OAZ_CHANGE_PERCENT"));
		o_add_field("I_OAZ_CHANGE_DIRECTION",iGet("I_OAZ_CHANGE_DIRECTION"));
		o_add_field("I_LOC_ROUTING_NUMBER",iGet("I_LOC_ROUTING_NUMBER"));
		o_add_field("I_LOC_ROUTING_NUMBER_TON",iGet("I_LOC_ROUTING_NUMBER_TON"));
		o_add_field("I_NPDB_QUERY_STATUS",iGet("I_NPDB_QUERY_STATUS"));
		o_add_field("I_CUG_INFORMATION",iGet("I_CUG_INFORMATION"));
		o_add_field("I_CUG_INTERLOCK",iGet("I_CUG_INTERLOCK"));
		o_add_field("I_CUG_OUTGOING_ACCESS",iGet("I_CUG_OUTGOING_ACCESS"));
		o_add_field("I_SELECTED_CODEC",iGet("I_SELECTED_CODEC"));
		o_add_field("I_OUT_BNC_CONNECTION_TYPE",iGet("I_OUT_BNC_CONNECTION_TYPE"));
		o_add_field("I_OUTSIDE_CONTROL_PLANE_INDEX",iGet("I_OUTSIDE_CONTROL_PLANE_INDEX"));
		o_add_field("I_OUTSIDE_USER_PLANE_INDEX",iGet("I_OUTSIDE_USER_PLANE_INDEX"));
		o_add_field("I_GLOBAL_CALL_REFERENCE",iGet("I_GLOBAL_CALL_REFERENCE"));
		o_add_field("I_REDIRECTED_INDICATOR",iGet("I_REDIRECTED_INDICATOR"));
		o_add_field("I_TERM_MCZ_DURATION",iGet("I_TERM_MCZ_DURATION"));
		o_add_field("I_TERM_MCZ_DURATION_TEN_MS",iGet("I_TERM_MCZ_DURATION_TEN_MS"));
		o_add_field("I_IN_CIRCUIT",iGet("I_IN_CIRCUIT"));
		o_add_field("I_IN_CIRCUIT_GROUP",iGet("I_IN_CIRCUIT_GROUP"));
		o_add_field("I_IN_CIRCUIT_GROUP_",EL_IN_CIRCUIT_GROUP_NAME);
		o_add_field("I_OUT_CIRCUIT_GROUP_",EL_OUT_CIRCUIT_GROUP_NAME);
		o_add_field("I_ICID_LENGTH",iGet("I_ICID_LENGTH"));
		o_add_field("I_ICID_OVERFLOW",iGet("I_ICID_OVERFLOW"));
		o_add_field("I_ICID",iGet("I_ICID"));             
	nb_write_record("DWH");	
	
	DIAG(DIAG_LOW, "write_PTC(): Leaving the function");
}
void write_COC()
{		
	DIAG(DIAG_LOW, "write_COC(): Entered the function");
	
	nb_new_record();
	audit_out_type("COC");
	o_add_field("I_RECORD_LENGTH",iGet("I_RECORD_LENGTH"));
	o_add_field("I_RECORD_TYPE",iGet("I_RECORD_TYPE"));
	o_add_field("I_RECORD_NUMBER",iGet("I_RECORD_NUMBER"));
	o_add_field("I_RECORD_STATUS",iGet("I_RECORD_STATUS"));
	o_add_field("I_CHECK_SUM",iGet("I_CHECK_SUM"));
	o_add_field("I_CALL_REFERENCE",iGet("I_CALL_REFERENCE"));
	o_add_field("I_EXCHANGE_ID",iGet("I_EXCHANGE_ID"));
	o_add_field("I_BASIC_CALL_STATE_MODEL",iGet("I_BASIC_CALL_STATE_MODEL"));
	o_add_field("I_INTERMEDIATE_RECORD_NUMBER",iGet("I_INTERMEDIATE_RECORD_NUMBER"));
	o_add_field("I_INTERMEDIATE_CHARGING_IND",iGet("I_INTERMEDIATE_CHARGING_IND"));
	o_add_field("I_INTERMEDIATE_CHRG_CAUSE",iGet("I_INTERMEDIATE_CHRG_CAUSE"));
	o_add_field("I_IN_CHANNEL_ALLOCATED_TIME",iGet("I_IN_CHANNEL_ALLOCATED_TIME"));
	o_add_field("I_CHARGING_START_TIME",iGet("I_CHARGING_START_TIME"));
	o_add_field("I_CHARGING_END_TIME",iGet("I_CHARGING_END_TIME"));
	o_add_field("I_DURATION_BEFORE_ANSWER",iGet("I_DURATION_BEFORE_ANSWER"));
	o_add_field("I_CHARGEABLE_DURATION",iGet("I_CHARGEABLE_DURATION"));
	o_add_field("I_LEG_CALL_REFERENCE",iGet("I_LEG_CALL_REFERENCE"));
	o_add_field("I_CAMEL_CALL_REFERENCE",iGet("I_CAMEL_CALL_REFERENCE"));
	o_add_field("I_CAMEL_EXCHANGE_ID_TON",iGet("I_CAMEL_EXCHANGE_ID_TON"));
	o_add_field("I_CAMEL_EXCHANGE_ID",iGet("I_CAMEL_EXCHANGE_ID"));
	o_add_field("I_SCF_ADDRESS_TON",iGet("I_SCF_ADDRESS_TON"));
	o_add_field("I_SCF_ADDRESS",iGet("I_SCF_ADDRESS"));
	o_add_field("I_CAMEL_SERVICE_KEY",iGet("I_CAMEL_SERVICE_KEY"));
	o_add_field("I_DEFAULT_CALL_HANDLING",iGet("I_DEFAULT_CALL_HANDLING"));
	o_add_field("I_DESTINATION_NUMBER_TON",iGet("I_DESTINATION_NUMBER_TON"));
	o_add_field("I_DESTINATION_NUMBER",iGet("I_DESTINATION_NUMBER"));
	o_add_field("I_LEVEL_OF_CAMEL_SERVICE",iGet("I_LEVEL_OF_CAMEL_SERVICE"));
	o_add_field("I_CAMEL_MODIFICATION",iGet("I_CAMEL_MODIFICATION"));
	o_add_field("I_CAMEL_MODIFY_PARAMETERS",iGet("I_CAMEL_MODIFY_PARAMETERS"));
	o_add_field("I_NUMBER_OF_IN_RECORDS",iGet("I_NUMBER_OF_IN_RECORDS"));
	o_add_field("I_CALL_REFERENCE_TIME",iGet("I_CALL_REFERENCE_TIME"));
	o_add_field("I_LEG_ID",iGet("I_LEG_ID"));
	o_add_field("I_GLOBAL_CALL_REFERENCE",iGet("I_GLOBAL_CALL_REFERENCE"));
	o_add_field("I_DIALOGUE_TYPE",iGet("I_DIALOGUE_TYPE"));
	nb_write_record("DWH");	
	
	DIAG(DIAG_LOW, "write_COC(): Leaving the function");
}
void write_CTC()
{		
	DIAG(DIAG_LOW, "write_CTC(): Entered the function");
	
	nb_new_record();
	audit_out_type("CTC");
	o_add_field("I_RECORD_LENGTH",iGet("I_RECORD_LENGTH"));
	o_add_field("I_RECORD_TYPE",iGet("I_RECORD_TYPE"));
	o_add_field("I_RECORD_NUMBER",iGet("I_RECORD_NUMBER"));
	o_add_field("I_RECORD_STATUS",iGet("I_RECORD_STATUS"));
	o_add_field("I_CHECK_SUM",iGet("I_CHECK_SUM"));
	o_add_field("I_CALL_REFERENCE",iGet("I_CALL_REFERENCE"));
	o_add_field("I_EXCHANGE_ID",iGet("I_EXCHANGE_ID"));
	o_add_field("I_BASIC_CALL_STATE_MODEL",iGet("I_BASIC_CALL_STATE_MODEL"));
	o_add_field("I_CAMEL_MODIFY_PARAMETERS",iGet("I_CAMEL_MODIFY_PARAMETERS"));
	o_add_field("I_INTERMEDIATE_RECORD_NUMBER",iGet("I_INTERMEDIATE_RECORD_NUMBER"));
	o_add_field("I_INTERMEDIATE_CHARGING_IND",iGet("I_INTERMEDIATE_CHARGING_IND"));
	o_add_field("I_INTERMEDIATE_CHRG_CAUSE",iGet("I_INTERMEDIATE_CHRG_CAUSE"));
	o_add_field("I_IN_CHANNEL_ALLOCATED_TIME",iGet("I_IN_CHANNEL_ALLOCATED_TIME"));
	o_add_field("I_CHARGING_START_TIME",iGet("I_CHARGING_START_TIME"));
	o_add_field("I_CHARGING_END_TIME",iGet("I_CHARGING_END_TIME"));
	o_add_field("I_DURATION_BEFORE_ANSWER",iGet("I_DURATION_BEFORE_ANSWER"));
	o_add_field("I_CHARGEABLE_DURATION",iGet("I_CHARGEABLE_DURATION"));
	o_add_field("I_LEG_CALL_REFERENCE",iGet("I_LEG_CALL_REFERENCE"));
	o_add_field("I_CAMEL_CALL_REFERENCE",iGet("I_CAMEL_CALL_REFERENCE"));
	o_add_field("I_CAMEL_EXCHANGE_ID_TON",iGet("I_CAMEL_EXCHANGE_ID_TON"));
	o_add_field("I_CAMEL_EXCHANGE_ID",iGet("I_CAMEL_EXCHANGE_ID"));
	o_add_field("I_SCF_ADDRESS_TON",iGet("I_SCF_ADDRESS_TON"));
	o_add_field("I_SCF_ADDRESS",iGet("I_SCF_ADDRESS"));
	o_add_field("I_CAMEL_SERVICE_KEY",iGet("I_CAMEL_SERVICE_KEY"));
	o_add_field("I_DEFAULT_CALL_HANDLING",iGet("I_DEFAULT_CALL_HANDLING"));
	o_add_field("I_DESTINATION_NUMBER_TON",iGet("I_DESTINATION_NUMBER_TON"));
	o_add_field("I_DESTINATION_NUMBER",iGet("I_DESTINATION_NUMBER"));
	o_add_field("I_LEVEL_OF_CAMEL_SERVICE",iGet("I_LEVEL_OF_CAMEL_SERVICE"));
	o_add_field("I_CAMEL_MODIFICATION",iGet("I_CAMEL_MODIFICATION"));
	o_add_field("I_NUMBER_OF_IN_RECORDS",iGet("I_NUMBER_OF_IN_RECORDS"));
	o_add_field("I_CALL_REFERENCE_TIME",iGet("I_CALL_REFERENCE_TIME"));
	o_add_field("I_LEG_ID",iGet("I_LEG_ID"));
	o_add_field("I_GLOBAL_CALL_REFERENCE",iGet("I_GLOBAL_CALL_REFERENCE"));
	o_add_field("I_DIALOGUE_TYPE",iGet("I_DIALOGUE_TYPE"));
	nb_write_record("DWH");	
	
	DIAG(DIAG_LOW, "write_CTC(): Leaving the function");
}

void write_SUP()
{		
	DIAG(DIAG_LOW, "write_SUP(): Entered the function");
	
	nb_new_record();
	audit_out_type("Sup");
	o_add_field("I_RECORD_LENGTH",iGet("I_RECORD_LENGTH"));
	o_add_field("I_RECORD_TYPE",iGet("I_RECORD_TYPE"));
	o_add_field("I_RECORD_NUMBER",iGet("I_RECORD_NUMBER"));
	o_add_field("I_RECORD_STATUS",iGet("I_RECORD_STATUS"));
	o_add_field("I_CHECK_SUM",iGet("I_CHECK_SUM"));
	o_add_field("I_CALL_REFERENCE",iGet("I_CALL_REFERENCE"));
	o_add_field("I_EXCHANGE_ID",iGet("I_EXCHANGE_ID"));
	o_add_field("I_SS_RECORD_NUMBER",iGet("I_SS_RECORD_NUMBER"));
	o_add_field("I_SERVED_IMSI",iGet("I_SERVED_IMSI"));
	o_add_field("I_SERVED_IMEI",iGet("I_SERVED_IMEI"));
	o_add_field("I_SERVED_NUMBER",iGet("I_SERVED_NUMBER"));
	o_add_field("I_SERVED_SUBS_LAC",EL_FIRST_LAC);
	o_add_field("I_SERVED_SUBS_CI",iGet("I_SERVED_SUBS_CI"));
	o_add_field("I_SUPPLEMENTARY_SERVICE_CODE",iGet("I_SUPPLEMENTARY_SERVICE_CODE"));
	o_add_field("I_ACTION",iGet("I_ACTION"));
	o_add_field("I_IN_CHANNEL_ALLOCATED_TIME",iGet("I_IN_CHANNEL_ALLOCATED_TIME"));
	o_add_field("I_CHARGING_TIME",iGet("I_CHARGING_TIME"));
	o_add_field("I_CAUSE_FOR_TERMINATION",iGet("I_CAUSE_FOR_TERMINATION"));
	o_add_field("I_RESULT_INDICATOR",iGet("I_RESULT_INDICATOR"));
	o_add_field("I_PARAMETERS_LENGTH",iGet("I_PARAMETERS_LENGTH"));
	o_add_field("I_PARAMETERS",EL_SUP_PARAM);
	o_add_field("I_SERVED_NUMBER_TON",iGet("I_SERVED_NUMBER_TON"));
	o_add_field("I_SERVED_MS_CLASSMARK",iGet("I_SERVED_MS_CLASSMARK"));
	o_add_field("I_BASIC_SERVICE_TYPE",iGet("I_BASIC_SERVICE_TYPE"));
	o_add_field("I_BASIC_SERVICE_CODE",iGet("I_BASIC_SERVICE_CODE"));
	o_add_field("I_LEG_CALL_REFERENCE",iGet("I_LEG_CALL_REFERENCE"));
	o_add_field("I_CALL_REFERENCE_TIME",iGet("I_CALL_REFERENCE_TIME"));
	o_add_field("I_BASIC_CALL_STATE_MODEL",iGet("I_BASIC_CALL_STATE_MODEL"));
	o_add_field("I_CAMEL_CALL_REFERENCE",iGet("I_CAMEL_CALL_REFERENCE"));
	o_add_field("I_CAMEL_EXCHANGE_ID",iGet("I_CAMEL_EXCHANGE_ID"));
	o_add_field("I_CAMEL_EXCHANGE_ID_TON",iGet("I_CAMEL_EXCHANGE_ID_TON"));
	o_add_field("I_RADIO_NETWORK_TYPE",iGet("I_RADIO_NETWORK_TYPE"));
	o_add_field("I_SERVED_SUBS_MCC",iGet("I_SERVED_SUBS_MCC"));
	o_add_field("I_SERVED_SUBS_MNC",iGet("I_SERVED_SUBS_MNC"));
	o_add_field("I_PNI",iGet("I_PNI"));
	o_add_field("I_GLOBAL_CALL_REFERENCE",iGet("I_GLOBAL_CALL_REFERENCE"));
	o_add_field("I_HOT_BILLING_RECORD_NUMBER",iGet("I_HOT_BILLING_RECORD_NUMBER"));
	o_add_field("I_SERVED_IMEISV",iGet("I_SERVED_IMEISV"));
	o_add_field("I_ADD_ROUTING_CATEGORY",iGet("I_ADD_ROUTING_CATEGORY"));
	o_add_field("I_ROUTING_CATEGORY",iGet("I_ROUTING_CATEGORY"));
	o_add_field("I_MS_CLASSMARK3",iGet("I_MS_CLASSMARK3"));
	o_add_field("I_SERVED_CELL_BAND",iGet("I_SERVED_CELL_BAND"));
	nb_write_record("DWH");	
	
	DIAG(DIAG_LOW, "write_SUP(): Leaving the function");
}

void write_SOC()
{
	DIAG(DIAG_LOW, "write_SOC(): Entered the function");
	
	nb_new_record();
	audit_out_type("SOC");

		o_add_field("I_RECORD_LENGTH",iGet("I_RECORD_LENGTH"));
		o_add_field("I_RECORD_TYPE",iGet("I_RECORD_TYPE"));
		o_add_field("I_RECORD_NUMBER",iGet("I_RECORD_NUMBER"));
		o_add_field("I_RECORD_STATUS",iGet("I_RECORD_STATUS"));
		o_add_field("I_CHECK_SUM",iGet("I_CHECK_SUM"));
		o_add_field("I_CALL_REFERENCE",iGet("I_CALL_REFERENCE"));
		o_add_field("I_EXCHANGE_ID",iGet("I_EXCHANGE_ID"));
		o_add_field("I_INTERMEDIATE_RECORD_NUMBER",iGet("I_INTERMEDIATE_RECORD_NUMBER"));
		o_add_field("I_INTERMEDIATE_CHARGING_IND",iGet("I_INTERMEDIATE_CHARGING_IND"));
		o_add_field("I_INTERMEDIATE_CHRG_CAUSE",iGet("I_INTERMEDIATE_CHRG_CAUSE"));
		o_add_field("I_NUMBER_OF_SS_RECORDS",iGet("I_NUMBER_OF_SS_RECORDS"));
		o_add_field("I_NUMBER_OF_IN_RECORDS",iGet("I_NUMBER_OF_IN_RECORDS"));
		o_add_field("I_CALLING_IMSI",iGet("I_CALLING_IMSI"));
		o_add_field("I_CALLING_NUMBER_TON",iGet("I_CALLING_NUMBER_TON"));
		o_add_field("I_CALLING_NUMBER",iGet("I_CALLING_NUMBER"));
		o_add_field("I_CALLED_NUMBER_TON",iGet("I_CALLED_NUMBER_TON"));
		o_add_field("I_CALLED_NUMBER",iGet("I_CALLED_NUMBER"));
		o_add_field("I_CALLING_CATEGORY",iGet("I_CALLING_CATEGORY"));
		o_add_field("I_CALLED_CATEGORY",iGet("I_CALLED_CATEGORY"));
		o_add_field("I_IN_CHANNEL_ALLOCATED_TIME",iGet("I_IN_CHANNEL_ALLOCATED_TIME"));
		o_add_field("I_FACILITY_USAGE",iGet("I_FACILITY_USAGE"));
		o_add_field("I_CHARGING_START_TIME",iGet("I_CHARGING_START_TIME"));
		o_add_field("I_CHARGING_END_TIME",iGet("I_CHARGING_END_TIME"));
		o_add_field("I_ANSWER_TIME",iGet("I_ANSWER_TIME"));
		o_add_field("I_CAUSE_FOR_TERMINATION",iGet("I_CAUSE_FOR_TERMINATION"));
		o_add_field("I_CALL_TYPE",iGet("I_CALL_TYPE"));
		o_add_field("I_ORIG_MCZ_DURATION",iGet("I_ORIG_MCZ_DURATION"));
		o_add_field("I_ORIG_MCZ_MODIFY_PERCENT",iGet("I_ORIG_MCZ_MODIFY_PERCENT"));
		o_add_field("I_ORIG_MCZ_MODIFY_DIRECTION",iGet("I_ORIG_MCZ_MODIFY_DIRECTION"));
		o_add_field("I_LEG_CALL_REFERENCE",iGet("I_LEG_CALL_REFERENCE"));
		o_add_field("I_CALLING_MODIFY_PARAMETERS",iGet("I_CALLING_MODIFY_PARAMETERS"));
		o_add_field("I_CALL_REFERENCE_TIME",iGet("I_CALL_REFERENCE_TIME"));
		o_add_field("I_ICID_LENGTH",iGet("I_ICID_LENGTH"));
		o_add_field("I_ICID",iGet("I_ICID"));
		o_add_field("I_CALL_MEDIA",iGet("I_CALL_MEDIA"));
		o_add_field("I_SIP_SIG_MODE",iGet("I_SIP_SIG_MODE"));
		o_add_field("I_DIALLED_DIGITS_TON",iGet("I_DIALLED_DIGITS_TON"));
		o_add_field("I_DIALLED_DIGITS",iGet("I_DIALLED_DIGITS"));
		o_add_field("I_ICID_OVERFLOW",iGet("I_ICID_OVERFLOW"));
		o_add_field("I_ORIG_MCZ_CHANGE_DIRECTION",iGet("I_ORIG_MCZ_CHANGE_DIRECTION"));
		o_add_field("I_ORIG_MCZ_CHANGE_PERCENT",iGet("I_ORIG_MCZ_CHANGE_PERCENT"));
		o_add_field("I_SCP_CONNECTION",iGet("I_SCP_CONNECTION"));
		o_add_field("I_SELECTED_CODEC",iGet("I_SELECTED_CODEC"));
		o_add_field("I_PNI",iGet("I_PNI"));
		o_add_field("I_IN_BNC_CONNECTION_TYPE",iGet("I_IN_BNC_CONNECTION_TYPE"));
		o_add_field("I_INSIDE_CONTROL_PLANE_INDEX",iGet("I_INSIDE_CONTROL_PLANE_INDEX"));
		o_add_field("I_INSIDE_USER_PLANE_INDEX",iGet("I_INSIDE_USER_PLANE_INDEX"));
		o_add_field("I_GLOBAL_CALL_REFERENCE",iGet("I_GLOBAL_CALL_REFERENCE"));
		o_add_field("I_REDIRECTING_NUMBER_TON",iGet("I_REDIRECTING_NUMBER_TON"));
		o_add_field("I_REDIRECTING_NUMBER",iGet("I_REDIRECTING_NUMBER"));
		o_add_field("I_IN_CIRCUIT_GROUP",iGet("I_IN_CIRCUIT_GROUP"));
		o_add_field("I_IN_CIRCUIT_GROUP_NAME",iGet("I_IN_CIRCUIT_GROUP_NAME"));
		o_add_field("I_OUT_CIRCUIT_GROUP",iGet("I_OUT_CIRCUIT_GROUP"));
		o_add_field("I_OUT_CIRCUIT_GROUP_NAME",iGet("I_OUT_CIRCUIT_GROUP_NAME"));
		o_add_field("I_ADD_ROUTING_CATEGORY",iGet("I_ADD_ROUTING_CATEGORY"));
		o_add_field("I_ROUTING_CATEGORY",iGet("I_ROUTING_CATEGORY"));
		o_add_field("I_CAMEL_CALL_REFERENCE",iGet("I_CAMEL_CALL_REFERENCE"));
		o_add_field("I_CAMEL_EXCHANGE_ID",iGet("I_CAMEL_EXCHANGE_ID"));
		o_add_field("I_CAMEL_EXCHANGE_ID_TON",iGet("I_CAMEL_EXCHANGE_ID_TON"));
		o_add_field("I_NUMBER_OF_ALL_IN_RECORDS",iGet("I_NUMBER_OF_ALL_IN_RECORDS"));
		o_add_field("I_ORIG_MCZ_DURATION_TEN_MS",iGet("I_ORIG_MCZ_DURATION_TEN_MS"));
		o_add_field("I_DIRECTORY_NUMBER",iGet("I_DIRECTORY_NUMBER"));
		o_add_field("I_IN_INTER_OPERATOR_ID",iGet("I_IN_INTER_OPERATOR_ID"));
		o_add_field("I_IN_SBC_DOMAIN_NAME",iGet("I_IN_SBC_DOMAIN_NAME"));

	nb_write_record("DWH");	
	
	DIAG(DIAG_LOW, "write_SOC(): Leaving the function");
}

void write_STC()
{
	DIAG(DIAG_LOW, "write_STC(): Entered the function");
	
	nb_new_record();
	audit_out_type("STC");
	o_add_field("I_RECORD_LENGTH",iGet("I_RECORD_LENGTH"));
	o_add_field("I_RECORD_TYPE",iGet("I_RECORD_TYPE"));
	o_add_field("I_RECORD_NUMBER",iGet("I_RECORD_NUMBER"));
	o_add_field("I_RECORD_STATUS",iGet("I_RECORD_STATUS"));
	o_add_field("I_CHECK_SUM",iGet("I_CHECK_SUM"));
	o_add_field("I_CALL_REFERENCE",iGet("I_CALL_REFERENCE"));
	o_add_field("I_EXCHANGE_ID",iGet("I_EXCHANGE_ID"));
	o_add_field("I_INTERMEDIATE_RECORD_NUMBER",iGet("I_INTERMEDIATE_RECORD_NUMBER"));
	o_add_field("I_INTERMEDIATE_CHARGING_IND",iGet("I_INTERMEDIATE_CHARGING_IND"));
	o_add_field("I_INTERMEDIATE_CHRG_CAUSE",iGet("I_INTERMEDIATE_CHRG_CAUSE"));
	o_add_field("I_NUMBER_OF_SS_RECORDS",iGet("I_NUMBER_OF_SS_RECORDS"));
	o_add_field("I_NUMBER_OF_IN_RECORDS",iGet("I_NUMBER_OF_IN_RECORDS"));
	o_add_field("I_CALLED_IMSI",iGet("I_CALLED_IMSI"));
	o_add_field("I_CALLING_NUMBER_TON",iGet("I_CALLING_NUMBER_TON"));
	o_add_field("I_CALLING_NUMBER",iGet("I_CALLING_NUMBER"));
	o_add_field("I_CALLED_NUMBER_TON",iGet("I_CALLED_NUMBER_TON"));
	o_add_field("I_CALLED_NUMBER",iGet("I_CALLED_NUMBER"));
	o_add_field("I_CALLED_CATEGORY",iGet("I_CALLED_CATEGORY"));
	o_add_field("I_ROUTING_CATEGORY",iGet("I_ROUTING_CATEGORY"));
	o_add_field("I_FACILITY_USAGE",iGet("I_FACILITY_USAGE"));
	o_add_field("I_IN_CHANNEL_ALLOCATED_TIME",iGet("I_IN_CHANNEL_ALLOCATED_TIME"));
	o_add_field("I_CHARGING_START_TIME",iGet("I_CHARGING_START_TIME"));
	o_add_field("I_CHARGING_END_TIME",iGet("I_CHARGING_END_TIME"));
	o_add_field("I_ANSWER_TIME",iGet("I_ANSWER_TIME"));
	o_add_field("I_CAUSE_FOR_TERMINATION",iGet("I_CAUSE_FOR_TERMINATION"));
	o_add_field("I_CALL_TYPE",iGet("I_CALL_TYPE"));
	o_add_field("I_TERM_MCZ_DURATION",iGet("I_TERM_MCZ_DURATION"));
	o_add_field("I_TERM_MCZ_MODIFY_PERCENT",iGet("I_TERM_MCZ_MODIFY_PERCENT"));
	o_add_field("I_TERM_MCZ_MODIFY_DIRECTION",iGet("I_TERM_MCZ_MODIFY_DIRECTION"));
	o_add_field("I_LEG_CALL_REFERENCE",iGet("I_LEG_CALL_REFERENCE"));
	o_add_field("I_CALLED_MODIFY_PARAMETERS",iGet("I_CALLED_MODIFY_PARAMETERS"));
	o_add_field("I_CALL_REFERENCE_TIME",iGet("I_CALL_REFERENCE_TIME"));
	o_add_field("I_ICID_LENGTH",iGet("I_ICID_LENGTH"));
	o_add_field("I_ICID",iGet("I_ICID"));
	o_add_field("I_SIP_SIG_MODE",iGet("I_SIP_SIG_MODE"));
	o_add_field("I_SCP_CONNECTION",iGet("I_SCP_CONNECTION"));
	o_add_field("I_TERM_MCZ_CHANGE_DIRECTION",iGet("I_TERM_MCZ_CHANGE_DIRECTION"));
	o_add_field("I_TERM_MCZ_CHANGE_PERCENT",iGet("I_TERM_MCZ_CHANGE_PERCENT"));
	o_add_field("I_PNI",iGet("I_PNI"));
	o_add_field("I_OUT_BNC_CONNECTION_TYPE",iGet("I_OUT_BNC_CONNECTION_TYPE"));
	o_add_field("I_OUTSIDE_CONTROL_PLANE_INDE",iGet("I_OUTSIDE_CONTROL_PLANE_INDE"));
	o_add_field("I_OUTSIDE_USER_PLANE_INDEX",iGet("I_OUTSIDE_USER_PLANE_INDEX"));
	o_add_field("I_GLOBAL_CALL_REFERENCE",iGet("I_GLOBAL_CALL_REFERENCE"));
	o_add_field("I_REDIRECTING_NUMBER_TON",iGet("I_REDIRECTING_NUMBER_TON"));
	o_add_field("I_REDIRECTING_NUMBER",iGet("I_REDIRECTING_NUMBER"));
	o_add_field("I_IN_CIRCUIT_GROUP",iGet("I_IN_CIRCUIT_GROUP"));
	o_add_field("I_IN_CIRCUIT_GROUP_NAME",iGet("I_IN_CIRCUIT_GROUP_NAME"));
	o_add_field("I_OUT_CIRCUIT_GROUP",iGet("I_OUT_CIRCUIT_GROUP"));
	o_add_field("I_OUT_CIRCUIT_GROUP_NAME",iGet("I_OUT_CIRCUIT_GROUP_NAME"));
	o_add_field("I_ADD_ROUTING_CATEGORY",iGet("I_ADD_ROUTING_CATEGORY"));
	o_add_field("I_NUMBER_OF_ALL_IN_RECORDS",iGet("I_NUMBER_OF_ALL_IN_RECORDS"));
	o_add_field("I_TERM_MCZ_DURATION_TEN_MS",iGet("I_TERM_MCZ_DURATION_TEN_MS"));
	o_add_field("I_DIRECTORY_NUMBER",iGet("I_DIRECTORY_NUMBER"));
	o_add_field("I_OUT_INTER_OPERATOR_ID",iGet("I_OUT_INTER_OPERATOR_ID"));
	o_add_field("I_OUT_SBC_DOMAIN_NAME",iGet("I_OUT_SBC_DOMAIN_NAME"));
	o_add_field("I_CAMEL_CALL_REFERENCE",iGet("I_CAMEL_CALL_REFERENCE"));
	o_add_field("I_CAMEL_EXCHANGE_ID",iGet("I_CAMEL_EXCHANGE_ID"));
	o_add_field("I_CAMEL_EXCHANGE_ID_TON",iGet("I_CAMEL_EXCHANGE_ID_TON"));
	o_add_field("I_ICID_OVERFLOW",iGet("I_ICID_OVERFLOW"));
	
	nb_write_record("DWH");	
	
	DIAG(DIAG_LOW, "write_STC(): Leaving the function");
}


void set_FIRST_LAC_DEC()
	
{
	
	DIAG(DIAG_LOW, "set_FIRST_LAC_DEC(): entered the function");
	if (strcmp(record_Type, "moc") == 0)
	{
		convert_HEX_DEC(iGet("I_CALLING_SUBS_FIRST_LAC"),EL_FIRST_LAC);	
	}
	else if (strcmp(record_Type, "mtc") == 0) {
		
		convert_HEX_DEC(iGet("I_CALLED_SUBS_FIRST_LAC"),EL_FIRST_LAC);
	}
	else if ((strcmp(record_Type, "smmo") == 0) || (strcmp(record_Type, "smmt") == 0) || (strcmp(record_Type, "Sup") == 0)) {
		
		convert_HEX_DEC(iGet("I_SERVED_SUBS_LAC"),EL_FIRST_LAC);  
	}
	
	
	DIAG(DIAG_LOW, "set_FIRST_LAC_DEC(): FIRST_LAC Value for Record Type [%s] after conversion = %s",record_Type,EL_FIRST_LAC);
	DIAG(DIAG_LOW, "set_FIRST_LAC_DEC(): leaving the function");
}

void set_LAST_LAC_DEC()
	
{
	
	DIAG(DIAG_LOW, "set_LAST_LAC_DEC(): entered the function");
	if (strcmp(record_Type, "moc") == 0)
	{
		convert_HEX_DEC(iGet("I_CALLING_SUBS_LAST_LAC"),EL_LAST_LAC);	
	}
	else if (strcmp(record_Type, "mtc") == 0) {
		
		convert_HEX_DEC(iGet("I_CALLED_SUBS_LAST_LAC"),EL_LAST_LAC);
	}
	
	
	DIAG(DIAG_LOW, "set_LAST_LAC_DEC(): LAST_LAC Value for Record Type [%s] after conversion = %s",record_Type,EL_LAST_LAC);
	DIAG(DIAG_LOW, "set_LAST_LAC_DEC(): leaving the function");
}

void set_IN_CIRCUIT_GROUP_NAME()
{
	DIAG(DIAG_LOW, "set_IN_CIRCUIT_GROUP_NAME(): entered the function");
	
		convert_HEX_ASCII(iGet("I_IN_CIRCUIT_GROUP_NAME"),EL_IN_CIRCUIT_GROUP_NAME);	
		
	DIAG(DIAG_LOW, "set_IN_CIRCUIT_GROUP_NAME(): EL_IN_CIRCUIT_GROUP_NAME Value  after conversion = %s",EL_IN_CIRCUIT_GROUP_NAME);	
	DIAG(DIAG_LOW, "set_IN_CIRCUIT_GROUP_NAME(): Leaving  the function");
}

void set_OUT_CIRCUIT_GROUP_NAME()
{
	DIAG(DIAG_LOW, "set_OUT_CIRCUIT_GROUP_NAME(): entered the function");
	
		convert_HEX_ASCII(iGet("I_OUT_CIRCUIT_GROUP_NAME"),EL_OUT_CIRCUIT_GROUP_NAME);	
		
	DIAG(DIAG_LOW, "set_OUT_CIRCUIT_GROUP_NAME(): EL_OUT_CIRCUIT_GROUP_NAME Value  after conversion = %s",EL_OUT_CIRCUIT_GROUP_NAME);		
	DIAG(DIAG_LOW, "set_OUT_CIRCUIT_GROUP_NAME(): Leaving  the function");
}

void set_SMMT_CAlling_NUMBER()
{
	
	char* temp = {'\0'};
	
	DIAG(DIAG_LOW, "set_SMMT_CAlling_NUMBER(): entered the function");
	if (strcmp(record_Type, "smmt") == 0) {
		
		strcpy(SMMT_CAL_NUM,iGet("I_CALLING_NUMBER"));
		temp = swap_number_F(SMMT_CAL_NUM);
		if(strcmp(iGet("I_CALLING_NUMBER_TON"), "24") == 0)
		{
			convert_HEX_ASCII(temp,EL_SMMT_CALLING_NUMBER);
			
		}
		else
		{
			strcpy(EL_SMMT_CALLING_NUMBER,temp);
			
		}
        DIAG(DIAG_LOW, "set_SMMT_CAlling_NUMBER(): Calling Number : = %s",EL_SMMT_CALLING_NUMBER);
	   
	}
	DIAG(DIAG_LOW, "set_SMMT_CAlling_NUMBER(): Leaving  the function");
}

void set_SUP_Parameters()
{
	char* temp = {'\0'};
	char User_Action [5];
	char USSD_OPerations [5];
	char Time_Stamp [40];
	char USSD_CODE [40];
	
	if (strcmp(record_Type, "Sup") == 0)
	{
		  strcpy(USSD_PARAM,iGet("I_PARAMETERS"));
		  DIAG(DIAG_MEDIUM, "get_VAS_USSD_Special_Number_Result(): USSD_PARAM = %s",USSD_PARAM);
		  temp =  Remove_Filler(USSD_PARAM);	
		  substring(temp,0,2,User_Action);
		  substring(temp,2,2,USSD_OPerations);
		  substring(temp,4,14,Time_Stamp);
		  substring(temp,18,(strlen(temp)-18),USSD_CODE);
		  DIAG(DIAG_MEDIUM, "get_VAS_USSD_Special_Number_Result(): USSD_CODE = %s",USSD_CODE);
		  DIAG(DIAG_MEDIUM, "get_VAS_USSD_Special_Number_Result(): User_Action = %s",User_Action);
		  DIAG(DIAG_MEDIUM, "get_VAS_USSD_Special_Number_Result(): USSD_OPerations = %s",USSD_OPerations);
		  DIAG(DIAG_MEDIUM, "get_VAS_USSD_Special_Number_Result(): Time_Stamp = %s",Time_Stamp);
		  convert_HEX_ASCII(USSD_CODE,EL_USSD_PARAMETER);
		   DIAG(DIAG_MEDIUM, "get_VAS_USSD_Special_Number_Result(): EL_USSD_PARAMETER = %s",EL_USSD_PARAMETER);
		  strcpy (EL_SUP_PARAM,User_Action);
		  strcat(EL_SUP_PARAM,":");
		  strcat (EL_SUP_PARAM,USSD_OPerations);
		  strcat(EL_SUP_PARAM,":");
		  strcat (EL_SUP_PARAM,Time_Stamp);
		  strcat(EL_SUP_PARAM,":");
		  strcat (EL_SUP_PARAM,EL_USSD_PARAMETER);
		  DIAG(DIAG_MEDIUM, "get_VAS_USSD_Special_Number_Result(): Parameter = %s",EL_SUP_PARAM);
		  
		  
	}
	
}

void convert_HEX_DEC(const char *input,char *output)
{   
	unsigned int dec1,dec2;
    char tmp[30];
	char LAC[30];
	if (strcmp(input,"FFFF") != 0)
	{
		lowercase(input,tmp);
		strcpy(LAC,tmp);
	    DIAG(DIAG_LOW, "convert_HEX_DEC(): Value = %s ",LAC);
        sscanf(LAC,"%02x%02x",&dec1,&dec2);
		if (dec2 == 0){
    	sprintf(output,"%d",dec1);
		}
		else{
			sprintf(output,"%d%d",dec1,dec2);
		}
	}
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

char* swap_number(char *num)
{
     int len = strlen(num);
     int i,lenx;
     DIAG(DIAG_HIGH, "swap_number(): Length of input number: %d, before swap: [%s]",len, num);
     lenx = len - 2;
     for ( i = 0; i <= lenx ; i=i+2){
     swap(&num[i],&num[i+1]);
     }

 return num;
}
char* swap_number_F(char *num)
{
     int len = strlen(num);
     int i,len1,lenx;
     DIAG(DIAG_HIGH, "swap_number(): Length of input number: %d, b4 swap: [%s]",len, num);
     lenx = len - 2;
     for ( i = 0; i <= lenx ; i=i+2){
     swap(&num[i],&num[i+1]);
     }

     len1 = strlen(num);
     DIAG(DIAG_HIGH, "swap_number(): Length of input number: %d, af swap: [%s]",len1, num);
     for ( i = len; i >= 0 ; i--)
     {
        if (num[i-1] == 'F'){
        num[i-1] = 0;
     }
     else {
        if ( len == len1 ){
        num[len1] = 0;
        break;
        }
     }
 }
 DIAG(DIAG_HIGH, "swap_number(): return number after removing trailing f char: %s", num);
 return num;
}
void swap(char *x, char *y)
{
  int tmp;
  tmp = *x;
  *x = *y;
  *y = tmp;
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

 