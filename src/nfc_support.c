//
//  nfc_support.c
//  UpsilonAudio
//
//  Created by Pierre Pelé on 14/06/2015.
//
//

/*
 *         Copyright (c), NXP Semiconductors Gratkorn / Austria
 *
 *                     (C)NXP Semiconductors
 *       All rights are reserved. Reproduction in whole or in part is
 *      prohibited without the written consent of the copyright owner.
 *  NXP reserves the right to make changes without notice at any time.
 * NXP makes no warranty, expressed, implied or statutory, including but
 * not limited to any implied warranty of merchantability or fitness for any
 *particular purpose, or that the use will not infringe any third party patent,
 * copyright or trademark. NXP must not be liable for any loss or damage
 *                          arising from its use.
 */

#include "nfc_support.h"
#include "neardal.h"

typedef struct params {
    
    gboolean debug;
    gchar* adapterObjectPath;
    GMainLoop* pMainLoop;
    gint returnCode;
    
} params_t;

// VARIABLES GLOBALES

gchar* uid_land = NULL;
gchar* uid_takeoff = NULL;

// DÉFINITION DES FONCTIONS

static gint start_polling(params_t* pParams)
{
    //List current adapters
    errorCode_t	err;
    
    int adaptersCount = 0;
    char** adapterNamesArray = NULL;
    err = neardal_get_adapters(&adapterNamesArray, &adaptersCount);
    if( err == NEARDAL_ERROR_NO_ADAPTER )
    {
        g_printf("No adapter found\r\n");
        return 1;
    }
    else if(err != NEARDAL_SUCCESS)
    {
        g_warning("Error %d when listing adapters (%s)\r\n", err, neardal_error_get_text(err));
        return 1;
    }
    
    gboolean adapterFound = FALSE;
    
    for(int i = 0; i < adaptersCount; i++)
    {
        if( pParams->adapterObjectPath != NULL )
        {
            if(strcmp(pParams->adapterObjectPath, adapterNamesArray[i]) != 0)
            {
                continue;
            }
        }
        
        //Reconfigure adapters if needed
        neardal_adapter* pAdapter;
        err = neardal_get_adapter_properties(adapterNamesArray[i], &pAdapter);
        if(err != NEARDAL_SUCCESS)
        {
            g_warning("Error %d when accessing adapter %s (%s)\r\n", err, adapterNamesArray[i], neardal_error_get_text(err));
            continue;
        }
        
        if( !pAdapter->powered )
        {
            g_info("Powering adapter %s\r\n", pAdapter->name);
            err = neardal_set_adapter_property(pAdapter->name, NEARD_ADP_PROP_POWERED, GUINT_TO_POINTER(1));
            if(err != NEARDAL_SUCCESS)
            {
                g_warning("Error %d when trying to power adapter %s (%s)\r\n", err, pAdapter->name, neardal_error_get_text(err));
            }
        }
        
        if( pAdapter->polling == 0 )
        {
            //Start polling
            g_info("Starting polling for adapter %s\r\n", pAdapter->name);
            err = neardal_start_poll(pAdapter->name); //Mode NEARD_ADP_MODE_INITIATOR
            if(err != NEARDAL_SUCCESS)
            {
                g_warning("Error %d when trying to start polling on adapter %s (%s)\r\n", err, pAdapter->name, neardal_error_get_text(err));
            }
        }
        
        neardal_free_adapter(pAdapter);
        
        adapterFound = TRUE;
        if( pParams->adapterObjectPath != NULL )
        {
            break;
        }
    }
    neardal_free_array(&adapterNamesArray);
    
    if(!adapterFound)
    {
        g_printf("Adapter not found\r\n");
        return 1;
    }
    
    return 0; //OK
}

static void dump_record(neardal_record* pRecord)
{
    if( pRecord->name != NULL )
    {
        g_printf("Found record %s\r\n", pRecord->name);
    }
    else
    {
        g_printf("Found record\r\n");
    }
    
    if( pRecord->type != NULL )
    {
        g_printf("Record type: \t%s\r\n", pRecord->type);
    }
    else
    {
        g_printf("Unknown record type\r\n");
    }
    
    //Dump fields that are set
    if( pRecord->uri != NULL )
    {
        g_printf("URI: \t%s\r\n", pRecord->uri);
    }
    
    if( pRecord->representation != NULL )
    {
        g_printf("Title: \t%s\r\n", pRecord->representation);
    }
    
    if( pRecord->action != NULL )
    {
        g_printf("Action: \t%s\r\n", pRecord->action);
    }
    
    if( pRecord->language != NULL )
    {
        g_printf("Language: \t%s\r\n", pRecord->language);
    }
    
    if( pRecord->encoding != NULL )
    {
        g_printf("Encoding: \t%s\r\n", pRecord->encoding);
    }
    
    if( pRecord->mime != NULL )
    {
        g_printf("MIME type: \t%s\r\n", pRecord->mime);
    }
    
    if( pRecord->uriObjSize > 0 )
    {
        g_printf("URI object size: \t%u\r\n", pRecord->uriObjSize);
    }
    
    if( pRecord->carrier != NULL )
    {
        g_printf("Carrier: \t%s\r\n", pRecord->carrier);
    }
    
    if( pRecord->ssid != NULL )
    {
        g_printf("SSID: \t%s\r\n", pRecord->ssid);
    }
    
    if( pRecord->passphrase != NULL )
    {
        g_printf("Passphrase: \t%s\r\n", pRecord->passphrase);
    }
    
    if( pRecord->encryption != NULL )
    {
        g_printf("Encryption: \t%s\r\n", pRecord->encryption);
    }
    
    if( pRecord->authentication != NULL )
    {
        g_printf("Authentication: \t%s\r\n", pRecord->authentication);
    }
}

static gchar* bytes_to_str(GBytes* bytes)
{
    gchar* str = g_malloc0( 2*g_bytes_get_size(bytes) + 1 );
    const guint8* data = g_bytes_get_data(bytes, NULL);
    for(int i = 0 ; i < g_bytes_get_size(bytes); i++)
    {
        sprintf(&str[2*i], "%02X", data[i]);
    }
    return str;
}

static void handle_tag(neardal_tag* pTag)
{
    if( pTag->iso14443aAtqa != NULL )
    {
        gchar *str = bytes_to_str(pTag->iso14443aAtqa);
        g_printf("ISO14443A ATQA: \t%s\r\n", str);
        g_free(str);
    }
    
    if( pTag->iso14443aSak != NULL )
    {
        gchar *str = bytes_to_str(pTag->iso14443aSak);
        g_printf("ISO14443A SAK: \t%s\r\n", str);
        g_free(str);
    }
    
    if( pTag->iso14443aUid != NULL )
    {
        gchar *str = bytes_to_str(pTag->iso14443aUid);
        g_printf("ISO14443A UID: \t%s\r\n", str);
        
        if (uid_land != NULL && g_strcmp0(str, uid_land) == 0) {
            on_nfc_land();
        } else if (uid_takeoff != NULL && g_strcmp0(str, uid_takeoff) == 0) {
            on_nfc_take_off();
        }
        
        g_free(str);
    }
    
    if( pTag->felicaManufacturer != NULL )
    {
        gchar *str = bytes_to_str(pTag->felicaManufacturer);
        g_printf("Felica Manufacturer: \t%s\r\n", str);
        g_free(str);
    }
    
    if( pTag->felicaCid != NULL )
    {
        gchar *str = bytes_to_str(pTag->felicaCid);
        g_printf("Felica CID: \t%s\r\n", str);
        g_free(str);
    }
    
    if( pTag->felicaIc != NULL )
    {
        gchar *str = bytes_to_str(pTag->felicaIc);
        g_printf("Felica IC: \t%s\r\n", str);
        g_free(str);
    }
    
    if( pTag->felicaMaxRespTimes != NULL )
    {
        gchar *str = bytes_to_str(pTag->felicaMaxRespTimes);
        g_printf("Felica Maximum response times: \t%s\r\n", str);
        g_free(str);
    }
}

static void record_found(const char *recordName, void *pUserData)
{
    errorCode_t	err;
    neardal_record* pRecord;
    
    params_t* pParams = (params_t*) pUserData;
    
    err = neardal_get_record_properties(recordName, &pRecord);
    if(err != NEARDAL_SUCCESS)
    {
        g_warning("Error %d when reading record %s (%s)\r\n", err, recordName, neardal_error_get_text(err));
        return;
    }
    
    //Dump record's content
    dump_record(pRecord);
    neardal_free_record(pRecord);
}

static void tag_found(const char *tagName, void *pUserData)
{
    g_printf("Tag found\r\n");
    
    errorCode_t	err;
    neardal_tag* pTag;
    
    err = neardal_get_tag_properties(tagName, &pTag);
    if(err != NEARDAL_SUCCESS)
    {
        g_warning("Error %d when reading tag %s (%s)\r\n", err, tagName, neardal_error_get_text(err));
        return;
    }
    
    //Dump record's content
    handle_tag(pTag);
    neardal_free_tag(pTag);
}

static void device_found(const char *name, void *pUserData)
{
    g_printf("Device found\r\n");
}

static void tag_device_lost(const char *name, void *pUserData)
{
    params_t* pParams = (params_t*) pUserData;
    
    //Go through adapters and restart polling
    pParams->returnCode = start_polling(pParams);
    if(pParams->returnCode != 0)
    {
        g_main_loop_quit(pParams->pMainLoop);
    }
}

static int nfc_main(int argc,
                    char** argv) {
    
    params_t params;
    params.debug = FALSE;
    params.adapterObjectPath = NULL;
    params.returnCode = 0;
    uid_land = NULL;
    uid_takeoff = NULL;
    
    //Parse options
    const GOptionEntry entries[] =
    {
        { "debug", 'd', 0, G_OPTION_ARG_NONE, &params.debug, "Enable debugging mode", NULL },
        { "uid-take-off", 't', 0, G_OPTION_ARG_STRING, &uid_takeoff, "UID for take off.", NULL},
        { "uid-land", 'l', 0, G_OPTION_ARG_STRING, &uid_land, "UID for land", NULL},
        { "adapter", 'a', 0, G_OPTION_ARG_STRING, &params.adapterObjectPath, "Use a specific adapter", NULL},
        { NULL }
    };
    
    GOptionContext* pContext = g_option_context_new(NULL);
    g_option_context_add_main_entries(pContext, entries, NULL);
    
    GError* pError = NULL;
    if(!g_option_context_parse(pContext, &argc, &argv, &pError))
    {
        if(pError != NULL)
        {
            g_printerr("%s\r\n", pError->message);
            g_error_free(pError);
        }
        else
        {
            g_printerr("An unknown error occurred\r\n");
        }
        return 1;
    }
    g_option_context_free(pContext);
    
    if(params.debug)
    {
        g_setenv("G_MESSAGES_DEBUG", "all", 1);
    }
    
    //Initialize GLib main loop
    params.pMainLoop = g_main_loop_new(NULL, FALSE);
    
    //Add tag found callback
    neardal_set_cb_tag_found(tag_found, (gpointer)&params);
    neardal_set_cb_dev_found(device_found, (gpointer)&params);
    
    //Add record found callback
    neardal_set_cb_record_found(record_found, (gpointer)&params);
    
    //Add tag/device lost callbacks
    neardal_set_cb_tag_lost(tag_device_lost, (gpointer)&params);
    neardal_set_cb_dev_lost(tag_device_lost, (gpointer)&params);
    
    params.returnCode = start_polling(&params);
    if(params.returnCode != 0)
    {
        return params.returnCode;
    }
    
    g_printf("Waiting for tag or device...\r\n");
    
    //Will run till signal or record found
    g_main_loop_run(params.pMainLoop);
    
    if( params.adapterObjectPath != NULL ) {
        g_free(params.adapterObjectPath);
    }
    
    return params.returnCode;
}

void *nfc_thread_func(void *data) {
    
    cli_params *params = NULL;
    
    if (data) {
        params = (cli_params*) data;
    } else {
        return NULL;
    }
    
    int status_code = nfc_main(params->argc, params->argv);
    
    return NULL;
}
