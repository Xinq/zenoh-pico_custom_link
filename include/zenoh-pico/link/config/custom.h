//
// Copyright (c) 2022 ZettaScale Technology
//
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// http://www.eclipse.org/legal/epl-2.0, or the Apache License, Version 2.0
// which is available at https://www.apache.org/licenses/LICENSE-2.0.
//
// SPDX-License-Identifier: EPL-2.0 OR Apache-2.0
//
// Contributors:
//   ZettaScale Zenoh Team, <zenoh@zettascale.tech>
//

#ifndef ZENOH_PICO_LINK_CONFIG_CUSTOM_H
#define ZENOH_PICO_LINK_CONFIG_CUSTOM_H

#include "zenoh-pico/collections/intmap.h"
#include "zenoh-pico/collections/string.h"

#define CUSTOM_CONFIG_ARGC 3

#define CUSTOM_CONFIG_IFACE_KEY 0x01
#define CUSTOM_CONFIG_IFACE_STR "iface"

#define CUSTOM_CONFIG_TOUT_KEY 0x02
#define CUSTOM_CONFIG_TOUT_STR "tout"

#define CUSTOM_CONFIG_JOIN_KEY 0x03
#define CUSTOM_CONFIG_JOIN_STR "join"

#if Z_FEATURE_LINK_CUSTOM_UNICAST == 1 || Z_FEATURE_LINK_CUSTOM_MULTICAST == 1
#define CUSTOM_CONFIG_MAPPING_BUILD                 \
    _z_str_intmapping_t args[CUSTOM_CONFIG_ARGC];   \
    args[0]._key = CUSTOM_CONFIG_IFACE_KEY;         \
    args[0]._str = (char *)CUSTOM_CONFIG_IFACE_STR; \
    args[1]._key = CUSTOM_CONFIG_TOUT_KEY;          \
    args[1]._str = (char *)CUSTOM_CONFIG_TOUT_STR;  \
    args[2]._key = CUSTOM_CONFIG_JOIN_KEY;          \
    args[2]._str = (char *)CUSTOM_CONFIG_JOIN_STR;

size_t _z_custom_config_strlen(const _z_str_intmap_t *s);

void _z_custom_config_onto_str(char *dst, size_t dst_len, const _z_str_intmap_t *s);
char *_z_custom_config_to_str(const _z_str_intmap_t *s);

int8_t _z_custom_config_from_str(_z_str_intmap_t *strint, const char *s);
int8_t _z_custom_config_from_strn(_z_str_intmap_t *strint, const char *s, size_t n);
#endif

#endif /* ZENOH_PICO_LINK_CONFIG_CUSTOM_H */
