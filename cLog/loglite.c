 /**
 * Copyright (c) 2024 
 *     All rights reserved.
 *
 * File: loglite.c
 * Author: 
 * Time: 2024/03
 * Version: 1.0.0
 * 
 * Loglite interface implementation
 *
 */
 #include "loglite.h"
 
 #include <stdint.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <stdarg.h>
 #include <unistd.h>
 #include <dirent.h>
 #include <sys/stat.h>
 #include <sys/statvfs.h>
 #include <sys/time.h>
 #include <time.h>
 #include <pthread.h>
 
 #define PATH_MAX_LEN 200U
 #define FILE_MAX_LEN 256U
 #define BINARY_UNIT 1000U
 
 typedef enum {
     LOG_OFF = 0,
     LOG_CONSOLE = 1,
     LOG_FILE = 2,
     LOG_SERIAL = 4,
     LOG_NET = 8,
 } LOG_OUTPUT;
 
 typedef struct {
     char path[PATH_MAX_LEN];
     uint16_t output_flush;
     uint16_t file_count;
     uint16_t file_size;
     uint32_t reserve_space;
 } GlobalLogParam;
 
 typedef struct {
     FILE* file;
     char  name[16];
     uint16_t file_num;
     LOG_LEVEL  level;
     LOG_OUTPUT output;
     pthread_mutex_t mutex;
 } ModuleLogParam;
 
 static GlobalLogParam g_global_param = {
     "./log", 1, 5, 30, 100
 };
 
 static ModuleLogParam g_module_param[MODULE_ID_MAX] = {
 /*    { NULL, "A", 5, LOG_INFO, LOG_FILE, PTHREAD_MUTEX_INITIALIZER },
     { NULL, "B", 0, LOG_INFO, LOG_FILE, PTHREAD_MUTEX_INITIALIZER },
     { NULL, "C", 0, LOG_INFO, LOG_FILE, PTHREAD_MUTEX_INITIALIZER },
     { NULL, "D", 0, LOG_INFO, LOG_FILE, PTHREAD_MUTEX_INITIALIZER },
 */
     { NULL, "defultname", 0, LOG_INFO, LOG_FILE, PTHREAD_MUTEX_INITIALIZER }
 };
 
 static char const* const g_log_level[LOG_LEVEL_MAX] = {
     "TRACE", "DEBUG", "INFO", "WRAN", "ERROR", "FATAL", "CLOSE"
 };
 
/*
 // remove spaces and endl
 static void trim(char* str)
 {
     char* start = str;
     char* end = str;
 
     while (' ' == *start) {
         start++;
     }
 
     while (*end != '\r' && *end != '\n' && *end != '\0') {
         end++;
     }
 
     int i = 0;
     for (; start < end; i++, start++) {
         str[i] = *start;
     }
 
     str[i] = '\0';
 }
 
 // Reading the configuration of numerical types
 static void get_config_number(const char* data, const char* name, void* dst, uint16_t width)
 {
     if (strstr(data, name) != NULL) return;

         char* pos = (char*)strstr(data, "=");
         if (pos != NULL)
         {
             pos += 1;
             trim(pos);
             
             char* endptr;
             long value = strtol(pos, &endptr, 10);
             if ('\0' == *endptr)
             {
                 memcpy(dst, &value, width);
             }
         }
 }
 
 // Reading the configuration of string types
 static void get_config_string(const char* data, const char* name, char* dst, uint16_t length)
 {
     if (strstr(data, name) == NULL) return;
     
         const char* posBeg = strstr(data, "\"");
         const char* posEnd = strrchr(data, '\"');
         if (posBeg != NULL && posEnd != NULL)
         {
             size_t size = posEnd - posBeg - 1U;
             size = size > length ? length : size;
             strncpy(dst, posBeg + 1U, size);
         }
 }
 */

 
 static char* get_log_time(char time[32])
 {
     struct timeval tm;
     gettimeofday(&tm, NULL);
     
     struct tm* ptm = localtime(&tm.tv_sec);
     strftime(time, 27, "%Y-%m-%d %H:%M:%S", ptm);
     
     uint32_t msec = tm.tv_usec / 1000U;
     sprintf(&time[strlen(time)], ".%03d", msec);
     
     return time;
 }
 
 static void rolling_log_file(MODULE_ID id)
 {
     // 1.close logfile
     if (g_module_param[id].file != NULL)
     {
         fflush(g_module_param[id].file);
         fclose(g_module_param[id].file);
         g_module_param[id].file = NULL;
     }
     
     // 2.remove excess logfile
     if (g_module_param[id].file_num == g_global_param.file_count)
     {
         char oldfile[FILE_MAX_LEN] = { 0 };
         if (0 == g_global_param.file_count)
         {
             snprintf(oldfile, sizeof(oldfile), "%s/%s.log",
                 g_global_param.path, g_module_param[id].name);
         }
         else
         {
             snprintf(oldfile, sizeof(oldfile), "%s/%s_%d.log",
                 g_global_param.path, g_module_param[id].name, g_global_param.file_count);
         }
         
         if (0 == access(oldfile, F_OK))
         {
             if (0 != remove(oldfile))
             {
                 printf("logfile: %s remove failed\n", oldfile);
             }
         }
     }
     
     // 3.rolling logfile
     for (uint16_t i = g_module_param[id].file_num; i > 0; i--)
     {
         char oldname[FILE_MAX_LEN] = { 0 };
         char newname[FILE_MAX_LEN] = { 0 };
         
         snprintf(oldname, sizeof(oldname), "%s/%s_%d.log",
             g_global_param.path, g_module_param[id].name, i);
         snprintf(newname, sizeof(newname), "%s/%s_%d.log",
             g_global_param.path, g_module_param[id].name, i+1);
                 
         if (0 == access(oldname, F_OK))
         {
             if (0 != rename(oldname, newname))
             {
                 printf("logfile: %s rename %s failed\n", oldname, newname);
             }
         }
     }
     
     // 4.rolling self
     if (g_global_param.file_count > 0)
     {
         char oldname[FILE_MAX_LEN] = { 0 };
         char newname[FILE_MAX_LEN] = { 0 };
         
         snprintf(oldname, sizeof(oldname), "%s/%s/%s.log",g_global_param.path,g_module_param[id].name, g_module_param[id].name);
         snprintf(newname, sizeof(newname), "%s/%s/%s_1.log",g_global_param.path,g_module_param[id].name, g_module_param[id].name);
                 
         if (0 == access(oldname, F_OK))
         {
             if (0 != rename(oldname, newname))
             {
                 printf("logfile: %s rename %s failed\n", oldname, newname);
             }
         }
     }
     
     // 5.file count++
     if (g_module_param[id].file_num < g_global_param.file_count)
     {
         g_module_param[id].file_num++;
     }
 }
 
 static void open_log_file(MODULE_ID id)
 {
     char file[FILE_MAX_LEN] = { 0 };
     snprintf(file, sizeof(file), "%s/%s/%s.log",
         g_global_param.path,g_module_param[id].name, g_module_param[id].name);
         
     g_module_param[id].file = fopen(file, "a");
     if (g_module_param[id].file != NULL)
     {
         if (g_global_param.output_flush)
         {
             // 遇到换行或者缓冲满时写入
             setvbuf(g_module_param[id].file, NULL, _IOLBF, 0);
         }
     }
     else
     {
         printf("logfile: %s open failed\n", file);
     }
 }
 
 static void check_log_file(MODULE_ID id)
 {
     fseek(g_module_param[id].file,0,SEEK_END);
     size_t fileSize = ftell(g_module_param[id].file);
     fseek(g_module_param[id].file,0,SEEK_SET);
   //  long fileSize = ftell(g_module_param[id].file);
     if (fileSize > g_global_param.file_size * BINARY_UNIT * BINARY_UNIT)
     {
         struct statvfs fs;
         if (0 == statvfs(g_global_param.path, &fs))
         {
             unsigned long freeSpace = fs.f_bavail * fs.f_frsize;
             if (freeSpace / (BINARY_UNIT * BINARY_UNIT) < g_global_param.reserve_space)
             {
                 fputs("The disk will be full, and log switch console output\n", 
                     g_module_param[id].file);
                 g_module_param[id].output = LOG_CONSOLE;
             }
         }
         
         rolling_log_file(id);
     }
 }
 
#define CONFIG_FILE "log.conf"
 static int init_log_config(const char* config)
 {
	printf("--%s--",config);
     int result = 0;
     char* buffer = NULL; 
     FILE* file = fopen(CONFIG_FILE,"r");
     if ( NULL == file) {
	     return -1;
     }

     fseek(file,0,SEEK_END);
     size_t  length = ftell(file);
     fseek(file,0,SEEK_SET);

     buffer = (char*)malloc(length+1);
     if ( NULL == buffer ) {
	     fclose(file);
	     return -1;
     }

    size_t read_size = fread(buffer,1,length,file);
     if ( read_size != length ) {
	     fclose(file);
	     free(buffer);
	     return -1;
     }

     buffer[length] = '\0';
     fclose(file);

     cJSON* json = cJSON_Parse(buffer);
     if ( !json ) {
	     free(buffer);
	     return -1;
     }

     cJSON* log_path = cJSON_GetObjectItemCaseSensitive(json,"log_path");
     strcpy(g_global_param.path,log_path->valuestring);

     cJSON *log_flush = cJSON_GetObjectItemCaseSensitive(json, "log_flush");
     g_global_param.output_flush = log_flush->valueint;


     cJSON *log_file_count = cJSON_GetObjectItemCaseSensitive(json,"log_file_count");
     g_global_param.file_count = log_file_count->valueint;

     cJSON *log_file_size = cJSON_GetObjectItemCaseSensitive(json,"log_file_size");
     g_global_param.file_size = log_file_size->valueint;

     cJSON *log_reserve_space = cJSON_GetObjectItemCaseSensitive(json,"log_reserve_space");
     g_global_param.reserve_space = log_reserve_space->valueint;

     /*
     cJSON *log_A_level = cJSON_GetObjectItemCaseSensitive(json, "log_A_level");
     g_module_param[MODULE_A].level = log_A_level->valueint;

     cJSON *log_B_level = cJSON_GetObjectItemCaseSensitive(json, "log_B_level");
     g_module_param[MODULE_B].level = log_B_level->valueint;

     cJSON *log_C_level = cJSON_GetObjectItemCaseSensitive(json, "log_C_level");
     g_module_param[MODULE_C].level = log_C_level->valueint;

     cJSON *log_D_level = cJSON_GetObjectItemCaseSensitive(json, "log_D_level");
     g_module_param[MODULE_D].level = log_D_level->valueint;

     cJSON *log_A_output = cJSON_GetObjectItemCaseSensitive(json, "log_A_output");
     g_module_param[MODULE_A].output = log_A_output->valueint;

     cJSON *log_B_output = cJSON_GetObjectItemCaseSensitive(json, "log_B_output");
     g_module_param[MODULE_B].output = log_B_output->valueint;

     cJSON *log_C_output = cJSON_GetObjectItemCaseSensitive(json, "log_C_output");
     g_module_param[MODULE_C].output = log_C_output->valueint;

     cJSON *log_D_output = cJSON_GetObjectItemCaseSensitive(json, "log_D_output");
     g_module_param[MODULE_D].output = log_D_output->valueint;
     */

     free(buffer);
     cJSON_Delete(json);

     return result;
    /*
     
     do {
         if (NULL = config)
         {
             printf("init log config failed, file is null\n");
             result = -1;
         }
         
         FILE *cfg = fopen(config, "r");
         if (NULL == cfg)
         {
             printf("init log config failed, open %s error\n", config);
             result = -1;
         }
     
         char line[512] = { 0 };
         while (fgets(line, sizeof(line), cfg) != NULL)
         {
             trim(line);
             if (line[0] != '#')
             {
                 //get_config_string(line, "log_path", g_global_param.path, sizeof(g_global_param.path));
//                 get_config_number(line, "log_flush", &g_global_param.output_flush, sizeof(g_global_param.output_flush));
//                 get_config_number(line, "log_file_count", &g_global_param.file_count, sizeof(g_global_param.file_count));
//                 get_config_number(line, "log_file_size", &g_global_param.file_size, sizeof(g_global_param.file_size));
//                 get_config_number(line, "log_reserve_space", &g_global_param.reserve_space, sizeof(g_global_param.reserve_space));
                 
                 get_config_number(line, "log_A_level",  &g_module_param[MODULE_A].level,  sizeof(g_module_param[MODULE_A].level));
                 get_config_number(line, "log_B_level",  &g_module_param[MODULE_B].level,  sizeof(g_module_param[MODULE_B].level));
                 get_config_number(line, "log_C_level",  &g_module_param[MODULE_C].level,  sizeof(g_module_param[MODULE_C].level));
                 get_config_number(line, "log_D_level",  &g_module_param[MODULE_D].level,  sizeof(g_module_param[MODULE_D].level));
                 get_config_number(line, "log_A_output", &g_module_param[MODULE_A].output, sizeof(g_module_param[MODULE_A].output));
                 get_config_number(line, "log_B_output", &g_module_param[MODULE_B].output, sizeof(g_module_param[MODULE_B].output));
                 get_config_number(line, "log_C_output", &g_module_param[MODULE_C].output, sizeof(g_module_param[MODULE_C].output));
                 get_config_number(line, "log_D_output", &g_module_param[MODULE_D].output, sizeof(g_module_param[MODULE_D].output));
             }
         }
     } while(0);
     
     return result;
     */
 }
 
 static int check_log_config(void)
 {
     int result = 0;
     
     for (uint16_t i = 0; i < MODULE_ID_MAX; i++)
     {
         if (g_module_param[i].level < 0 || g_module_param[i].level >= LOG_LEVEL_MAX)
         {
             printf("%s log level config is invalid\n", g_module_param[i].name);
             result = -1;
             break;
         }
         
         if (g_module_param[i].output < 0 || g_module_param[i].output > 15)
         {
             printf("%s log output config is invalid\n", g_module_param[i].name);
             result = -1;
             break;
         }
         
         if ((g_module_param[i].output & LOG_FILE) && g_global_param.file_size == 0)
         {
             printf("log file size config is invalid\n");
             result = -1;
             break;
         }
     }
     
     return result;
 }
 
/*
 static int init_log_path(void)
 {
     int result = 0;
     
     do {
         if (0 == access(g_global_param.path, F_OK))
         {
             break;
         }
         
         for (uint16_t i = 0; g_global_param.path[i] != '\0'; i++)
         {
             if ('/' == g_global_param.path[i])
             {
                 g_global_param.path[i] = '\0';
                 if (g_global_param.path[0] != '\0'
                     && 0 != strcmp(g_global_param.path, ".")
                     && 0 != access(g_global_param.path, F_OK))
                 {
                     if (0 != mkdir(g_global_param.path, 0777))
                     {
                         printf("logpath: %s mkdir failed\n", g_global_param.path);
                         g_global_param.path[i] = '/';
                         result = -1;
                         break;
                     }
		     else {
                         printf("logpath: %s mkdir succeed\n", g_global_param.path);
		     }
                 }
                 
                 g_global_param.path[i] = '/';
             }
         }
         
         if (0 == result)
         {
             size_t len = strlen(g_global_param.path);
             if (g_global_param.path[len - 1] != '/')
             {
                 if (0 != mkdir(g_global_param.path, 0777))
                 {
                     printf("logpath: %s mkdir failed\n", g_global_param.path);
                     result = -1;
                 }
		 else {
                     printf("logpath: %s mkdir succeed\n", g_global_param.path);
		 }
             }
         }
         
     } while (0);
     
     return result;
 }
*/

int init_log_path(void)
{
    char DirName[256];

    for (int i=0;i<MODULE_ID_MAX;++i) {
        memset(DirName,0,sizeof(DirName));   
      	strcpy(DirName, g_global_param.path);
    	int len1 = strlen(DirName);
        if (DirName[len1] != '/')
       		strcat(DirName, "/");
    	strcat(DirName,g_module_param[i].name);
    	int len2 = strlen(DirName);
    	if (DirName[len2] != '/')
        	strcat(DirName, "/");
    	for (int i = 1; i < len2+1; i++) {
        	if (DirName[i] == '/') {
            	DirName[i] = 0;
            	if (access(DirName, F_OK) != 0) {
                	if (mkdir(DirName, 0755) == -1) {
                    	perror("mkdir error");
                    	return -1;
                	}
            	}
            	DirName[i] = '/';
        	}
    	}
    }
    
    return 0;
} 
 
 static int init_log_file_number(void)
 {
     int result = 0;
     
     DIR* dp = opendir(g_global_param.path);
     if (NULL == dp)
     {
         printf("logpath: %s open failed\n", g_global_param.path);
         result = -1;
	 return result;
     }
     else
     {
         struct dirent* entry = NULL;
         while ((entry = readdir(dp)) != NULL)
         {
             char* name = entry->d_name;
             if (0 == strcmp(name, ".") || 0 == strcmp(name, ".."))
                 continue;
             
             for (uint16_t i = 0; i < MODULE_ID_MAX; i++)
             {
                 if (strstr(name, g_module_param[i].name) != NULL)
                 {
                     char head[20] = { 0 };
                     size_t headlen = strlen(g_module_param[i].name);
                     strncpy(head, g_module_param[i].name, sizeof(head));
                     head[headlen] = '_';
                     head[headlen+1] = '\0';
                     
                     char* posBeg = strstr(name, head);
                     char* posEnd = strstr(name, ".log");
                     if (posBeg != NULL && posEnd != NULL)
                     {
                         char value[16] = { 0 };
                         posBeg = posBeg + headlen + 1;
                         strncpy(value, posBeg, posEnd-posBeg);
                         
                         char* endptr;
                         long total = strtol(value, &endptr, 10);
                         if (*endptr == '\0')
                         {
                             unsigned char num = total;
                             if (num > g_module_param[i].file_num)
                             {
                                 g_module_param[i].file_num = num;
                             }
                             
                             // 配置改小时，上次多余的日志文件不主动移除
                             if (g_module_param[i].file_num > g_global_param.file_count)
                             {
                                 g_module_param[i].file_num = g_global_param.file_count;
                             }
                         }
                     }
                 }
             }
         }
     }
     
     closedir(dp);
     return result;
 }
 
 int log_init(const char* config,const char* moduleName)
 {
     const char* exeName = strrchr(moduleName,'/');
     const char* mName = (exeName != NULL) ? exeName+1 : moduleName;

     strcpy(g_module_param[APPNAME].name,mName);
     // 1.init log config
     int result = init_log_config(config);
     
     // 2.check log config
     if (0 == result)
     {
         result = check_log_config();
     }
     
     // 3.init log path
     if (0 == result)
     {
         result = init_log_path();
     }
     
     // 4.init log file number
     if (0 == result)
     {
         result = init_log_file_number();
     }
     
     return result;
 }
 
 void logging(LOG_LEVEL level, MODULE_ID id, const char* file, int line, 
     const char* func, const char* format, const char* suffix, ...)
 {
     if (g_module_param[id].level <= level
         && g_module_param[id].output != LOG_OFF
         && format != NULL && suffix != NULL)
     {
         char tm[32] = { 0 };
         get_log_time(tm);
         pthread_t tid = pthread_self();
         
         pthread_mutex_lock(&g_module_param[id].mutex);
         
         if (g_module_param[id].output & LOG_CONSOLE)
         {
             printf("[%s] [%-5s] [%lu] [%s:%d %s] ",
                 tm, g_log_level[level], tid, file, line, func);
             
             va_list args;
             va_start(args, suffix);
             vprintf(format, args);
             va_end(args);
             
             printf("%s", suffix);
         }
         
         if (g_module_param[id].output & LOG_FILE)
         {
             if (NULL == g_module_param[id].file)
             {
                 open_log_file(id);
             }
             
             if (NULL != g_module_param[id].file)
             {
                 fprintf(g_module_param[id].file, "[%s] [%-5s] [%lu] [%s:%d %s] ",
                     tm, g_log_level[level], tid, file, line, func);
                 
                 va_list args;
                 va_start(args, suffix);
                 vfprintf(g_module_param[id].file, format, args);
                 va_end(args);
                 
                 fprintf(g_module_param[id].file, "%s", suffix);
                 
                 check_log_file(id);
             }
         }
         
         pthread_mutex_unlock(&g_module_param[id].mutex);
     }
 }
 
 int log_drop(void)
 {
     for (uint16_t i = 0; i < MODULE_ID_MAX; i++)
     {
         if (g_module_param[i].file != NULL)
         {
             fflush(g_module_param[i].file);
             fclose(g_module_param[i].file);
             g_module_param[i].file = NULL;
             g_module_param[i].output = LOG_OFF;
         }
     }
     
     return 0;
 }
