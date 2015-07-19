======

svhttp是一个基于libcurl实现的HTTP (c++)库.

-

##### 简易上手

``` c++
#include <iostream>
#include "svhttp.h"

int main(int* argc, char** argv)
{
	svhttp::http_client _client;
	_client.open("http://1111.ip138.com/ic.asp");
	
	std::cout << _client.read_some();
	
	return 0;
}
```
-

##### 使用request_header定制HTTP请求

``` c++
#include <iostream>
#include "svhttp.h"

int main(int* argc, char** argv)
{
	svhttp::http_client _client;
	
	svhttp::request_header opts;
	opts.insert(std::string("Accept: application/javascript, */*;q=0.8"));
	opts.insert(std::string("Connection: Keep-Alive"));
	_client.set_headers(opts);
	
	_client.open("http://1111.ip138.com/ic.asp");
	
	std::cout << _client.read_some();
	
	return 0;
}
``` 

-

##### post数据提交

``` c++
#include <iostream>
#include "svhttp.h"

int main(int* argc, char** argv)
{
	svhttp::http_client _client;
	
	std::string post_str = "post string.";
	_client.set_post_fields(post_str);
	
	_client.open("http://1111.ip138.com/ic.asp");
	
	std::cout << _client.read_some();
	
	return 0;
}
```

-

##### 设置代理

``` c++
#include <iostream>
#include "svhttp.h"

int main(int* argc, char** argv)
{
	svhttp::http_client _client;
	// 设置代理
	_client.set_proxy(std::string("127.0.0.1:808"));
	
	_client.open("http://1111.ip138.com/ic.asp");
	
	std::cout << _client.read_some();
	
	return 0;
}
```

更多代理设置项请参照svhttp源码。

-

##### 克隆代码

使用 git 获取 svhttp 项目源代码:

    $ git clone git://github.com/byxing/svhttp
    $ cd svhttp

克隆子模块项目:

    $ git submodule init
    $ git submodule update

更新 svhttp 项目代码:

    $ cd svhttp
    $ git pull -f

-

##### svhttp使用解决方案配置示例解析[以vs2012为例,其中目录路径需按实际解决方案修改,其他平台使用请参照之]

	[VS2012 解决方案配置]

	<svhttp库配置>：
	C/C++ -> 常规 -> 附加包含目录
	../../external/libcurl/include;
	../../svhttp;
	链接器 -> 常规 -> 附加库目录
	../../external/libcurl/build;
	链接器 -> 输入 -> 附加依赖项
	libcurl_imp.lib;

	<svhttp支持iconv编码库配置>：
	C/C++ -> 常规 -> 附加包含目录
	../../external/libiconv/include;
	链接器 -> 常规 -> 附加库目录
	../../external/libiconv/build/vc11;
	链接器 -> 输入 -> 附加依赖项
	libiconvd.lib;

	如svhttp需要使用iconv编码库支持，更新github子模块项目并且按上述配置iconv编码库支持，
	如不需要支持则不做此操作。

	若项目需要开启iconv编码库支持，还需在项目引入svhttp库前加上启用宏，代码如下：
	
	#define ENABLE_LIBICONV
	#include "svhttp.h"

	或者在项目属性的预处理宏中添加 ENABLE_LIBICONV;
	
-

更多svhttp使用方法，请参照svhttp源码。

fixed: 此库已实现为 HEADER_ONLY 模式。