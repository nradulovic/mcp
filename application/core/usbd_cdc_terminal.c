/*
 * usbd_cdc_terminal.c
 *
 *  Created on: Mar 1, 2021
 *      Author: nenad
 */

#include <stdbool.h>
#include <string.h>

#include "terminal.h"
#include "app_usbd_cdc.h"
#include "config_usbd_cdc_terminal.h"
#include "command_help.h"

struct usbd_cdc_terminal__context
{
    struct usb_input
    {
        char data[CONFIG__USBD_CDC_TERMINAL__BUFFER_SIZE];
        size_t data_size;
        volatile bool is_pending;
    } usb_input;
    struct terminal_descriptor *terminal;
};

static void usb_receive(void *arg, const void *data, uint16_t length);

static char g_terminal_buffer[CONFIG__USBD_CDC_TERMINAL__BUFFER_SIZE];

static const struct terminal__command_descriptor g__commands[] = {
    {
        .command_id = "help",
        .interpreter = {
            .fn = command_help__fn, }}};

static struct terminal_descriptor g__terminal = {
    .command_arg_buffer = &g_terminal_buffer[0],
    .command_arg_size = CONFIG__USBD_CDC_TERMINAL__BUFFER_SIZE,
    .commands = &g__commands[0],
    .error_message = "\r\nType 'help' for help.\r\n",
    .no_commands = sizeof(g__commands) / sizeof(g__commands[0]),
    .terminal_context = NULL};

static struct usbd_cdc_terminal__context g__context;

static void usb_receive(void *arg, const void *data, uint16_t length)
{
    struct usbd_cdc_terminal__context *context = arg;

    if (length >= CONFIG__USBD_CDC_TERMINAL__BUFFER_SIZE) {
        return;
    }
    if (!context->usb_input.is_pending) {
        context->usb_input.is_pending = true;
        context->usb_input.data_size = length;
        memcpy(&context->usb_input.data[0], data, length);
    }
}

void usbd_cdc_terminal__init(void)
{
    static const struct app_usbd_cdc__context usbd_cdc_context = {
        .receive = usb_receive,
        .arg = &g__context};
    g__context.terminal = &g__terminal;
    app_usbd_cdc__init(&usbd_cdc_context);
}

void usbd_cdc_terminal__loop(void)
{
    static enum sm_state
    {
        STATE_PROCESS,
        STATE_RETRY_TRANSMIT
    } sm_state = STATE_PROCESS;
    static const char *response;
    struct usbd_cdc_terminal__context *context = &g__context;

    switch (sm_state) {
    case STATE_PROCESS:
        if (context->usb_input.is_pending) {
            response = terminal__interpret(context->terminal,
                                           &context->usb_input.data[0],
                                           context->usb_input.data_size);
            context->usb_input.is_pending = false;
            if (response) {
                enum app_usbd_cdc__error error;

                error = app_usbd_cdc__transmit(response, (uint16_t) strlen(response));

                if (error != APP_USBD_CDC__ERROR__OK) {
                    sm_state = STATE_RETRY_TRANSMIT;
                }
            }
        }
        break;
    case STATE_RETRY_TRANSMIT: {
        enum app_usbd_cdc__error error;

        error = app_usbd_cdc__transmit(response, (uint16_t) strlen(response));

        if (error != APP_USBD_CDC__ERROR__BUSY) {
            sm_state = STATE_PROCESS;
        }
        break;
    }
    default:
        sm_state = STATE_PROCESS;
        break;
    }

}
