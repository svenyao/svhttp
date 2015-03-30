#include "bencode.h"
#include <stdlib.h>

namespace svhttp
{
#ifdef ENABLE_LIBICONV
	/**
	 *  convert(...) string封装
	 *  [in|out]put string : src_str
	 *  s[from|to]: 编码名称
	 */
	int convert_str(const std::string& sfrom, const std::string& sto, std::string& src_str)
	{
		int ilen = src_str.length();
		int ioutlen = ilen * 3;
		char *outbuf = (char *)malloc(ioutlen);
		memset(outbuf, 0, ioutlen);

		int nret = convert(sfrom.c_str(), sto.c_str(), outbuf, ioutlen, src_str.c_str(), ilen);
		if (nret < 0)
		{
			free(outbuf);
			outbuf = NULL;
			return nret;
		}

		outbuf[nret] = '\0';
		src_str = outbuf;
		free(outbuf);
		outbuf = NULL;

		return nret;
	}

	/**
	 *	对字符串进行语言编码转换
	 *	param from  原始编码，比如"GB2312",的按照iconv支持的写
	 *	param to    转换的目的编码
	 *	param save  转换后的数据保存到这个指针里，需要在外部分配内存
	 *	param savelen  存储转换后数据的内存大小
	 *	param src      原始需要转换的字符串
	 *	param srclen   原始字符串长度
	 */
	int convert(const char *from, const char *to, char* save, int savelen, const char *src, int srclen)
	{
		iconv_t cd;
		const char   *inbuf = src;
		char *outbuf = save;
		size_t outbufsize = savelen;
		int status = 0;
		size_t  savesize = 0;
		size_t inbufsize = srclen;
		const char* inptr = inbuf;
		size_t      insize = inbufsize;
		char* outptr = outbuf;
		size_t outsize = outbufsize;
    
		cd = iconv_open(to, from);
		iconv(cd, NULL, NULL, NULL, NULL);
		if (inbufsize == 0) {
			status = -1;
			goto done;
		}
		while (insize > 0) {
			size_t res = iconv(cd, (const char**)&inptr, &insize, &outptr, &outsize);
			if (outptr != outbuf) {
				int saved_errno = errno;
				int outsize = outptr - outbuf;
				strncpy(save+savesize, outbuf, outsize);
				errno = saved_errno;
			}
			if (res == (size_t)(-1)) {
				if (errno == EILSEQ) {
					int one = 1;
					iconvctl(cd, ICONV_SET_DISCARD_ILSEQ,&one);
					status = -3;
				} else if (errno == EINVAL) {
					if (inbufsize == 0) {
						status = -4;
						goto done;
					} else {
						break;
					}
				} else if (errno == E2BIG) {
					status = -5;
					goto done;
				} else {
					status = -6;
					goto done;
				}
			}
		}
		status = strlen(save);
	done:
		iconv_close(cd);
		return status;
	}

#endif

	/**
	 * Encodiert einen std::string base64
	 */
	std::string base64_encode(const std::string& toEncode) 
	{
		char* buf = 0;

		base64_encode(toEncode.c_str(), toEncode.size(), &buf);
		std::string result(buf);
		free(buf);

		return result;
	}

	/**
	 * Entfernt die base64 Maskierung von einem String
	 */
	std::string base64_decode(const std::string& toDecode) 
	{
		char* buf = (char*)malloc(sizeof(char)*toDecode.size());
		memset(buf, 0, sizeof(char)*toDecode.size());
	
		base64_decode(toDecode.c_str(), buf);
		std::string result(buf);
		free(buf);

		return result;
	}

	static char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	static int pos(char c)
	{
		char *p;
		for (p = base64_chars; *p; p++)
		if (*p == c)
			return p - base64_chars;
		return -1;
	}

	int base64_encode(const void *data, int size, char **str)
	{
		char *s, *p;
		int i;
		int c;
		const unsigned char *q;

		p = s = (char *) malloc(size * 4 / 3 + 4);
		if (p == NULL)
		return -1;
		q = (const unsigned char *) data;
		i = 0;
		for (i = 0; i < size;) {
		c = q[i++];
		c *= 256;
		if (i < size)
			c += q[i];
		i++;
		c *= 256;
		if (i < size)
			c += q[i];
		i++;
		p[0] = base64_chars[(c & 0x00fc0000) >> 18];
		p[1] = base64_chars[(c & 0x0003f000) >> 12];
		p[2] = base64_chars[(c & 0x00000fc0) >> 6];
		p[3] = base64_chars[(c & 0x0000003f) >> 0];
		if (i > size)
			p[3] = '=';
		if (i > size + 1)
			p[2] = '=';
		p += 4;
		}
		*p = 0;
		*str = s;
		return strlen(s);
	}

	#define DECODE_ERROR 0xffffffff

	static unsigned int	token_decode(const char *token)
	{
		int i;
		unsigned int val = 0;
		int marker = 0;
		if (strlen(token) < 4)
		return DECODE_ERROR;
		for (i = 0; i < 4; i++) {
		val *= 64;
		if (token[i] == '=')
			marker++;
		else if (marker > 0)
			return DECODE_ERROR;
		else
			val += pos(token[i]);
		}
		if (marker > 2)
		return DECODE_ERROR;
		return (marker << 24) | val;
	}

	int base64_decode(const char *str, void *data)
	{
		const char *p;
		unsigned char *q;

		q = (unsigned char*)data;
		for (p = str; *p && (*p == '=' || strchr(base64_chars, *p)); p += 4) {
		unsigned int val = token_decode(p);
		unsigned int marker = (val >> 24) & 0xff;
		if (val == DECODE_ERROR)
			return -1;
		*q++ = (val >> 16) & 0xff;
		if (marker < 2)
			*q++ = (val >> 8) & 0xff;
		if (marker < 1)
			*q++ = val & 0xff;
		}
		return q - (unsigned char *) data;
	}


} //@ end namespace svhttp.
