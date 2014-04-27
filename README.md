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

2014/4/24
封装了文件操作函数
实现了文件传输模块:列目录,上传,下载 (递归列目录考虑是否做)

2014/4/26
使用Qt 5.2.1 编写客户端程序.
修改了url上线的一些bug
进一步优化大小(WIN VC6,12K)

============

通信协议

按照格式发送命令字符串（'\0'结尾），server端在执行成功后会返回'#'，失败返回'!'

newconnect

	新建一个反弹连接，指定 域名：端口 或者 . 代表当前会话连接地址端口
	建立连接后，将连接作为会话处理。

	连接到当前会话 的地址：端口
	newconnect::.
	连接到 123.45.67.89:2000
	newconnect::123.45.67.89:2000 	
	建立新连接, 发送短语字符串
	newconnect::<Carry String>@.
	newconnect::<Carry String>@123.45.67.89:2000


reconnect

	协议类似newconnect，区别在于该命令要求连接到新的地址，并断开当前的连接


ioredirect

	开启一个进程，将进程的输入输出流重定向到会话，进程结束后当前会话连接会断开

	//将cmd.exe进程的输入输出流重定向到会话
	ioredirect::cmd.exe

file

	文件管理类命令，具有数个子命令

	上传文件命令
	file::put::<Path>
	执行该命令后,创建文件成功返回'#',失败返回'!',客户端发送格式为 <偏移量>:<长度>\0 的串给服务端，然后传输 格式给定长度的数据给服务端， 服务端将会把数据写入文件的指定偏移处,写入成功返回'#',失败返回'!'
	当格式串格式错误或者长度为0时，退出上传文件模式，不过不会关闭会话

	下载文件命令
	file::get::<Path>
	下载目标路径的文件,若文件打开成功 返回'#',失败返回'!',客户端发送格式为 <偏移量>:<长度>\0 的串给服务端, 服务端会在文件中从指定偏移中读取指定长度的数据，分数据包发回给客户端。
	当格式串格式错误或者长度为0时，退出下载文件模式，不过不会关闭会话
	发回数据包格式：4字节的网络字节序有符号整形数(读取成功长度)+读取成功的数据
	注意，发回的数据包是分块按顺序发回的

	列目录命令
	file::ls::<Path>
	列出指定路径的目录，若成功 返回'#'以及列出的目录字符串，若失败 返回'!'
	目录字符串格式：<<文件名>|<文件大小>|<文件属性值>|<访问时间>|<创建时间>|<修改时间>>\n (字串外部为尖括号)
	返回一个或多个目录字符串，目录字符串之间用'\n'分隔
	文件大小为uint64_t，时间为time_t值，可以使用ctime()函数转换

	删除文件(夹)
	file::rm::<Path>
	删除指定位置的文件或者空文件夹,成功 返回'#',失败返回'!'

	建立文件夹
	file::mkdir::<Path>
	在指定路径建立文件夹,成功 返回'#',失败返回'!'

control
	
	控制命令,目前只写了一个

	killyourself
	终结自身的进程


