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
	DIAG(DIAG_HIGH, "read_input_output_fields(): Done.");
}

/* -----------------------------------------------------------------------------
* Function  : app_validate()
* Purpose   : Implements Record Validation Rules
* Arguments : None
* Return    : PASS or FAIL
* --------------------------------------------------------------------------- */
int app_validate(void)
{  
   
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
		
	DIAG(DIAG_LOW, "handle_conversion_rules(): Leaving the function");
}

/*----------------------------- output functions
------------------------------------*/
void write_output_fields()
{	
	DIAG(DIAG_LOW, "write_output_fields(): Entered the function");
	write_IMS_PRM();
	 write_IMS_SP();
	write_IMS_DWH();
	DIAG(DIAG_LOW, "write_output_fields(): Leaving the function");
}		


void write_IMS_DWH()
{		
	DIAG(DIAG_LOW, "write_IMS_DWH(): Entered the function");
	     nb_new_record();
	        audit_out_type("IMS_DWH");
		  o_add_field("dialled-Party-Address_sIP-URI",iGet("dialled-Party-Address.sIP-URI"));
		o_add_field("record-Type",iGet("record-Type"));
		o_add_field("retransmission",iGet("retransmission"));
        o_add_field("node-Address_domainName",iGet("node-Address.domainName"));  
		o_add_field("session-ID",iGet("session-ID"));
		o_add_field("Calling-Party-Address_sIP-URI",iGet("Calling-Party-Address.sIP-URI"));
		o_add_field("Calling-Party-Address_tEL-URI",iGet("Calling-Party-Address.tEL-URI"));
		
		o_add_field("service-Request-Time-Stamp",iGet("service-Request-Time-Stamp"));
		o_add_field("iMS-Charging-Identifier",iGet("iMS-Charging-Identifier"));  
		o_add_field("privateUserID",iGet("privateUserID"));
		o_add_field("inter-Operator-Identifiers_Inter-Operator-Identifiers_terminating-IOI",iGet("inter-Operator-Identifiers.Inter-Operator-Identifiers.terminating-IOI"));
		o_add_field("role-of-Node",iGet("role-of-Node"));
		o_add_field("service-Delivery-Start-Time-Stamp",iGet("service-Delivery-Start-Time-Stamp"));
		o_add_field("record-Opening-Time",iGet("record-Opening-Time"));
		o_add_field("record-Opening-Time-Fraction",iGet("record-Opening-Time-Fraction"));
		o_add_field("list-Of-SDP-Media-Components_List-of-Media-Components_sIP-Request-Time-Stamp",iGet("list-Of-SDP-Media-Components.List-of-Media-Components.sIP-Request-Time-Stamp"));
		o_add_field("list-Of-SDP-Media-Components_List-of-Media-Components_sIP-Response-Time-Stamp",iGet("list-Of-SDP-Media-Components.List-of-Media-Components.sIP-Response-Time-Stamp"));
		o_add_field("list-Of-SDP-Media-Components_List-of-Media-Components_sDP-Session-Description_GraphicString",iGet("list-Of-SDP-Media-Components.List-of-Media-Components.sDP-Session-Description.GraphicString"));
	
		o_add_field("list-Of-SDP-Media-Components_List-of-Media-Components_sDP-Media-Components_SDP-Media-Component_sDP-Media-Name",iGet("list-Of-SDP-Media-Components.List-of-Media-Components.sDP-Media-Components.SDP-Media-Component.sDP-Media-Name"));
	
		o_add_field("list-Of-SDP-Media-Components_List-of-Media-Components_sDP-Media-Components_SDP-Media-Component_sDP-Media-Descriptions_GraphicString_1",iGet("list-Of-SDP-Media-Components.List-of-Media-Components.sDP-Media-Components.SDP-Media-Component.sDP-Media-Descriptions.GraphicString[0]"));
		o_add_field("list-Of-SDP-Media-Components_List-of-Media-Components_sDP-Media-Components_SDP-Media-Component_sDP-Media-Descriptions_GraphicString_2",iGet("list-Of-SDP-Media-Components.List-of-Media-Components.sDP-Media-Components.SDP-Media-Component.sDP-Media-Descriptions.GraphicString[1]"));
		
		o_add_field("list-Of-SDP-Media-Components_List-of-Media-Components_sDP-Media-Components_SDP-Media-Component_sDP-Media-Descriptions_GraphicString_3",iGet("list-Of-SDP-Media-Components.List-of-Media-Components.sDP-Media-Components.SDP-Media-Component.sDP-Media-Descriptions.GraphicString[2]"));
		
		o_add_field("list-Of-SDP-Media-Components_List-of-Media-Components_sDP-Media-Components_SDP-Media-Component_sDP-Media-Descriptions_GraphicString_4",iGet("list-Of-SDP-Media-Components.List-of-Media-Components.sDP-Media-Components.SDP-Media-Component.sDP-Media-Descriptions.GraphicString[3]"));
		
		o_add_field("list-Of-SDP-Media-Components_List-of-Media-Components_sDP-Media-Components_SDP-Media-Component_sDP-Media-Descriptions_GraphicString_5",iGet("list-Of-SDP-Media-Components.List-of-Media-Components.sDP-Media-Components.SDP-Media-Component.sDP-Media-Descriptions.GraphicString[4]"));
		
		o_add_field("list-Of-SDP-Media-Components_List-of-Media-Components_sDP-Media-Components_SDP-Media-Component_sDP-Media-Descriptions_GraphicString_6",iGet("list-Of-SDP-Media-Components.List-of-Media-Components.sDP-Media-Components.SDP-Media-Component.sDP-Media-Descriptions.GraphicString[5]"));
	
		o_add_field("applicationServersInformation_ApplicationServersInformation_applicationServersInvolved_domainName",iGet("applicationServersInformation.ApplicationServersInformation.applicationServersInvolved.domainName"));
	
		o_add_field("Total_volume",iGet("Total_volume"));
		o_add_field("record-Sequence-Number",iGet("record-Sequence-Number"));
		o_add_field("called-Party-Address_sIP-URI",iGet("called-Party-Address.sIP-URI"));
		o_add_field("service-Reason-Return-Code",iGet("service-Reason-Return-Code"));
		o_add_field("service-Delivery-End-Time-Stamp",iGet("service-Delivery-End-Time-Stamp"));
		o_add_field("record-Closure-Time",iGet("record-Closure-Time"));
		o_add_field("record-Closure-Time-Fraction",iGet("record-Closure-Time-Fraction"));
		o_add_field("cause-For-Record-Closing",iGet("cause-For-Record-Closing")); 
		o_add_field("duration",iGet("duration"));  
		o_add_field("local-Record-Sequence-Number",iGet("local-Record-Sequence-Number"));
				nb_write_record("DWH");	
	
	DIAG(DIAG_LOW, "write_IMS_DWH(): Leaving the function");
}
void write_IMS_SP()
{		
	DIAG(DIAG_LOW, "write_IMS_SP(): Entered the function");
	     nb_new_record();
	        audit_out_type("IMS_SP");
			    o_add_field("TYPE","IMS");
				o_add_field("IMSI","");
				o_add_field("Originator",iGet("list-Of-Calling-Party-Address.tEL-URI"));
				o_add_field("Destination",iGet("called-Party-Address.sIP-URI"));
				o_add_field("date_time",iGet("record-Opening-Time"));
				o_add_field("location_info","");
				o_add_field("operator_id",iGet("inter-Operator-Identifiers.Inter-Operator-Identifiers.terminating-IOI"));
				o_add_field("Volume",iGet("Total_volume"));
				o_add_field("orig_cdr_id",iGet("session-ID"));
				o_add_field("service_id","101");
				o_add_field("Info_transfer_cap","0");
				o_add_field("user_rate","");
				o_add_field("syn_asyn","");
				o_add_field("user_info_layer1","");   
				o_add_field("updated_location_info","");
			
		    nb_write_record("SUREPAY");	
	
	DIAG(DIAG_LOW, "write_IMS_SP(): Leaving the function");
}

void write_IMS_PRM()
{		
	DIAG(DIAG_LOW, "write_IMS_PRM(): Entered the function");
	     nb_new_record();
	     audit_out_type("IMS_PRM");
	     o_add_field("NE_CODE","");
          o_add_field("CDR_TYPE","");
          o_add_field("SERVICE_ID","");
          o_add_field("IN_TRUNK","IN_TRUNK");
          o_add_field("OUT_TRUNK","OUT_TRUNK");
          o_add_field("HNI","HNI");
          o_add_field("A_NUMBER",iGet("list-Of-Calling-Party-Address.tEL-URI"));
          o_add_field("B_NUMBER","");
          o_add_field("C_NUMBER","");
           o_add_field("SGSN_ADDRESS","");
          o_add_field("APN_ADDRESS","");
          o_add_field("PDP_ADDRESS",""); 
          o_add_field("CALL_DATE","");
          o_add_field("CALL_DURATION","");
           o_add_field("INCOMING_GPRS_VOLUME","");
          o_add_field("OUTGOING_GPRS_VOLUME",""); 
          o_add_field("TELESERVICE","");  
          o_add_field("BEARERSERVICE","00");	
          o_add_field("CAMEL_FLAG","");
          o_add_field("CAMEL_SERVICE_LEVEL",""); 
          o_add_field("CAMEL_SERVICE_KEY","");		
          o_add_field("CAMEL_DEFAULT_CALL_HANDLING","");
          o_add_field("CAMEL_SERVER_ADDRESS","");  
          o_add_field("CAMEL_MSC_ADDRESS","");
          o_add_field("CAMEL_CALL_REF_NUM","");
          o_add_field("CAMEL_INIT_CF_INDICATOR","");
          o_add_field("CAMEL_DESTINATION_NUM","");    		
          o_add_field("CAMEL_MODIFICATION","");
          o_add_field("SUPPLIMENTARY_NUM","");
          o_add_field("NETWORK_TIME","");
          o_add_field("REASON_FOR_CLEARDOWN","");
           o_add_field("PARTIAL_INDICATOR","");
          o_add_field("PARTIAL_CDR_SEQ_NUM","");
          o_add_field("CHARGING_ID",""); 		
          o_add_field("IMEI_NUMBER","");
          o_add_field("CDR_CHRONO_NUM","");
          o_add_field("SUBSCRIBER_TYPE",""); 
          o_add_field("DIALLED_DIGITS","");
           o_add_field("SUPPLEMENTARY_ACTION_CODE","");  
          o_add_field("CELL_ID",""); 
          o_add_field("UTC_OFFSET","");		
           o_add_field("MNP_IDENTIFIER","");
          o_add_field("P_CSCF_ADDRESS","");
          o_add_field("A_USERID","");
          o_add_field("B_USERID","");
          o_add_field("EVENT_REFERENCE","");
          o_add_field("APN_OPERATOR_ID","");	
          o_add_field("QCI","");
          o_add_field("MAXIMUM_BIT_RATE","");
          o_add_field("GUARANTEED_BIT_RATE","");
          o_add_field("ALLOCATION_AND_RETENTION_PRIORITY",""); 
          o_add_field("NETWORK_CALL_REFERENCE_NUMBER",""); 
          o_add_field("ROAMING_DURATION","");
           o_add_field("ANSWER_TIME","");
          o_add_field("RELEASE_TIME","");
          o_add_field("DISCONNECT_TIME","");
          o_add_field("DEFAULT_DURATION",""); 				
		    nb_write_record("PRM");	
	
	DIAG(DIAG_LOW, "write_IMS_PRM(): Leaving the function");
}


 