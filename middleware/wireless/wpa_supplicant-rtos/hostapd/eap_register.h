/*
 * EAP method registration
 * Copyright (c) 2004-2009, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef EAP_REGISTER_H
#define EAP_REGISTER_H

#if CONFIG_HOSTAPD
int eap_server_register_methods(void);
#endif /* CONFIG_HOSTAPD */
#endif /* EAP_REGISTER_H */
