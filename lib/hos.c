/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2019, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/

#include "curl_setup.h"

#ifdef USE_LIBNX

#include <switch.h>

#undef BIT

#include "urldata.h"
#include "setopt.h"
#include "sendf.h"
#include "hos.h"

/* The last #include files should be: */
#include "curl_printf.h"
#include "curl_memory.h"
#include "memdebug.h"

CURLcode Curl_os_get_system_proxy(struct connectdata *conn)
{
  struct Curl_easy *data = conn->data;
  CURLcode result = CURLE_OK;
  Result rc = 0;
  NifmNetworkProfileData profile;
  NifmProxySetting *proxy = &profile.ip_setting_data.proxy_setting;
  char server[0x64];
  char user[0x20];
  char password[0x20];

  rc = nifmInitialize(NifmServiceType_User);
  if(R_SUCCEEDED(rc)) {
    rc = nifmGetCurrentNetworkProfile(&profile);
    nifmExit();

    if(R_SUCCEEDED(rc) && proxy->enabled) {
      data->set.proxyport = proxy->port;

      strncpy(server, proxy->server, sizeof(server) - 1);
      server[sizeof(server) - 1] = 0;

      result = Curl_setstropt(&data->set.str[STRING_PROXY], server);
      if(result)
        return result;

      if(proxy->auto_auth_enabled) {
        strncpy(user, proxy->user, sizeof(user)-1);
        user[sizeof(user) - 1] = 0;

        result = Curl_setstropt(&data->set.str[STRING_PROXYUSERNAME], user);
        if(result)
          return result;

        strncpy(password, proxy->password, sizeof(password)-1);
        password[sizeof(password) - 1] = 0;

        result = Curl_setstropt(&data->set.str[STRING_PROXYPASSWORD], password);
        if(result)
          return result;

        conn->bits.proxy_user_passwd = TRUE;
      }
    }
  }

  return result;
}
#endif /* USE_LIBNX */

