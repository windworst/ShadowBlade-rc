ShadowBlade-rc
==============

Blade in Shadow

2014/4/10
千里之行,始于足下.

2014/4/12
通信基于TCP协议,正向和反向连接
完成了:
新连接创建功能
输入输出重定向功能
反弹端口上线
HTTP页面上线

2014/4/20
修复了一些Bug
修改了newconnect功能携带字符串功能
加入control命令(用来控制主机)
加入正向连接功能
优化了线程模块

2014/4/21
加入AES256和MD5模块
引入新的编译环境: CodeBlocks+VC6(cl.exe),为了减小exe体积

2014/4/22
移除了AES256模块,替换为RC4加密模块
修改了session机制,加入session_context 处理session
为了迎合新的session机制大量修改代码
加入file (文件管理功能) 不过未动笔
