#include "tools.h"
#include "aes/aes.h"
#include "md5/md5.h"

#define AES_KEY_SIZE 256
#define AES_KEY_BTYE 32
#define AES_BUFFER_LEN 16

_CANNP_BEGIN

void encrypt::EncryptMD5(const char* src, uint8 len, std::string& strOut)
{
	strOut.clear();
	md5_state_t state;
	memset(&state, 0, sizeof(state));
	md5_byte_t digest[16] = { 0 };
	char hex_output[16 * 2 + 1] = "";

	md5_init(&state);
	md5_append(&state, (md5_byte_t*)src, len);
	md5_finish(&state, digest);
	for (int di = 0; di < 16; ++di)
		sprintf_s(hex_output + di * 2, 2 * sizeof(md5_byte_t) + 1, "%02x", digest[di]);

	strOut.append(hex_output);
}

bool encrypt::EncryptAES(const char* src, const char* pwd, std::string& strOut)
{	
	strOut.clear();
	aes_byte outtex[AES_BUFFER_LEN + 1] = "";
	aes_ctx ctx;
	aes_set_key(&ctx, (aes_byte*)pwd, AES_KEY_BTYE, aes_enc);
	if(aes_encrypt(&ctx, (aes_byte*)src, outtex) == aes_good)
		strOut.append((char*)outtex);

	return strOut.size()>0 ;
}

bool encrypt::DecryptAES(const char* ciphertext, const char* pwd, std::string& strOut)
{
	strOut.clear();
	aes_byte outtex[AES_BUFFER_LEN + 1] = "";
	aes_ctx ctx;
	aes_set_key(&ctx, (aes_byte*)pwd, AES_KEY_BTYE, aes_dec);
	if(aes_decrypt(&ctx, (aes_byte*)ciphertext, outtex) == aes_good)
		strOut.append((char*)outtex);

	return strOut.size() > 0;
}

void encrypt::EncryptBuffer(const char* src, const uint32& iLen, const char* pwd, std::string&strOut)
{
	strOut.clear();
	uint32 iIndex(0);
	uint8 iLenH(AES_BUFFER_LEN);
	bool isDo(true);

	aes_byte outtex[AES_BUFFER_LEN + 1] = "";
	aes_ctx ctx;
	aes_set_key(&ctx, (aes_byte*)pwd, AES_KEY_BTYE, aes_enc);

	while (isDo)
	{
		if (iLen - iIndex <= AES_BUFFER_LEN)
		{
			iLenH = iLen - iIndex;
			isDo = false;
		}

		if (aes_encrypt(&ctx, (aes_byte*)&src[iIndex], outtex) == aes_good)
			strOut.append((char*)outtex, AES_BUFFER_LEN);
		else
			strOut.append(&src[iIndex], iLenH);

		iIndex += iLenH;
	}
}

void encrypt::DecryptBuffer(const char* src, const uint32& iLen, const char* pwd, std::string&strOut)
{
	strOut.clear();
	uint32 iIndex(0);
	uint8 iLenH(AES_BUFFER_LEN);
	bool isDo(true);

	aes_byte outtex[AES_BUFFER_LEN + 1] = "";
	aes_ctx ctx;
	aes_set_key(&ctx, (aes_byte*)pwd, AES_KEY_BTYE, aes_dec);
	while (isDo)
	{
		if (iLen - iIndex <= AES_BUFFER_LEN)
		{
			iLenH = iLen - iIndex;
			isDo = false;
		}

		if (aes_decrypt(&ctx, (aes_byte*)&src[iIndex], outtex) == aes_good)
			strOut.append((char*)outtex, iLenH);
		else
			strOut.append(&src[iIndex], iLenH);

		iIndex += iLenH;
	}
}

_CANNP_END