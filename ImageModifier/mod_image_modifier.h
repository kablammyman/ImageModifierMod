#ifndef MOD_FOO_HPP
#define MOD_FOO_HPP

#ifdef __cplusplus
#define EXTERN_C_BLOCK_BEGIN    extern "C" {
#define EXTERN_C_BLOCK_END      }
#define EXTERN_C_FUNC           extern "C"
#else
#define EXTERN_C_BLOCK_BEGIN
#define EXTERN_C_BLOCK_END
#define EXTERN_C_FUNC
#endif

#include <httpd.h>
#include <http_protocol.h>
#include <http_config.h>
#include "ap_config.h"
#include "apr_optional.h"
#include <http_log.h>
#include "apr_general.h"
#include "apr_getopt.h"
#include "apr_xml.h"
#include "apr_file_io.h"
#endif	/* MOD_FOO_HPP */