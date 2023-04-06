/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <stddef.h>
#include "os/os.h"
#include "sysinit/sysinit.h"
#include "host/ble_hs.h"
#include "nimble/nimble_port.h"
#include "nimble/transport.h"
#if NIMBLE_CFG_CONTROLLER
#include "controller/ble_ll.h"
#endif
#ifndef RIOT_VERSION
#include "hal/hal_timer.h"
#include "os/os_cputime.h"
#endif

static struct ble_npl_eventq g_eventq_dflt;

// TODO filter with ifdef, controller stuff
extern void os_msys_init(void);
extern void os_mempool_module_init(void);
extern void ble_transport_init(void);
extern void ble_transport_ll_init(void);
extern void ipc_nrf5340_init(const uint8_t* mac_nr);

// host stuff
void ble_svc_gap_init(void);
void ble_svc_gatt_init(void);
void ble_svc_bas_init(void);
void ble_store_config_init(void);

void nimble_port_init_host(const uint8_t* mac_nr)
{
    /* Initialize default event queue */
    ble_npl_eventq_init(&g_eventq_dflt);
    /* Initialize the global memory pool */
    os_mempool_module_init();
    os_msys_init();
    /* Initialize transport */
    ipc_nrf5340_init(mac_nr);

    ble_transport_init();

    /* Initialize the host */
    ble_transport_hs_init();

    ble_svc_gap_init();

    ble_svc_gatt_init();

    ble_svc_bas_init();

    ble_store_config_init();

    ble_transport_ll_init();
}

#if NIMBLE_CFG_CONTROLLER
void nimble_port_init_controller(void)
{
    ble_npl_eventq_init(&g_eventq_dflt);
    os_mempool_module_init();
    os_msys_init();

    ipc_nrf5340_init(NULL);
    ble_transport_init();
    ble_transport_hs_init();

#ifndef RIOT_VERSION
    hal_timer_init(5, NULL);
    os_cputime_init(32768);
#endif
    ble_transport_ll_init();
}
#endif

void
nimble_port_run(void)
{
    struct ble_npl_event *ev;

    while (1) {
        ev = ble_npl_eventq_get(&g_eventq_dflt, BLE_NPL_TIME_FOREVER);
        ble_npl_event_run(ev);
    }
}

struct ble_npl_eventq *
nimble_port_get_dflt_eventq(void)
{
    return &g_eventq_dflt;
}

#if NIMBLE_CFG_CONTROLLER
void
nimble_port_ll_task_func(void *arg)
{
    extern void ble_ll_task(void *);

    ble_ll_task(arg);
}
#endif
