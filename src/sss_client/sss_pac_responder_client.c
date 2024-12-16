/*
    Copyright (C) 2016 Red Hat

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

#ifdef __FreeBSD__
#include <pthread_np.h>
#endif // __FreeBSD__
#include <sys/syscall.h>

#include "sss_client/sss_cli.h"
#include "util/util.h"

const uint8_t pac[] = {
0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x10,
0x02, 0x00, 0x00, 0x58, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x00,
0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x68, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x0c, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x78, 0x02, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0xb8,
0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x10, 0x00,
0x00, 0x00, 0xc8, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x08,
0x00, 0xcc, 0xcc, 0xcc, 0xcc, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x02, 0x00, 0x30, 0xe3, 0xd6, 0x9e, 0x99, 0x2b, 0xd3, 0x01, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0x7f, 0xe2, 0xf7, 0x8a, 0xaf, 0x00, 0x0f, 0xd0, 0x01, 0xe2, 0xb7, 0xf4,
0xd9, 0xc9, 0x0f, 0xd0, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f,
0x06, 0x00, 0x06, 0x00, 0x04, 0x00, 0x02, 0x00, 0x06, 0x00, 0x06, 0x00, 0x08,
0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00, 0x00,
0x00, 0x00, 0x10, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x02,
0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x02, 0x00, 0x45, 0x02, 0x00, 0x00,
0x50, 0x04, 0x00, 0x00, 0x01, 0x02, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x1c,
0x00, 0x02, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x00, 0x14,
0x00, 0x20, 0x00, 0x02, 0x00, 0x04, 0x00, 0x06, 0x00, 0x24, 0x00, 0x02, 0x00,
0x28, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10,
0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x2c, 0x00, 0x02, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x74, 0x00,
0x75, 0x00, 0x31, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x03, 0x00, 0x00, 0x00, 0x74, 0x00, 0x20, 0x00, 0x75, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00,
0xfd, 0xa2, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x01, 0x02, 0x00, 0x00, 0x07,
0x00, 0x00, 0x00, 0x5c, 0x04, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x56, 0x04,
0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x89, 0xa6, 0x00, 0x00, 0x07, 0x00, 0x00,
0x00, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00,
0x41, 0x00, 0x44, 0x00, 0x2d, 0x00, 0x53, 0x00, 0x45, 0x00, 0x52, 0x00, 0x56,
0x00, 0x45, 0x00, 0x52, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x41, 0x00, 0x44, 0x00, 0x04, 0x00, 0x00,
0x00, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x15, 0x00, 0x00, 0x00,
0xf8, 0x12, 0x13, 0xdc, 0x47, 0xf3, 0x1c, 0x76, 0x47, 0x2f, 0x2e, 0xd7, 0x02,
0x00, 0x00, 0x00, 0x30, 0x00, 0x02, 0x00, 0x07, 0x00, 0x00, 0x00, 0x34, 0x00,
0x02, 0x00, 0x07, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x01, 0x05, 0x00,
0x00, 0x00, 0x00, 0x00, 0x05, 0x15, 0x00, 0x00, 0x00, 0x29, 0xc9, 0x4f, 0xd9,
0xc2, 0x3c, 0xc3, 0x78, 0x36, 0x55, 0x87, 0xf8, 0x54, 0x04, 0x00, 0x00, 0x05,
0x00, 0x00, 0x00, 0x01, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x15, 0x00,
0x00, 0x00, 0x25, 0xe1, 0xff, 0x1c, 0xf7, 0x87, 0x6b, 0x2c, 0x25, 0xd2, 0x0c,
0xe3, 0xf2, 0x03, 0x00, 0x00, 0x00, 0x2c, 0x29, 0x89, 0x65, 0x2d, 0xd3, 0x01,
0x06, 0x00, 0x74, 0x00, 0x75, 0x00, 0x31, 0x00, 0x20, 0x00, 0x10, 0x00, 0x10,
0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x74, 0x00,
0x75, 0x00, 0x31, 0x00, 0x74, 0x00, 0x65, 0x00, 0x73, 0x00, 0x74, 0x00, 0x40,
0x00, 0x61, 0x00, 0x64, 0x00, 0x2e, 0x00, 0x64, 0x00, 0x65, 0x00, 0x76, 0x00,
0x65, 0x00, 0x6c, 0x00, 0x41, 0x00, 0x44, 0x00, 0x2e, 0x00, 0x44, 0x00, 0x45,
0x00, 0x56, 0x00, 0x45, 0x00, 0x4c, 0x00, 0x10, 0x00, 0x00, 0x00, 0x76, 0x8e,
0x25, 0x32, 0x7c, 0x85, 0x00, 0x32, 0xac, 0x8f, 0x02, 0x2c, 0x10, 0x00, 0x00,
0x00, 0x6b, 0xe8, 0x51, 0x03, 0x30, 0xed, 0xca, 0x7d, 0xe2, 0x12, 0xa5, 0xde};

enum nss_status _nss_sss_getpwuid_r(uid_t uid, struct passwd *result,
                                    char *buffer, size_t buflen, int *errnop);
static void *pac_client(void *arg)
{
    struct sss_cli_req_data sss_data = { sizeof(pac), pac };
    int errnop = -1;
    int ret;
    size_t c;

    fprintf(stderr, "[%"SPRItime"][%d][%ld][%s] started\n",
#ifdef __FreeBSD__
            time(NULL), getpid(), pthread_getthreadid_np(), (char *) arg);
#else // __FreeBSD__
            time(NULL), getpid(), syscall(SYS_gettid), (char *) arg);
#endif // __FreeBSD__
    for (c = 0; c < 1000; c++) {
        /* sss_pac_make_request() does not protect the client's file
         * descriptor to the PAC responder. With this one thread will miss a
         * reply for an SSS_GET_VERSION request and will wait until
         * SSS_CLI_SOCKET_TIMEOUT is passed.

        ret = sss_pac_make_request(SSS_PAC_ADD_PAC_USER, &sss_data,
                                   NULL, NULL, &errnop);
         */
        ret = sss_pac_make_request_with_lock(SSS_PAC_ADD_PAC_USER, &sss_data,
                                             NULL, NULL, &errnop);
        if (ret != NSS_STATUS_SUCCESS
                && !(ret == NSS_STATUS_UNAVAIL && errnop != ECONNREFUSED)) {
                /* NSS_STATUS_UNAVAIL is returned if the PAC responder rejects
                 * the request which is ok because the client is waiting for a
                 * response here as well. Only errnop == ECONNREFUSED should
                 * be treated as error because this means that the PAC
                 * responder is not running. */
            fprintf(stderr, "pac: [%s][%d][%d]\n", (char *)arg, ret, errnop);
            return ((void *)((uintptr_t)("X")));
        }
    }

    fprintf(stderr, "[%"SPRItime"][%s] done\n", time(NULL),(char *) arg);
    return NULL;
}

int main(void)
{
    pthread_t thread1;
    pthread_t thread2;
    int ret;
    void *t_ret;

    pthread_create(&thread1, NULL, pac_client,
                   ((void *)((uintptr_t)("Thread 1"))));
    pthread_create(&thread2, NULL, pac_client,
                   ((void *)((uintptr_t)("Thread 2"))));

    ret = pthread_join(thread1, &t_ret);
    if (ret != 0 || t_ret != NULL) {
        fprintf(stderr, "Thread 1 failed.\n");
        return EIO;
    }

    ret = pthread_join(thread2, &t_ret);
    if (ret != 0 || t_ret != NULL) {
        fprintf(stderr, "Thread 1 failed.\n");
        return EIO;
    }

    return 0;
}
