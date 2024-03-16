 /**
   * Copyright (c) 2023 yuwanxian
   *     All rights reserved.
   *
   * File: loglite.h
   * Author: yuwanxian
   * Time: 2023/07/07
   * Version: 1.0.0
   *
 * Loglite interface declaration
 *
 */
 
 #ifndef _LOGLITE_H_
 #define _LOGLITE_H_
 
 #include <string.h>
 #include "cJSON.h"
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 
 /**
 * @brief 模块ID
 */
 typedef enum {
     MODULE_A,
     MODULE_B,
     MODULE_C,
     MODULE_D,
     MODULE_ID_MAX
 } MODULE_ID;
 
 /**
 * @brief 日志等级
 */
 typedef enum {
     LOG_TRACE,
     LOG_DEBUG,
     LOG_INFO,
     LOG_WARN,
     LOG_ERROR,
     LOG_FATAL,
     LOG_CLOSE,
     LOG_LEVEL_MAX
 } LOG_LEVEL;
 
 /**
 * @brief 日志初始化
 *
 * @param config [in] 日志配置文件路径
 *
 * @return int 0：成功，其它：失败
 */
 int log_init(const char* config);
 
 /**
 * @brief 日志输出
 *
 * @param level  [in] 日志等级
 * @param id     [in] 日志模块ID
 * @param file   [in] 日志输出时所在文件名
 * @param line   [in] 日志输出时所在代码行数
 * @param func   [in] 日志输出时所在函数名
 * @param format [in] 日志格式化字符串
 * @param suffix [in] 日志后缀，可自定义
 * @param ...    [in] 日志格式化可变参数
 *
 * @return void
 */
 void logging(LOG_LEVEL level, MODULE_ID id, const char* file, int line, 
     const char* func, const char* format, const char* suffix, ...);
 
 /**
 * @brief 日志结束
 *
 * @param void
 *
 * @return int 0：成功，其它：失败
 */
 int log_drop(void);
 
 
 /**
 * @brief 日志打印宏，子模块可根据需要再定义
 *
 * @example 参考子模块自定义日志打印宏
 */
 #define LOG_FILE_NAME (strrchr(__FILE__, '/')+1)
 //#define LOG_VAARGS_EX LOG_FILE_NAME, __LINE__, __func__
 #define LOG_VAARGS_EX __FILE__,__LINE__, __func__
 
 #define LOGT(id, format, suffix, ...)\
         logging(LOG_TRACE, id, LOG_VAARGS_EX, format, suffix, ##__VA_ARGS__)
 
 #define LOGD(id, format, suffix, ...)\
         logging(LOG_DEBUG, id, LOG_VAARGS_EX, format, suffix, ##__VA_ARGS__)
 
 #define LOGI(id, format, suffix, ...)\
         logging(LOG_INFO,  id, LOG_VAARGS_EX, format, suffix, ##__VA_ARGS__)
 
 #define LOGW(id, format, suffix, ...)\
         logging(LOG_WARN,  id, LOG_VAARGS_EX, format, suffix, ##__VA_ARGS__)
 
 #define LOGE(id, format, suffix, ...)\
         logging(LOG_ERROR, id, LOG_VAARGS_EX, format, suffix, ##__VA_ARGS__)
         
 #define LOGF(id, format, suffix, ...)\
         logging(LOG_FATAL, id, LOG_VAARGS_EX, format, suffix, ##__VA_ARGS__)
 
 /**
 * @brief 子模块自定义日志打印宏
 *
 * @note 注意：1.日志自带换行，2.结尾自带分号
 */
 #define A_LOGT(format, ...) LOGT(MODULE_A, format, "\n", ##__VA_ARGS__);
 #define A_LOGD(format, ...) LOGD(MODULE_A, format, "\n", ##__VA_ARGS__);
 #define A_LOGI(format, ...) LOGI(MODULE_A, format, "\n", ##__VA_ARGS__);
 #define A_LOGW(format, ...) LOGW(MODULE_A, format, "\n", ##__VA_ARGS__);
 #define A_LOGE(format, ...) LOGE(MODULE_A, format, "\n", ##__VA_ARGS__);
 #define A_LOGF(format, ...) LOGF(MODULE_A, format, "\n", ##__VA_ARGS__);
 
 
 #ifdef __cplusplus
 }
 #endif
 
 #endif // _LOGLITE_H_

