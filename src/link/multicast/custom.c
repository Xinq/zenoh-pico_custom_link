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

#include "zenoh-pico/link/config/custom.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "zenoh-pico/config.h"
#include "zenoh-pico/link/manager.h"
#include "zenoh-pico/system/link/custom.h"
#include "zenoh-pico/utils/pointers.h"

#if Z_FEATURE_LINK_CUSTOM_MULTICAST == 1

char *__z_parse_port_segment_custom_multicast(const char *address) {
    char *ret = NULL;

    const char *p_start = strrchr(address, ':');
    if (p_start != NULL) {
        p_start = _z_cptr_char_offset(p_start, 1);

        const char *p_end = &address[strlen(address)];

        size_t len = _z_ptr_char_diff(p_end, p_start) + (size_t)1;
        ret = (char *)z_malloc(len);
        if (ret != NULL) {
            _z_str_n_copy(ret, p_start, len);
        }
    }

    return ret;
}

char *__z_parse_address_segment_custom_multicast(const char *address) {
    char *ret = NULL;

    const char *p_start = &address[0];
    const char *p_end = strrchr(address, ':');

    // IPv6
    if ((p_start[0] == '[') && (p_end[-1] == ']')) {
        p_start = _z_cptr_char_offset(p_start, 1);
        p_end = _z_cptr_char_offset(p_end, -1);
        size_t len = _z_ptr_char_diff(p_end, p_start) + (size_t)1;
        ret = (char *)z_malloc(len);
        if (ret != NULL) {
            _z_str_n_copy(ret, p_start, len);
        }
    }
    // IPv4
    else {
        size_t len = _z_ptr_char_diff(p_end, p_start) + (size_t)1;
        ret = (char *)z_malloc(len);
        if (ret != NULL) {
            _z_str_n_copy(ret, p_start, len);
        }
    }
    return ret;
}

int8_t _z_endpoint_custom_multicast_valid(_z_endpoint_t *endpoint) {
    int8_t ret = _Z_RES_OK;    

    if (_z_str_eq(endpoint->_locator._protocol, CUSTOM_SCHEMA) != true) {
        ret = _Z_ERR_CONFIG_LOCATOR_INVALID;
    }

    if (ret == _Z_RES_OK) {
        char *s_address = __z_parse_address_segment_custom_multicast(endpoint->_locator._address);
        if (s_address == NULL) {
            ret = _Z_ERR_CONFIG_LOCATOR_INVALID;
        } else {
            z_free(s_address);
        }
    }

    if (ret == _Z_RES_OK) {
        char *s_port = __z_parse_port_segment_custom_multicast(endpoint->_locator._address);
        if (s_port == NULL) {
            ret = _Z_ERR_CONFIG_LOCATOR_INVALID;
        } else {
            // uint32_t port = strtoul(s_port, NULL, 10);
            // if ((port < (uint32_t)1) || (port > (uint32_t)65355)) {  // Port numbers should range from 1 to 65355
            //     ret = _Z_ERR_CONFIG_LOCATOR_INVALID;
            // }
            z_free(s_port);
        }
    }

    const char *iface = _z_str_intmap_get(&endpoint->_config, CUSTOM_CONFIG_IFACE_KEY);
    if (iface == NULL) {
        // ret = _Z_ERR_CONFIG_LOCATOR_INVALID;
    }
    return ret;
}

int8_t _z_f_link_open_custom_multicast(_z_link_t *self) {
    int8_t ret = _Z_RES_OK;

    uint32_t tout = Z_CONFIG_SOCKET_TIMEOUT;
    char *tout_as_str = _z_str_intmap_get(&self->_endpoint._config, CUSTOM_CONFIG_TOUT_KEY);
    if (tout_as_str != NULL) {
        tout = strtoul(tout_as_str, NULL, 10);
    }

    const char *iface = _z_str_intmap_get(&self->_endpoint._config, CUSTOM_CONFIG_IFACE_KEY);
    ret = _z_open_custom_multicast(&self->_socket._custom._sock, self->_socket._custom._rep, &self->_socket._custom._lep, tout,
                                iface);

    return ret;
}

int8_t _z_f_link_listen_custom_multicast(_z_link_t *self) {
    int8_t ret = _Z_RES_OK;
    const char *iface = _z_str_intmap_get(&self->_endpoint._config, CUSTOM_CONFIG_IFACE_KEY);
    const char *join = _z_str_intmap_get(&self->_endpoint._config, CUSTOM_CONFIG_JOIN_KEY);
    ret = _z_listen_custom_multicast(&self->_socket._custom._sock, self->_socket._custom._rep, Z_CONFIG_SOCKET_TIMEOUT, iface,
                                  join);
    ret |= _z_open_custom_multicast(&self->_socket._custom._sock, self->_socket._custom._rep, &self->_socket._custom._lep,
                                 Z_CONFIG_SOCKET_TIMEOUT, iface);
    return ret;
}

void _z_f_link_close_custom_multicast(_z_link_t *self) {
    _z_close_custom_multicast(&self->_socket._custom._sock, &self->_socket._custom._msock, self->_socket._custom._rep,
                           self->_socket._custom._lep);
}

void _z_f_link_free_custom_multicast(_z_link_t *self) {
    _z_free_endpoint_custom(&self->_socket._custom._lep);
    _z_free_endpoint_custom(&self->_socket._custom._rep);
}

size_t _z_f_link_write_custom_multicast(const _z_link_t *self, const uint8_t *ptr, size_t len) {
    return _z_send_custom_multicast(self->_socket._custom._sock, ptr, len, self->_socket._custom._rep);
}

size_t _z_f_link_write_all_custom_multicast(const _z_link_t *self, const uint8_t *ptr, size_t len) {
    return _z_send_custom_multicast(self->_socket._custom._sock, ptr, len, self->_socket._custom._rep);
}

size_t _z_f_link_read_custom_multicast(const _z_link_t *self, uint8_t *ptr, size_t len, _z_bytes_t *addr) {
    return _z_read_custom_multicast(self->_socket._custom._sock, ptr, len, self->_socket._custom._lep, addr);
}

size_t _z_f_link_read_exact_custom_multicast(const _z_link_t *self, uint8_t *ptr, size_t len, _z_bytes_t *addr) {
    return _z_read_exact_custom_multicast(self->_socket._custom._sock, ptr, len, self->_socket._custom._lep, addr);
}

uint16_t _z_get_link_mtu_custom_multicast(void) {
    // @TODO: the return value should change depending on the target platform.
    return 1450;
}

int8_t _z_new_link_custom_multicast(_z_link_t *zl, _z_endpoint_t endpoint) {
    int8_t ret = _Z_RES_OK;

    zl->_cap._transport = Z_LINK_CAP_TRANSPORT_MULTICAST;
    zl->_cap._flow = Z_LINK_CAP_FLOW_DATAGRAM;
    zl->_cap._is_reliable = false;

    zl->_mtu = _z_get_link_mtu_custom_multicast();

    zl->_endpoint = endpoint;
    char *s_address = __z_parse_address_segment_custom_multicast(endpoint._locator._address);
    char *s_port = __z_parse_port_segment_custom_multicast(endpoint._locator._address);
    ret = _z_create_endpoint_custom(&zl->_socket._custom._sock, s_address, s_port);
    // memset(&zl->_socket._custom._lep, 0, sizeof(zl->_socket._custom._lep));
    z_free(s_address);
    z_free(s_port);

    zl->_open_f = _z_f_link_open_custom_multicast;
    zl->_listen_f = _z_f_link_listen_custom_multicast;
    zl->_close_f = _z_f_link_close_custom_multicast;
    zl->_free_f = _z_f_link_free_custom_multicast;

    zl->_write_f = _z_f_link_write_custom_multicast;
    zl->_write_all_f = _z_f_link_write_all_custom_multicast;
    zl->_read_f = _z_f_link_read_custom_multicast;
    zl->_read_exact_f = _z_f_link_read_exact_custom_multicast;

    return ret;
}

#endif