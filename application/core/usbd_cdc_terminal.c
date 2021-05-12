/*
 * usbd_cdc_terminal.c
 *
 *  Created on: Mar 1, 2021
 *      Author: nenad
 */

#include <stdbool.h>
#include <string.h>

#include "usbd_cdc_terminal.h"
#include "generic/terminal/nk_terminal.h"
#include "generic/nk_array.h"
#include "generic/nk_string.h"
#include "app_usbd_cdc.h"
#include "config_usbd_cdc_terminal.h"
#include "mdrv.h"
#include "command_help.h"
#include "command_rxchg.h"
#include "command_set.h"
#include "command_get.h"

struct usbd_cdc_buffer
    NK_STRING__BUCKET_T(CONFIG__USBD_CDC_TERMINAL__BUFFER_SIZE)
;

struct usbd_cdc_terminal__state
{
    struct usb_input
    {
        struct usbd_cdc_buffer data;
        volatile bool is_pending;
    } usb_input;
    struct terminal_descriptor *terminal;
};

static void usb_receive(void *arg, const void *data, uint16_t length);

struct command_id
    NK_STRING__BUCKET_T(20)
;

static struct command_id g__command_id__help = NK_STRING__BUCKET_INITIALIZER(&g__command_id__help,
                    "help")
;
static struct command_id g__command_id__rxchg = NK_STRING__BUCKET_INITIALIZER(&g__command_id__rxchg,
                    "rxchg")
;
static struct command_id g__command_id__set = NK_STRING__BUCKET_INITIALIZER(&g__command_id__set,
                    "set")
;
static struct command_id g__command_id__get = NK_STRING__BUCKET_INITIALIZER(&g__command_id__get,
                    "get")
;
static const struct terminal__command_descriptor g__command_desc__help = {
    .command_id = &g__command_id__help.array,
    .interpreter = {
        .fn = command_help__fn}};

static const struct terminal__command_descriptor g__command_desc__rxchg = {
    .command_id = &g__command_id__rxchg.array,
    .interpreter = {
        .fn = command_rxchg__fn}};

static const struct terminal__command_descriptor g__command_desc__set = {
    .command_id = &g__command_id__set.array,
    .interpreter = {
        .fn = command_set__fn}};

static const struct terminal__command_descriptor g__command_desc__get = {
    .command_id = &g__command_id__get.array,
    .interpreter = {
        .fn = command_get__fn}};

static struct
    NK_ARRAY__BUCKET_TYPED_T(const struct terminal__command_descriptor *, 4, struct terminal_commands)
g__terminal_commands = NK_ARRAY__BUCKET_INITIALIZER(&g__terminal_commands,
                    4,
                    {
                        &g__command_desc__help,
                        &g__command_desc__rxchg,
                        &g__command_desc__set,
                        &g__command_desc__get,
                    })
;

static struct terminal_descriptor g__terminal;
static struct usbd_cdc_terminal__state g__state = {
    .usb_input = {
        .data = NK_STRING__BUCKET_INITIALIZER_EMPTY(&g__state.usb_input.data) }, };

static void usb_receive(void *arg, const void *data, uint16_t length)
{
    struct usbd_cdc_terminal__state *context = arg;

    if (length >= CONFIG__USBD_CDC_TERMINAL__BUFFER_SIZE) {
        return;
    }
    if (!context->usb_input.is_pending) {
        context->usb_input.is_pending = true;
        nk_string__append_literal(&context->usb_input.data.array, data, length);
    }
}

void usbd_cdc_terminal__init(void)
{
    static struct usbd_cdc_buffer arg_buffer = NK_STRING__BUCKET_INITIALIZER_EMPTY(&arg_buffer);
    static const struct app_usbd_cdc__context usbd_cdc_context = {
        .receive = usb_receive,
        .arg = &g__state};
    g__state.terminal = &g__terminal;
    static struct
        NK_STRING__BUCKET_T(100)
    error_message =
            NK_STRING__BUCKET_INITIALIZER(&error_message, "\n\rE0001: unknown command, type 'help' for help\n\r")
    ;
    terminal__init(&g__terminal,
                   &g__terminal_commands.array,
                   &arg_buffer.array,
                   &error_message.array);
    app_usbd_cdc__init(&usbd_cdc_context);
}

void usbd_cdc_terminal__set_terminal_context(struct usbd_cdc_terminal__context *terminal_context)
{
    terminal__set_terminal_context(g__state.terminal, terminal_context);
}

void usbd_cdc_terminal__loop(void)
{
    static enum sm_state
    {
        STATE_PROCESS,
        STATE_RETRY_TRANSMIT
    } sm_state = STATE_PROCESS;
    static struct usbd_cdc_buffer output = NK_STRING__BUCKET_INITIALIZER_EMPTY(&output);
    struct usbd_cdc_terminal__state *context = &g__state;

    switch (sm_state) {
    case STATE_PROCESS:
        if (context->usb_input.is_pending) {
            terminal__interpret(context->terminal, &context->usb_input.data.array, &output.array);
            nk_string__clear_all(&context->usb_input.data.array);
            context->usb_input.is_pending = false;
            if (output.array.length) {
                enum app_usbd_cdc__error error;

                error = app_usbd_cdc__transmit(output.array.items, (uint16_t) output.array.length);

                if (error != APP_USBD_CDC__ERROR__OK) {
                    sm_state = STATE_RETRY_TRANSMIT;
                }
                nk_string__clear_all(&output.array);
            }
        }
        break;
    case STATE_RETRY_TRANSMIT: {
        enum app_usbd_cdc__error error;

        error = app_usbd_cdc__transmit(output.array.items, (uint16_t) output.array.length);

        if (error != APP_USBD_CDC__ERROR__BUSY) {
            nk_string__clear_all(&output.array);
            sm_state = STATE_PROCESS;
        }
        break;
    }
    default:
        sm_state = STATE_PROCESS;
        break;
    }
}
