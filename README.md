# MYLOG

1.支持日志级别控制
2.日志格式化
3.线程安全
4.日志输出方式
5.日志滚动（按文件大小，暂不支持按时间滚动）
6.日志异步写入
7.日志级别过滤
8.日志轮转（支持gzip压缩老文件）
9.json配置文件支持



　　一、日志功能
支持日志路径可配，不存在自动创建每个模块名的日志路径；
支持日志级别，按日志优先级打印；
支持打印多种输出方式（串口和网络方式输出待完善）；
支持日志文件大小和数量可配置，日志文件滚动；
支持日志输出超过磁盘限制大小改变输出方式；
支持按模块来打印日志，每个模块可独立配置和打印日志；
支持多线程输出日志。
　　二、日志优点
性能极高，内部无缓存，内存占用可忽略，对嵌入式开发极为友好；
可根据项目要求高度定制。
　　三、日志缺点
日志输出格式固定，不能配置（一般项目上配置好之后也不会轻易修改）；
模块内多线程打印有资源竞争时会等待锁，有一定的业务实时性影响。
