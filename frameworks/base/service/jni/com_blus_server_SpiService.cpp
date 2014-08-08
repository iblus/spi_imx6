/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 */

#define LOG_TAG "BlusPlatform"
#include "utils/Log.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include <android/log.h>
#include "JNIHelp.h"
#include "utils/misc.h"

#include <jni.h>
#include <spi.h>

#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__) 
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG , LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO  , LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN  , LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , LOG_TAG, __VA_ARGS__)


using namespace android;

#define BUF_MAX_SIZE	0x1000
// ----------------------------------------------------------------------------
//static char RBuffer[BUF_MAX_SIZE];
//static char TBuffer[BUF_MAX_SIZE];
struct spi_control_device_t *sSpiDevice = NULL;
/*
 * Class:     SpiService
 * Method:    _open
 * Signature: (I)I
 */
static jint spi_open(JNIEnv* env, jclass thiz, jint Spi)
{

	int no = Spi;
	LOGI("SpiService JNI: spi_open() is invoked.");

	if (sSpiDevice == NULL)
	{
		LOGI("SpiService JNI: sSpiDevice was not fetched correctly.");
		return -1;
	} else
	{
		return sSpiDevice->open(no);
	}
}
/*
 * Class:     SpiService
 * Method:    _config
 * Signature: (IIII)Z
 */
static jboolean spi_config(JNIEnv* env, jclass thiz, jint fd, jint _speed, jint _bit_per_word, jint _mode)
{

	LOGI("SpiService JNI: spi_config() is invoked.");

	if (sSpiDevice == NULL)
	{
		LOGI("SpiService JNI: sSpiDevice was not fetched correctly.");
		return false;
	} else
	{
		if (sSpiDevice->config(fd, _speed, _bit_per_word, _mode) == 0)
		{
			return true;
		} else
		{
			return false;
		}
	}
}
/*
 * Class:     SpiService
 * Method:    _transfer
 * Signature: (I[Ljava/lang/String;[Ljava/lang/String;I)I
 */
static jint spi_transfer(JNIEnv* env, jclass thiz, jint _fd, jobjectArray _tbuf, jobjectArray _rbuf, jint _len)
{

	int i = 0;
	int fd = _fd;
	int len = _len;
	int data = 0;
	char *pTbuff = 0;
	char *pTbuff2 = 0;
	char *pRbuff = 0;
	jstring jstrTmp = NULL;
	int nArrLen = env->GetArrayLength( _tbuf);

	LOGI("SpiService JNI: spi_transfer() is invoked.");

	if (sSpiDevice == NULL)
	{
		LOGE("SpiService JNI: sSpiDevice was not fetched correctly.");
		return -1;
	} else
	{
	//==get data to send=========	
		pTbuff = (char*)malloc(BUF_MAX_SIZE);
		memset(pTbuff, 0, BUF_MAX_SIZE);

		jstring js = (jstring)(env->GetObjectArrayElement( _tbuf, 0));
		pTbuff2 = (char*) (env->GetStringUTFChars( js, 0));
		strcpy(pTbuff, (char *)pTbuff2);
		LOGI("TBuf = %s\n",pTbuff);
	//===========================

	//==prepare data for receive=
	
		pRbuff = (char*)malloc(BUF_MAX_SIZE);
		memset(pRbuff, 0, BUF_MAX_SIZE);
	//===========================

		data = sSpiDevice->transfer( fd, pTbuff, pRbuff, len);
		jstrTmp = env->NewStringUTF( pRbuff);
		env->SetObjectArrayElement( _rbuf, 0, jstrTmp);
		env->DeleteLocalRef(jstrTmp);
		LOGI("RBuf = %s\n",pRbuff);
		free(pTbuff);
		free(pRbuff);
	}
	
	return data;
}
/*
 * Class:     SpiService
 * Method:    _close
 * Signature: (I)Z
 */
static jboolean spi_close(JNIEnv* env, jclass thiz, jint _fd)
{

	int fd = _fd;
	LOGI("SpiService JNI: spi_close() is invoked.");

	if (sSpiDevice == NULL)
	{
		LOGE("SpiService JNI: sSpiDevice was not fetched correctly.");
		return false;
	} else
	{
		if(sSpiDevice->close(fd) == 0)
			return true;
		else
			return false;
	}
}
//============================================================================
static inline int Spi_control_open(const struct hw_module_t* module, struct spi_control_device_t** device)
{
	return module->methods->open(module,
	SPI_HARDWARE_MODULE_ID, (struct hw_device_t**) device);
}

static jboolean spi_init(JNIEnv *env, jclass clazz)
{
	spi_module_t* module;

	if (hw_get_module(SPI_HARDWARE_MODULE_ID, (const hw_module_t**) &module) == 0)
	{
		LOGI("SpiService JNI: Spi Stub found.");
		if (Spi_control_open(&module->common, &sSpiDevice) == 0)
		{
			LOGI("SpiService JNI: Got Stub operations.");
			return true;
		}
	}

	LOGE("SpiService JNI: Get Stub operations failed.");
	return false;
}

// ----------------------------------------------------------------------------

/*
 * Array of methods.
 *
 * Each entry has three fields: the name of the method, the method
 * signature, and a pointer to the native implementation.
 */
static const JNINativeMethod gMethods[] =
	{	
		{ "_init", "()Z", (void*) spi_init },
		{ "_open", "(I)I", (void*) spi_open },
		{ "_config", "(IIII)Z", (void*) spi_config },
		{ "_transfer", "(I[Ljava/lang/String;[Ljava/lang/String;I)I", (void*) spi_transfer },
		{ "_close", "(I)Z", (void*) spi_close }, };

static int registerMethods(JNIEnv* env)
{
	static const char* const kClassName = "com/blus/server/SpiService";
	jclass clazz;

	/* look up the class */
	clazz = env->FindClass(kClassName);
	if (clazz == NULL)
	{
		LOGE("Can't find class %s\n", kClassName);
		return -1;
	}

	/* register all the methods */
	if (env->RegisterNatives(clazz, gMethods, sizeof(gMethods) / sizeof(gMethods[0])) != JNI_OK)
	{
		LOGE("Failed registering methods for %s\n", kClassName);
		return -1;
	}
	LOGI("Register Method succes!\n");

	/* fill out the rest of the ID cache */
	return 0;
}

// ----------------------------------------------------------------------------

/*
 * This is called by the VM when the shared library is first loaded.
 */
jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	JNIEnv* env = NULL;
	jint result = -1;

	if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK)
	{
		LOGE("ERROR: GetEnv failed\n");
		goto bail;
	}
	assert(env != NULL);

	if (registerMethods(env) != 0)
	{
		LOGE("ERROR: PlatformLibrary native registration failed\n");
		goto bail;
	}

	/* success -- return valid version number */
	result = JNI_VERSION_1_4;

	bail: return result;
}
