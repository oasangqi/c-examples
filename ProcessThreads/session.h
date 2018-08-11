/*
 * Copyright Hunan Kylin Information Engineering Technology Co, Ltd.(c) (2015-2020)
 * FileName: session.h
 * Description: provide session operation interfaces 
 * Version: 1.0
 * Function Description:
 * History:
 * lilei add 2015/08/25
 */
#ifndef KBM_SESSION_H
#define	KBM_SESSION_H

#include <netinet/in.h>

/* define max len */
#define SESSION_IP_MAXLEN     INET6_ADDRSTRLEN
#define SESSION_SKEY_MAXLEN       32 
#define SESSION_ID_MAXLEN         64
#define SESSION_PWD_MAXLEN        32
#define SESSION_ADDR_MAXLEN		  100

/* define update flag */
#define SESSION_UPDATE_STATUS		(0x1 << 1)
#define SESSION_UPDATE_LAST_TIME 	(0x1 << 2)
#define SESSION_UPDATE_SKEY			(0x1 << 3)
#define SESSION_UPDATE_HOST			(0x1 << 5)
#define SESSION_UPDATE_SC			(0x1 << 6)
#define SESSION_UPDATE_CC			(0x1 << 7)
#define SESSION_UPDATE_PORT			(0x1 << 8)
#define SESSION_UPDATE_PEERADDR		(0x1 << 9)
#define SESSION_UPDATE_SVR_IP		(0x1 << 10)
#define SESSION_UPDATE_SVR_PORT		(0x1 << 11)

#define SESSION_QUERY_BY_SID	1
#define SESSION_QUERY_BY_UID_IP	2
#define SESSION_QUERY_BY_UID	3

#define SESSION_STATUS_VALIDING 	0
#define SESSION_STATUS_VALID		1
#define SESSION_STATUS_FORCE_OUT	2

#define ALG_SHA1 1

struct session_handle_s;
typedef struct session_handle_s *session_handle;

typedef  struct  session {
	unsigned long sid;
	int	status;
	int	create_time;
	int	last_time;
	int	skeylen; //不存到数据库
	char	 skey[SESSION_SKEY_MAXLEN];
	int	uid;
	char	 alias[SESSION_ID_MAXLEN];
	int	online;
	char	 host[SESSION_IP_MAXLEN];
	int	sc;
	int	cc;
	int	port;
	char peeraddr[SESSION_ADDR_MAXLEN];
	int	peeraddr_len; //不存到数据库
	char	svr_ip[SESSION_IP_MAXLEN];
	int	svr_port;
} session_t;

typedef struct	session_cfg {
	char	host[SESSION_IP_MAXLEN];
	int	port;
	char	username[SESSION_ID_MAXLEN];
	char	password[SESSION_PWD_MAXLEN];
} session_cfg_t;

int	session_init(session_handle *phandle, const session_cfg_t *cfg, int timeout, int alg);
int	session_add(session_handle handle, session_t *data);
int	session_update(session_handle handle, const session_t *session, long flag);
int	session_query(session_handle handle, session_t *session, long flag);
int	session_delete(session_handle handle, unsigned long sid);
int	session_delete_extend(session_handle handle, unsigned long sid, int uid);
int	session_force_out(session_handle handle, unsigned long sid, int uid);
int	session_clean_timeout(session_handle handle, int timeout);
int	session_exit(session_handle handle);
int auth_session(session_handle handle, const char *sid, int timestamp,
		int nonce, const unsigned char *sig);

#endif /* !KBM_SESSION_H */
