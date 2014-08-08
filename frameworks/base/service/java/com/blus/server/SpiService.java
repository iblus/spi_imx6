/*
 * Copyright (C) 2014 The blus Open Source Project
 *
 */

package com.blus.server;

import android.util.Config;
import android.util.Log;
import android.content.Context;
import android.os.Binder;
import android.os.Bundle;
import android.os.RemoteException;
import android.os.IBinder;

//import blus.hardware.ILedService;

public final class SpiService // extends ISpiService.Stub
{

	private int fd;
	private boolean isOK;

	static
	{
		System.load("/system/lib/libblus_runtime.so");
	}

	public SpiService()
	{
		Log.i("SpiService", "Go to get SPI Stub...");
		if (init())
		{
			Log.i("SpiService", "SPI Init success!...");
		} else
		{
			Log.i("SpiService", "SPI Init fail!...");
		}
		fd = -1;
		isOK = false;
	}

	public boolean getStatus()
	{
		return isOK;
	}

	/*
	 * blus SPI native methods.
	 */
	public int transfer(String[] tbuf, String[] rbuf, int bytes)
	{
		boolean falg = false;
		Log.i("SpiService", "SPI transfer");
		if (isOK)
		{
			return _transfer(fd, tbuf, rbuf, bytes);
		} else
		{
			return -1;
		}
	}

	public boolean config(int _speed, int _bits_per_word, int _mode)
	{
		boolean falg = false;
		Log.i("SpiService", "SPI config");
		if (isOK)
		{
			falg = _config(fd, _speed, _bits_per_word, _mode);
		}
		return falg;
	}

	public boolean open(int no)
	{
		if (isOK)
		{
			Log.i("SpiService", "SPI devices has beng open");
			return false;
		}
		fd = _open(no);
		if (fd > 0)
		{
			isOK = true;
			Log.i("SpiService", "SPI open success");
		} else
		{
			isOK = false;
			Log.i("SpiService", "SPI open fail");
		}
		return isOK;
	}

	public boolean close()
	{
		if (isOK)
		{
			Log.i("SpiService", "SPI is closing");
			return _close(fd);
		} else
		{
			Log.i("SpiService", "the SPI devices non-existent ");
			return false;
		}
	}

	public boolean init()
	{
		return _init();
	}

	private static native int _open(int fd);

	private static native boolean _init();

	private static native boolean _close(int no);

	private static native boolean _config(int fd, int _speed, int _bits_per_word, int _mode);

	private static native int _transfer(int fd, String[] tbuf, String[] rbuf, int bytes);
}
